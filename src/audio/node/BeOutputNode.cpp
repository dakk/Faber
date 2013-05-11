#include "EMGlobals.h"

#ifdef PLATFORM_BEOS

#include "EMBeOutputNode.h"
#include <support/ByteOrder.h>
#include <media/Buffer.h>
#include <media/TimeSource.h>
#include <media/MediaDefs.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "EMMediaGlobals.h"
#include "EMMediaFormat.h"
#include "EMBeMediaUtility.h"
#include "EMMediaTimer.h"
#include "EMSettingsRepository.h"
#include "EMSettingIDs.h"
#include "EMMediaEngine.h"
#include "EMMediaProject.h"

EMBeOutputNode::EMBeOutputNode(media_type p_vType)
	:	BMediaNode("Titan Alpha 0.1"),
		BBufferProducer(p_vType),
		BMediaEventLooper(),
		mLatency(0),
		mInternalLatency(0),
		mOutputEnabled(true),
		vCount(127),
		m_vStartingTime(0),
		m_opBuffer(NULL),
		m_opSystemAudioFormat(NULL)
{
	m_vNumberOfReceivedFrames = 0;
	m_vIsClockMaster = false;
	EMBeMediaUtility::push(this, "EMBeOutputNode");
}

EMBeOutputNode::~EMBeOutputNode()
{
	delete m_opSystemAudioFormat;
	EMBeMediaUtility::pop("EMBeOutputNode");
}

bool EMBeOutputNode::Shutdown()
{
	Quit();

	while(EventQueue() -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER) != NULL)
		EventQueue() -> RemoveEvent(EventQueue() -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER));
	
	delete m_opConnectedEMMediaFormat;

	//if(m_opBuffer != NULL)
	//	m_opBuffer -> Recycle();

	return true;
}

BMediaAddOn* EMBeOutputNode::AddOn(int32 *internal_id) const
{
	return NULL;
}

// BBufferProducer methods
status_t EMBeOutputNode::FormatSuggestionRequested(media_type p_sSuggestedType, int32 /*quality*/, media_format* p_spFinalFormat)
{
	// FormatSuggestionRequested() is not necessarily part of the format negotiation
	// process; it's simply an interrogation -- the caller wants to see what the node's
	// preferred data format is, given a suggestion by the caller.
	if(! p_spFinalFormat)
		return B_BAD_VALUE;

	// this is the format we'll be returning (our preferred format)
	memcpy(p_spFinalFormat, &m_sPreferredOutputFormat, sizeof(media_format));

	// a wildcard type is okay; we can specialize it
	if(p_sSuggestedType == B_MEDIA_UNKNOWN_TYPE) 
		p_sSuggestedType = m_sPreferredOutputFormat.type;

	if(p_sSuggestedType != m_sPreferredOutputFormat.type) 
		return B_MEDIA_BAD_FORMAT;
	else 
		return B_OK;
}

status_t EMBeOutputNode::FormatProposal(const media_source& p_sSource, media_format* p_spFinalFormat)
{
	// FormatProposal() is the first stage in the BMediaRoster::Connect() process.  We hand
	// out a suggested format, with wildcards for any variations we support.

	// is this a proposal for our one output?
	if (p_sSource != m_sOutput.source)
	{
		return B_MEDIA_BAD_SOURCE;
	}

	media_type requestedType = p_spFinalFormat -> type;
	memcpy(p_spFinalFormat, &m_sPreferredOutputFormat, sizeof(media_format));
	if((requestedType != B_MEDIA_UNKNOWN_TYPE) && (requestedType != m_sPreferredOutputFormat.type))
	{
		return B_MEDIA_BAD_FORMAT;
	}
	else return B_OK;
}

status_t EMBeOutputNode::FormatChangeRequested(const media_source& source, const media_destination& destination, media_format* io_format, int32* _deprecated_)
{
	// we don't support any other formats, so we just reject any format changes.
	return B_ERROR;
}

status_t EMBeOutputNode::GetNextOutput(int32* cookie, media_output* out_output)
{
	// we have only a single output; if we supported multiple outputs, we'd
	// iterate over whatever data structure we were using to keep track of
	// them.

	if (0 == *cookie)
	{
		memcpy(out_output, &m_sOutput, sizeof(media_output));
		(*cookie) += 1;
		return B_OK;
	}
	else return B_BAD_INDEX;
}

status_t EMBeOutputNode::DisposeOutputCookie(int32 cookie)
{
	// do nothing because we don't use the cookie for anything special
	return B_OK;
}

status_t EMBeOutputNode::SetBufferGroup(const media_source& for_source, BBufferGroup* newGroup)
{
	// verify that we didn't get bogus arguments before we proceed
	if (for_source != m_sOutput.source) return B_MEDIA_BAD_SOURCE;

	// Ahh, someone wants us to use a different buffer group.  At this point we delete
	// the one we are using and use the specified one instead.  If the specified group is
	// NULL, we need to recreate one ourselves, and use *that*.  Note that if we're
	// caching a BBuffer that we requested earlier, we have to Recycle() that buffer
	// *before* deleting the buffer group, otherwise we'll deadlock waiting for that
	// buffer to be recycled!
	if (newGroup != NULL)
	{
		// we were given a valid group; just use that one from now on
	}
	else
	{
		// we were passed a NULL group pointer; that means we construct
		// our own buffer group to use from now on
	}

	return B_OK;
}

status_t EMBeOutputNode::GetLatency(bigtime_t* out_latency)
{
	// report our *total* latency:  internal plus downstream plus scheduling
	(*out_latency) = EventLatency() + SchedulingLatency();
	return B_OK;
}

status_t EMBeOutputNode::PrepareToConnect(const media_source& p_sSource, const media_destination& p_sDestination, media_format* p_spFormat, media_source* p_spOutSource, char* p_vpOutName)
{
	// PrepareToConnect() is the second stage of format negotiations that happens
	// inside BMediaRoster::Connect().  At this point, the consumer's AcceptFormat()
	// method has been called, and that node has potentially changed the proposed
	// format.  It may also have left wildcards in the format.  PrepareToConnect()
	// *must* fully specialize the format before returning!

	// trying to connect something that isn't our source?
	if(p_sSource != m_sOutput.source) 
		return B_MEDIA_BAD_SOURCE;

	// are we already connected?
	if(m_sOutput.destination != media_destination::null) 
		return B_MEDIA_ALREADY_CONNECTED;

	// the format may not yet be fully specialized (the consumer might have
	// passed back some wildcards).  Finish specializing it now, and return an
	// error if we don't support the requested format.
	 // !!! validate all other fields except for buffer_size here, because the consumer might have
	// supplied different values from AcceptFormat()?

	status_t vError = CheckFinalFormat(p_spFormat);
	if(vError != B_OK)
		return vError;
	
	// Now reserve the connection, and return information about it
	memcpy(&(m_sOutput.destination), &p_sDestination, sizeof(media_destination));
	memcpy(&(m_sOutput.format), p_spFormat, sizeof(media_format));
	memcpy(p_spOutSource, &(m_sOutput.source), sizeof(media_source));
	strncpy(p_vpOutName, m_sOutput.name, B_MEDIA_NAME_LENGTH);
	return B_OK; 

}

void EMBeOutputNode::Connect(status_t error, const media_source& p_sSource, const media_destination& p_sDestination, const media_format& p_sFormat, char* p_vpName)
{
/*	if (error)
	{
		m_sOutput.destination = media_destination::null;
		m_sOutput.format = m_sPreferredOutputFormat;
		return;
	}
	
	media_output* spDesiredOutput = NULL;
	if(m_sOutput.source == p_sSource)
		spDesiredOutput = &m_sOutput;
	else
		return;
	
	spDesiredOutput -> destination = media_destination::null;
	spDesiredOutput -> destination = p_sDestination;
	spDesiredOutput -> format = p_sFormat;
	
	strncpy(p_vpName, Name(), B_MEDIA_NAME_LENGTH);

	media_node_id id;
	FindLatencyFor(m_sOutput.destination, &mLatency, &id);
	mInternalLatency = 12000;
	BMediaEventLooper::SetEventLatency(mLatency + mInternalLatency);

	int64 vDuration = EMBeMediaUtility::FramesToTime(EMBeMediaUtility::FramesPerBuffer(GetConnectedEMMediaFormat()), GetConnectedEMMediaFormat());
	if(vDuration < 0)
		vDuration = 40000;
	GetConnectedEMMediaFormat() -> Display();
	
	SetBufferDuration(vDuration); */



	// If something earlier failed, Connect() might still be called, but with a non-zero
	// error code.  When that happens we simply unreserve the connection and do
	// nothing else.
//	if (error)
//	{
//		m_sOutput.destination = media_destination::null;
//		m_sOutput.format = m_sPreferredOutputFormat;
//		return;
//	}

	// Okay, the connection has been confirmed.  Record the destination and format
	// that we agreed on, and report our connection name again.
	memcpy(&(m_sOutput.destination), &p_sDestination, sizeof(media_destination));
	memcpy(&(m_sOutput.format), &p_sFormat, sizeof(media_format));
	strncpy(p_vpName, m_sOutput.name, B_MEDIA_NAME_LENGTH);

	// Now that we're connected, we can determine our downstream latency.
	// Do so, then make sure we get our events early enough.
	media_node_id id;
	FindLatencyFor(m_sOutput.destination, &mLatency, &id);

	mInternalLatency = 2000;
	SetEventLatency(mLatency + mInternalLatency);

	// reset our buffer duration, etc. to avoid later calculations
	bigtime_t duration = GetBufferDuration(); 
	SetBufferDuration(duration);
}

void EMBeOutputNode::Disconnect(const media_source& what, const media_destination& where)
{
	// Make sure that our connection is the one being disconnected
	if ((where == m_sOutput.destination) && (what == m_sOutput.source))
	{
		memcpy(&(m_sOutput.destination), &(media_destination::null), sizeof(media_destination));
		m_sOutput.format = m_sPreferredOutputFormat;
	}
}

void EMBeOutputNode::LateNoticeReceived(const media_source& what, bigtime_t how_much, bigtime_t performance_time)
{
	// If we're late, we need to catch up.  Respond in a manner appropriate to our
	// current run mode.
	if (what == m_sOutput.source)
	{
		if (RunMode() == B_RECORDING)
		{
			// A hardware capture node can't adjust; it simply emits buffers at
			// appropriate points.  We (partially) simulate this by not adjusting
			// our behavior upon receiving late notices -- after all, the hardware
			// can't choose to capture "sooner"....
		}
		else if (RunMode() == B_INCREASE_LATENCY)
		{
			// We're late, and our run mode dictates that we try to produce buffers
			// earlier in order to catch up.  This argues that the downstream nodes are
			// not properly reporting their latency, but there's not much we can do about
			// that at the moment, so we try to start producing buffers earlier to
			// compensate.
			mInternalLatency += how_much;
			SetEventLatency(mLatency + mInternalLatency);
		}
		else
		{
			// The other run modes dictate various strategies for sacrificing data quality
			// in the interests of timely data delivery.  The way *we* do this is to skip
			// a buffer, which catches us up in time by one buffer duration.
		}
	}
}

void EMBeOutputNode::EnableOutput(const media_source& what, bool enabled, int32* _deprecated_)
{
	// If I had more than one output, I'd have to walk my list of output records to see
	// which one matched the given source, and then enable/disable that one.  But this
	// node only has one output, so I just make sure the given source matches, then set
	// the enable state accordingly.
	if (what == m_sOutput.source)
	{
//		mOutputEnabled = enabled;
	}
}

status_t EMBeOutputNode::SetPlayRate(int32 numer, int32 denom)
{
	// Play rates are weird.  We don't support them.  Maybe we will in a
	// later newsletter article....
	return B_ERROR;
}

void EMBeOutputNode::AdditionalBufferRequested(const media_source& source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag* prev_tag)
{
	// we don't support offline mode (yet...)
	return;
}

void EMBeOutputNode::LatencyChanged(const media_source& source, const media_destination& destination, bigtime_t new_latency, uint32 flags)
{
	// something downstream changed latency, so we need to start producing
	// buffers earlier (or later) than we were previously.  Make sure that the
	// connection that changed is ours, and adjust to the new downstream
	// latency if so.
	
	if ((source == m_sOutput.source) && (destination == m_sOutput.destination))
	{
		mLatency = new_latency;
		SetEventLatency(mLatency + mInternalLatency);
	}
}

// Workaround for a Metrowerks PPC compiler bug
status_t EMBeOutputNode::DeleteHook(BMediaNode* node)
{
	return BMediaEventLooper::DeleteHook(node);
}

// BMediaEventLooper methods
void EMBeOutputNode::NodeRegistered()
{
	// set up as much information about our output as we can
	m_sOutput.source.port = ControlPort();
	m_sOutput.source.id = 0;
	memcpy(&(m_sOutput.node), &(Node()), sizeof(media_node));
	
	::strcpy(m_sOutput.name, "Titan Alpha 0.1");

	// Start the BMediaEventLooper thread
	SetPriority(B_REAL_TIME_PRIORITY);
	Run();
}

status_t EMBeOutputNode::AddTimer(bigtime_t at_performance_time, int32 cookie)
{
	// A bug in the current PowerPC compiler demands that we implement
	// this, even though it just calls up to the inherited implementation.
	return BMediaEventLooper::AddTimer(at_performance_time, cookie);
}

void EMBeOutputNode::SetRunMode(run_mode mode)
{
	// We don't support offline run mode, so broadcast an error if we're set to
	// B_OFFLINE.  Unfortunately, we can't actually reject the mode change...
	if(mode == B_OFFLINE)
	{
		ReportError(B_NODE_FAILED_SET_RUN_MODE);
		EMDebugger("ERROR! We don't like OFFLINE MODE!");
	}
	BMediaEventLooper::SetRunMode(mode);
}

media_format EMBeOutputNode::GetConnectedFormat()
{
	media_format sFormat;
	if(m_sOutput.destination != media_destination::null)
		return m_sOutput.format;
	return m_sPreferredOutputFormat;
}

EMMediaFormat* EMBeOutputNode::GetConnectedEMMediaFormat()
{
	if(m_opConnectedEMMediaFormat != NULL)
		return m_opConnectedEMMediaFormat;

	m_opConnectedEMMediaFormat = new EMMediaFormat((m_sPreferredOutputFormat.type == B_MEDIA_RAW_AUDIO ? EM_TYPE_RAW_AUDIO : EM_TYPE_RAW_VIDEO));
	m_opConnectedEMMediaFormat -> CreateFormat(&(m_sOutput.format));
	return m_opConnectedEMMediaFormat;
}

void EMBeOutputNode::Start(bigtime_t performance_time)
{
	// A bug in the current PowerPC compiler demands that we implement
	// this, even though it just calls up to the inherited implementation.
	BMediaEventLooper::Start(performance_time);
}

void EMBeOutputNode::Stop(bigtime_t performance_time, bool immediate)
{
	// A bug in the current PowerPC compiler demands that we implement
	// this, even though it just calls up to the inherited implementation.
	BMediaEventLooper::Stop(performance_time, immediate);
}

void FlushBuffers(BTimedEventQueue* Q)
{
	media_timed_event e;
	while(Q -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER) != NULL)
	{
		e = *(Q -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER));
		Q -> RemoveEvent(&e);
		BBuffer* buff = static_cast<BBuffer*>(e.pointer);
		if(buff != NULL)
			buff -> Recycle();
	}
/*
	//This algorithm removes all but the FIRST bbuffer event!
	media_timed_event sBufferEventToSave;
	bool vInclusive = true;
	int64 vTimeOfFirstBufferEvent = 0;
	if(Q -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER) != NULL)
	{
		sBufferEventToSave = *Q -> FindFirstMatch(0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER);
		vInclusive = false;
		vTimeOfFirstBufferEvent = sBufferEventToSave.event_time;
	}
		
	media_timed_event sEventToFlush;
	while(Q -> FindFirstMatch(vTimeOfFirstBufferEvent, BTimedEventQueue::B_AFTER_TIME, false, BTimedEventQueue::B_HANDLE_BUFFER) != NULL)
	{
		sEventToFlush = *Q -> FindFirstMatch(vTimeOfFirstBufferEvent, BTimedEventQueue::B_AFTER_TIME, vInclusive, BTimedEventQueue::B_HANDLE_BUFFER);
		Q -> RemoveEvent(&sEventToFlush);
		BBuffer* opBufferToRecycle = static_cast<BBuffer*>(sEventToFlush.pointer);
		if(opBufferToRecycle != NULL)
			opBufferToRecycle -> Recycle();
		
	}
*/
}

status_t EMBeOutputNode::HandleMessage(int32 message, const void* data, size_t size)
{
	if(message == EM_PORT_MESSAGE_INCOMING_BUFFER)
	{
		m_opBuffer = *((BBuffer**) data);
		if(RunState() != B_STARTED)
		{
			emerr << "ERROR! Node received buffer when not running!" << endl;
			m_opBuffer -> Recycle();
			return B_OK;
		}

		if(m_opBuffer != NULL)
		{
			vCount++;
			EMMediaFormat* opFormat = GetConnectedEMMediaFormat();
			if((opFormat -> m_eType & EM_TYPE_ANY_VIDEO) > 0)
			{
				opFormat -> m_vFrameRate = *(static_cast<float*>(EMMediaEngine::Instance() -> GetSettingsRepository() -> GetSetting(SETTING_VIDEO_FRAMERATE)));			
			}

			int64 vFrames = EMBeMediaUtility::FramesInBuffer(m_opBuffer, opFormat, EM_TYPE_RAW_AUDIO); // + 44100 / 4;
			m_vNumberOfReceivedFrames += vFrames;
			int64 vTime = EMBeMediaUtility::FramesToTime(m_vNumberOfReceivedFrames, m_opSystemAudioFormat);
			int64 vNextBufferPerf = static_cast<uint64>(floor(static_cast<float>(m_vStartingTime) + vTime));
			
			m_opBuffer -> Header() -> start_time = vNextBufferPerf;
			
			media_timed_event sEvent(vNextBufferPerf, BTimedEventQueue::B_HANDLE_BUFFER);
			sEvent.pointer = m_opBuffer;
			sEvent.cleanup = BTimedEventQueue::B_RECYCLE_BUFFER;
			status_t vResult = EventQueue() -> AddEvent(sEvent);
			if(vResult != B_OK)
			{
				emerr << "ERROR! Couldn't add the B_HANDLE_BUFFER event to the queue: " << strerror(vResult) << endl;
				m_opBuffer -> Recycle();
			}
		}
		else
			emerr << "ERROR! Node received invalid EM_NODE_BUFFER_ADDED message!" << endl;
		return B_OK;
	}
	else if(message == EM_PORT_MESSAGE_FLUSH_QUEUE)
	{
		media_timed_event sEvent(TimeSource() -> Now(), EM_TIMED_EVENT_FLUSH_CASH);
		EventQueue() -> AddEvent(sEvent);
	}
	else if(message == EM_PORT_MESSAGE_RESET)
	{
		media_timed_event sEvent(TimeSource() -> Now(), EM_TIMED_EVENT_RESET);
		EventQueue() -> AddEvent(sEvent);
	}
	else 
	{
		;//emout_commented_out_4_release << "ERROR! Received unknown message!" << endl;
		BMediaNode::HandleBadMessage(message, data, size);
	}
	return B_OK;
}

void EMBeOutputNode::HandleEvent(const media_timed_event* event, bigtime_t lateness, bool realTimeEvent)
{
	switch(event -> type)
	{
		case BTimedEventQueue::B_START:
		{
			if(RunState() != B_STARTED)
			{
//				mOutputEnabled = true;
				m_vNumberOfReceivedFrames = 0;
				m_vStartingTime = TimeSource() -> Now(); //event -> event_time;
				vCount = 0;
				SendDataStatus(B_DATA_AVAILABLE, m_sOutput.destination, event -> event_time);
			}
			break;
		}
		case BTimedEventQueue::B_STOP:
		{
			FlushBuffers(EventQueue());
			SendDataStatus(B_PRODUCER_STOPPED, m_sOutput.destination, event -> event_time);
			break;
		}
		case EM_TIMED_EVENT_RESET:
		{
			FlushBuffers(EventQueue());
			m_vNumberOfReceivedFrames = 0;
//			m_vStartingTime = TimeSource() -> Now(); //event -> event_time;
			m_vStartingTime = event -> event_time;
			vCount = 0;		
		}
		case BTimedEventQueue::B_HANDLE_BUFFER:
		{
			BBuffer* opBuffer = (BBuffer*) (event -> pointer);
			if(opBuffer == NULL)
			{
				emerr << "ERROR! Received NULL BBuffer in HandleEvent!" << endl;
				break;
			}
			
			if((RunState() == BMediaEventLooper::B_STARTED) && 
				(m_sOutput.destination != media_destination::null))
			{
				int64 vFrames = EMBeMediaUtility::FramesInBuffer(opBuffer, GetConnectedEMMediaFormat(), EM_TYPE_RAW_AUDIO);
				if(mOutputEnabled) 
				{
					opBuffer -> Header() -> start_time = event -> event_time;
					opBuffer -> Header() -> type = m_sOutput.format.type;
					
					status_t err = SendBuffer(opBuffer, m_sOutput.destination);
					if(err != B_OK)
					{
						emerr << "ERROR! Could not send buffer to downstream node: " << strerror(err) << endl;
						opBuffer -> Recycle();
					}
					else
					{
						bool m_vIsCurrentlyRecording = (EMMediaEngine::Instance() -> GetMediaProject() -> IsArmed() && EMMediaEngine::Instance() -> GetMediaProject() -> IsPlaying());
//						bool m_vIsCurrentlyPlaying = (! EMMediaEngine::Instance() -> GetMediaProject() -> IsArmed() && EMMediaEngine::Instance() -> GetMediaProject() -> IsPlaying());
						if(m_vIsClockMaster && ! m_vIsCurrentlyRecording)
							EMMediaTimer::Instance() -> IncreaseNowFrame(vFrames);
					}
				}
				else
					opBuffer -> Recycle();
			}
			else
				opBuffer -> Recycle();
			break;
		}
		case EM_TIMED_EVENT_FLUSH_CASH:
		{
			FlushBuffers(EventQueue());
			m_vStartingTime = event -> event_time;
			m_vNumberOfReceivedFrames = 0;
			Notify(EM_TIMED_EVENT_FLUSH_CASH);			
			break;
		}
		default:
		{	
			;//emout_commented_out_4_release << "ERROR! EMBeOutoutNode received unknown event!" << endl;
			break;
		}
	}
}

void EMBeOutputNode::SetClockMaster(bool p_vFlag)
{
	m_vIsClockMaster = p_vFlag;
}

bool EMBeOutputNode::IsClockMaster() const
{
	return m_vIsClockMaster;
}

bool EMBeOutputNode::IsRunning() const
{
	if(RunState() == BMediaEventLooper::B_STARTED)
		return true;
	else
		return false;
}

bool EMBeOutputNode::Initialize()
{
	InitializePreferredFormat(&m_sPreferredOutputFormat);
	memcpy(&(m_sOutput.destination), &(media_destination::null), sizeof(media_destination));
	memcpy(&(m_sOutput.format), &m_sPreferredOutputFormat, sizeof(media_format));
	m_opConnectedEMMediaFormat = NULL;
	m_opSystemAudioFormat = new EMMediaFormat(EM_TYPE_RAW_AUDIO);
	return true;
}

#endif