#include "EMGlobals.h"

#ifdef PLATFORM_BEOS

#include "EMBeAudioOutputNode.h"
#include "EMSettingIDs.h"
#include "EMSettingsRepository.h"
#include "EMMediaEngine.h"

#include "EMBeMediaUtility.h"
#include <iostream>

EMBeAudioOutputNode::EMBeAudioOutputNode(media_input* p_spPhysicalOutInput)
	:	BMediaNode("Titan stream mixer node"),
		EMBeOutputNode(B_MEDIA_RAW_AUDIO),
		m_spPhysicalOutInput(p_spPhysicalOutInput)
{
	m_vAudioFrameRate = (float) *(static_cast<int32*>(EMMediaEngine::Instance() -> GetSettingsRepository() -> GetSetting(SETTING_AUDIO_FRAMERATE)));
	EMBeMediaUtility::push(this, "EMBeAudioOutputNode");
}

EMBeAudioOutputNode::~EMBeAudioOutputNode()
{
	EMBeMediaUtility::pop("EMBeAudioOutputNode");
}

bool EMBeAudioOutputNode::Shutdown()
{
	status_t e = EMBeMediaUtility::GetRosterE() -> StopNode(Node(), true);
	if(e != B_OK)
		EMDebugger((string(string("ERROR! Could not stop node: ") + string(strerror(e)))).c_str());
	status_t vErrorCode = EMBeMediaUtility::GetRosterE() -> Disconnect(m_oConnection.m_sProducer.node, m_oConnection.m_sOutput.source, m_oConnection.m_sConsumer.node, m_oConnection.m_sInput.destination); //.source, m_oConnection.m_sInput.destination, &m_oConnection.m_sFormat, &m_oConnection.m_sOutput, &m_oConnection.m_sInput);
	if(vErrorCode != B_OK)
		EMDebugger("ERROR! Could not disconnect Titan's audio output from the system's audio output!");
	EMBeOutputNode::Shutdown();
	return true;
}

bool EMBeAudioOutputNode::Initialize()
{
	EMBeMediaUtility::GetRosterE() -> RegisterNode(this);
	
	//First, call parent's implementation
	if(! EMBeOutputNode::Initialize())
	{
		emerr << "ERROR! EMBeAudioOutputNode could not initialize parent!" << endl;
		return false;
	}

	media_node mTimeSource;
	memcpy(&(m_oConnection.m_sProducer), &(Node()), sizeof(media_node));
	memcpy(&(m_oConnection.m_sConsumer), &(m_spPhysicalOutInput -> node), sizeof(media_node));
	EMBeMediaUtility::GetRosterE() -> GetTimeSource(&mTimeSource);
	EMBeMediaUtility::GetRosterE() -> SetTimeSourceFor(m_oConnection.m_sProducer.node, mTimeSource.node);
	BTimeSource* opTimeSource = EMBeMediaUtility::GetRosterE() -> MakeTimeSourceFor(m_oConnection.m_sProducer);

/*	if(EMBeMediaUtility::GetRosterE() -> SetTimeSourceFor(m_oConnection.m_sConsumer.node, opTimeSource -> Node().node) != B_OK)
		EMDebugger("ERROR! Could not set time source for consumer node!");
	EMBeMediaUtility::GetRosterE() -> StartNode(opTimeSource -> Node(), 0);
	opTimeSource -> Release();
*/
	int32 count = 1;
	status_t err = EMBeMediaUtility::GetRosterE() -> GetFreeOutputsFor(Node()/*m_oConnection.m_sProducer*/, &m_oConnection.m_sOutput, 1, &count);
	memcpy(&(m_oConnection.m_sInput), m_spPhysicalOutInput, sizeof(media_input));
	m_oConnection.m_sFormat.type = B_MEDIA_RAW_AUDIO;	
	memcpy(&(m_oConnection.m_sFormat.u.raw_audio), &(media_raw_audio_format::wildcard), sizeof(media_raw_audio_format));

	err = EMBeMediaUtility::GetRosterE() -> Connect(m_oConnection.m_sOutput.source, m_oConnection.m_sInput.destination, &m_oConnection.m_sFormat, &m_oConnection.m_sOutput, &m_oConnection.m_sInput);
	if(err != B_OK)
		EMDebugger("ERROR! Could not connect audio titan output to system audio output!");

	memcpy(&(m_oConnection.m_sSource), &(m_oConnection.m_sOutput.source), sizeof(media_source));
	memcpy(&(m_oConnection.m_sDestination), &(m_oConnection.m_sOutput.destination), sizeof(media_destination));

	EMBeMediaUtility::GetRosterE() -> SetRunModeNode(m_oConnection.m_sProducer, BMediaNode::B_INCREASE_LATENCY);
	return true;
}

void EMBeAudioOutputNode::InitializePreferredFormat(media_format* p_spInOutFormat)
{
	p_spInOutFormat -> u.raw_audio.buffer_size = media_raw_audio_format::wildcard.buffer_size;
	p_spInOutFormat -> type = B_MEDIA_RAW_AUDIO;
	p_spInOutFormat -> u.raw_audio.format = media_raw_audio_format::B_AUDIO_SHORT;
	p_spInOutFormat -> u.raw_audio.channel_count = EM_AUDIO_NUM_CHANNELS;
	float vAudioFrameRate = (float) *(static_cast<int32*>(EMMediaEngine::Instance() -> GetSettingsRepository() -> GetSetting(SETTING_AUDIO_FRAMERATE)));
	p_spInOutFormat -> u.raw_audio.frame_rate = vAudioFrameRate;
	p_spInOutFormat -> u.raw_audio.byte_order = B_MEDIA_LITTLE_ENDIAN;
	p_spInOutFormat -> u.raw_audio.buffer_size = EM_AUDIO_BUFFER_SIZE;
}

status_t EMBeAudioOutputNode::CheckFinalFormat(media_format* p_spInOutFormat)
{
	if(p_spInOutFormat -> type != B_MEDIA_RAW_AUDIO)
		return B_MEDIA_BAD_FORMAT;
	else if(p_spInOutFormat -> u.raw_audio.format != media_raw_audio_format::B_AUDIO_SHORT)
		return B_MEDIA_BAD_FORMAT;

	if(p_spInOutFormat -> u.raw_audio.buffer_size == media_raw_audio_format::wildcard.buffer_size)
		p_spInOutFormat -> u.raw_audio.buffer_size = EM_AUDIO_BUFFER_SIZE; // pick something comfortable to suggest
	
	return B_OK;
}

int64 EMBeAudioOutputNode::GetBufferDuration() const
{
	return static_cast<int64>(ceil(1000000.0 * (EM_AUDIO_BUFFER_SIZE / 4) / m_vAudioFrameRate));
}


#endif
