#include "EMSilentAudioSource.h"

#include "EMMediaDataBuffer.h"
#include "EMMediaTimer.h"
#include "EMBeMediaUtility.h"
#include "EMMediaFormat.h"

EMSilentAudioSource::EMSilentAudioSource()
	:	EMMediaBufferSource(EM_TYPE_RAW_AUDIO)
{
}

EMSilentAudioSource::~EMSilentAudioSource() //Deleted by MediaProject
{
}

bool EMSilentAudioSource::InitCheckE(EMMediaFormat* p_opFormat)
{
	EMMediaFormat* opFormat = EM_new EMMediaFormat(EM_TYPE_ANY_AUDIO);
	if(! EMMediaBufferSource::InitCheckE(opFormat))
	{
		delete opFormat;
		return false;
	}

	SetBufferSilence(true);
	SetBufferFormat(opFormat);
	delete opFormat;
	return true;
}

EMMediaDataBuffer* EMSilentAudioSource::GetBuffer() const
{
	EMMediaDataBuffer* opBuffer = EMMediaBufferSource::GetBuffer();
	opBuffer -> m_vAvailable = false;
	opBuffer -> m_vSizeUsed = opBuffer -> m_vSizeAvailable;
	memset(opBuffer -> Data(), 0, opBuffer -> m_vSizeUsed);
	opBuffer -> SetFrame(EMMediaTimer::Instance() -> AudioThenFrame());
	return opBuffer;
}
