/*
	EMBeOutputNode.h

	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#include "EMGlobals.h"

#ifdef PLATFORM_BEOS

#ifndef __EM_BE_OUTPUT_NODE
#define __EM_BE_OUTPUT_NODE

#include "EMListenerRepository.h"
#include "EMMediaFormat.h"

#include <media/BufferProducer.h>
#include <media/MediaEventLooper.h>
#include <list>

class EMMediaFormat;

class EMBeOutputNode : public BBufferProducer,  public BMediaEventLooper, public EMListenerRepository
{
public:
// BMediaNode methods
	BMediaAddOn* AddOn(	int32* internal_id) const;

// BBufferProducer methods
	status_t FormatSuggestionRequested(media_type type, int32 quality, media_format* format);
	status_t FormatProposal(const media_source& output, media_format* format);

	/* If the format isn't good, put a good format into *io_format and return error */
	/* If format has wildcard, specialize to what you can do (and change). */
	/* If you can change the format, return OK. */
	/* The request comes from your destination sychronously, so you cannot ask it */
	/* whether it likes it -- you should assume it will since it asked. */
	status_t FormatChangeRequested(const media_source& source, const media_destination& destination, media_format* io_format, int32* _deprecated_);
	status_t GetNextOutput(int32* cookie, media_output* out_output);
	status_t DisposeOutputCookie(int32 cookie);
	status_t SetBufferGroup(const media_source& for_source, BBufferGroup* group);

	/* Iterates over all outputs and maxes the latency found */
	status_t GetLatency(bigtime_t* out_latency);
	status_t PrepareToConnect(const media_source& what, const media_destination& where, media_format* format, media_source* out_source,	char* out_name);
	void Connect(status_t error, const media_source& source, const media_destination& destination, const media_format& format, char* io_name);
	void Disconnect(const media_source& what, const media_destination& where);
	void LateNoticeReceived(const media_source& what, bigtime_t how_much, bigtime_t performance_time);
	void EnableOutput(const media_source & what, bool enabled, int32* _deprecated_);
	status_t SetPlayRate(int32 numer, int32 denom);
	status_t HandleMessage(int32 message, const void* data, size_t size);
	void AdditionalBufferRequested(const media_source& source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag* prev_tag);	//	may be NULL
	void LatencyChanged(const media_source& source, const media_destination& destination, bigtime_t new_latency, uint32 flags);
// BMediaEventLooper methods
	void NodeRegistered();
	status_t AddTimer(bigtime_t at_performance_time, int32 cookie);
	void SetRunMode(run_mode mode);
	void HandleEvent(const media_timed_event* event, bigtime_t lateness, bool realTimeEvent = false);
	void Start(bigtime_t performance_time);
	void Stop(bigtime_t performance_time, bool immediate);

	media_format GetConnectedFormat();
	EMMediaFormat* GetConnectedEMMediaFormat();
//	bool Shutdown() { return true; };
	bool CheckOutputFormat(media_format* p_spFinalFormat) { return true; };
	void SetClockMaster(bool p_vFlag);
	bool IsClockMaster() const;
	bool IsRunning() const;

	virtual bool Initialize(); //Inherit and write connect-stuff. Remember to call my (i.e. parent's) implementation FIRST!
	virtual void InitializePreferredFormat(media_format* p_spInOutFormat) = 0; //Set the parameter with the stuff you like
	virtual status_t CheckFinalFormat(media_format* p_spInOutFormat) = 0; //Is the format ok? Return native status_t codes
	virtual int64 GetBufferDuration() const = 0; //Return how long, in uS one media buffer is.
	
	virtual bool Shutdown();

protected:
	EMBeOutputNode(media_type p_vType);
	virtual ~EMBeOutputNode();
	status_t DeleteHook(BMediaNode* node);
	media_output m_sOutput;
	EMMediaFormat* m_opConnectedEMMediaFormat;
	int64 m_vNumberOfReceivedFrames;

private:
	media_format m_sPreferredOutputFormat;
	bigtime_t mLatency, mInternalLatency;
	media_output mOutput;
	bool mOutputEnabled;
	bool m_vIsClockMaster;
	unsigned short vCount;
	bigtime_t m_vStartingTime;
	BBuffer* m_opBuffer;
	EMMediaFormat* m_opSystemAudioFormat;
};

#endif

#endif