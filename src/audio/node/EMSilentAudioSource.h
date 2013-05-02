/*******************************************************
* Creator: Richard Kronfalt
* Portability: Non-Native
*-------------------------------------------------------
* 
* This class inherits from buffer source, and is used
* for producing silence. It is instantiated from the 
* EMMediaProject class (namely in the Run()-method) and
* supplies it with silent audio buffers for sending 
* downstream when there's no "real" audio to 
* produce/process.
*
* Richard Kronfält, 01-01-24
*******************************************************/

#include "EMGlobals.h"

#ifndef __EM_SILENT_AUDIO_SOURCE
#define __EM_SILENT_AUDIO_SOURCE

#include "EMMediaBufferSource.h"

class EMMediaTrack;


class EMSilentAudioSource : public EMMediaBufferSource
{
public:
	EMSilentAudioSource();
	~EMSilentAudioSource();
	EMMediaDataBuffer* GetBuffer() const;
	bool InitCheckE(EMMediaFormat* p_opFormat);

	//EMMediaTrack* GetTrack(void) const {return NULL;}

protected:
private:
};

#endif
