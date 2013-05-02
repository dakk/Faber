/*******************************************************
* Creator: Richard Kronfalt
* Portability: Non-Native
*-------------------------------------------------------
* 
*******************************************************/

#include "EMGlobals.h"

#ifndef __EM_AUDIO_MIX_FUNCTIONS
#define __EM_AUDIO_MIX_FUNCTIONS

#include "EMMediaGlobals.h"
#include "EMMediaDataBuffer.h"

#include <list>

class EMAudioMixFunctions 
{
public:
	~EMAudioMixFunctions();
	static EMMediaDataBuffer* AudioMixPreFader(list<EMMediaDataBuffer*>* p_opList);
	static EMMediaDataBuffer* AudioMixPostFader(list<EMMediaDataBuffer*>* p_opList);

private:
	EMAudioMixFunctions();
};

#endif
