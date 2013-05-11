/*******************************************************
* Creator: Richard Kronfalt
* Portability: Native
*-------------------------------------------------------
* 
* Audio-specific buffer producing media node.
*
* Inherits from EMBeOutputNode, and implements the needed
* methods for complementing its parent and thus makes a
* complete media node - in this case, an audio producing
* node.
*
* Richard Kronfält, 00-11-28
*******************************************************/

#include "EMGlobals.h"

#ifdef PLATFORM_BEOS

#ifndef __EM_BE_AUDIO_OUTPUT_NODE
#define __EM_BE_AUDIO_OUTPUT_NODE

#include "EMBeOutputNode.h"

#include "EMBeRealtimeAudioOutputDescriptor.h"

class EMBeNodeConnection;

class EMBeAudioOutputNode : public EMBeOutputNode
{
public:
	EMBeAudioOutputNode(media_input* p_spPhysicalOutInput);
	~EMBeAudioOutputNode();
	bool Initialize();
	void InitializePreferredFormat(media_format* p_spInOutFormat);
	status_t CheckFinalFormat(media_format* p_spInOutFormat);
	int64 GetBufferDuration() const;
	bool Shutdown();

protected:
private:
	EMBeNodeConnection m_oConnection;
	media_input* m_spPhysicalOutInput;
	port_id m_vPortID;
	float m_vAudioFrameRate;
};

#endif

#endif
