#include "EMGlobals.h"

#include "EMAudioMixFunctions.h"
#include "EMMediaUtility.h"

#include <iostream>
#include <string>

EMAudioMixFunctions::EMAudioMixFunctions()
{
}

EMAudioMixFunctions::~EMAudioMixFunctions()
{
}

EMMediaDataBuffer* EMAudioMixFunctions::AudioMixPreFader(list<EMMediaDataBuffer*>* p_opList)
{
//	uint32 vPosition = 0;
	EMMediaDataBuffer* opTarget = p_opList -> front(); //p_opList[vPosition]; // -> front();
	p_opList -> pop_front();
//	vPosition++;

	uint64 vMax = opTarget -> m_vSizeUsed / sizeof(signed short);
	signed short* vpTargetPtr = static_cast<signed short*>(opTarget -> Data());
	while(p_opList -> size() > 0) //[vPosition] != NULL)
	{
		EMMediaDataBuffer* opSource = p_opList -> front(); //[vPosition];
		signed short* vpSourcePtr = static_cast<signed short*>(opSource -> Data());
		for(; vpTargetPtr != static_cast<signed short*>(opTarget -> Data()) + vMax; vpTargetPtr++, vpSourcePtr++)
			(*vpTargetPtr) += (*vpSourcePtr);
		opSource -> Recycle();
		//vPosition++;
		p_opList -> pop_front();
		vpTargetPtr -= vMax;
	}
	return opTarget;
}

EMMediaDataBuffer* EMAudioMixFunctions::AudioMixPostFader(list<EMMediaDataBuffer*>* p_opList)
{
//	uint32 vPosition = 0;
	EMMediaDataBuffer* opTarget = p_opList -> front(); //[vPosition]; // -> front();
	p_opList -> pop_front();
//	vPosition++;

	if(opTarget == NULL)
		;//eo << "ERROR! Target is NULL in AudioMixPostFader!" << ef;

	//Calculate the fade- and pan-factors, if needed
	float vFaderFactor = 1.0;
	if(opTarget -> m_vFader != -1 && opTarget -> m_vFader != 127)
		vFaderFactor = opTarget -> m_vFader / 127.0;

	float vLeftPanFactor = 1.0 * vFaderFactor;
	float vRightPanFactor = 1.0 * vFaderFactor;
	if(opTarget -> m_vPan != 0)
	{
		if(opTarget -> m_vPan < 0) //Dim right channel, since we're panning left!
			vRightPanFactor = vFaderFactor * (1.0 + (static_cast<float>(opTarget -> m_vPan) / 63.0));
		else if(opTarget -> m_vPan > 0) //Dim left channel, since we're panning right
			vLeftPanFactor = vFaderFactor * (1.0 + (static_cast<float>(opTarget -> m_vPan) / -63.0));
	}

	signed short* vpTargetPtr = static_cast<signed short*>(opTarget -> Data());
	uint64 vMax = opTarget -> m_vSizeUsed / sizeof(signed short);
	
	//We mustn't forget to apply pan and fader to the first buffer as well, coz the while-loop a few rows
	//down only applies it to the remaining buffers in the list (and we've only taken the first one out!).
	for(; vpTargetPtr < static_cast<signed short*>(opTarget -> Data()) + vMax; vpTargetPtr += 2)
	{
		(*vpTargetPtr) = static_cast<signed short>(static_cast<float>((*vpTargetPtr)) * vLeftPanFactor);
		(*(vpTargetPtr + 1)) = static_cast<signed short>(static_cast<float>((*(vpTargetPtr + 1))) * vRightPanFactor);
	}
	
	while(p_opList -> size() > 0) // != NULL)
	{
		EMMediaDataBuffer* opSource = p_opList -> front(); //[vPosition];
		p_opList -> pop_front();
//		vPosition++;

		vFaderFactor = 1.0;
		if(opSource -> m_vFader != -1 && opSource -> m_vFader != 127)
			vFaderFactor = opSource -> m_vFader / 127.0;

		vLeftPanFactor = 1.0 * vFaderFactor;
		vRightPanFactor = 1.0 * vFaderFactor;
		if(opSource -> m_vPan != 0)
		{
			if(opSource -> m_vPan < 0) //Dim right channel, since we're panning left!
				vRightPanFactor = vFaderFactor * (1.0 + (static_cast<float>(opSource -> m_vPan) / 63.0));
			else if(opSource -> m_vPan > 0) //Dim left channel, since we're panning right
				vLeftPanFactor = vFaderFactor * (1.0 + (static_cast<float>(opSource -> m_vPan) / -63.0));
		}

		vpTargetPtr = static_cast<signed short*>(opTarget -> Data());
		signed short* vpSourcePtr = static_cast<signed short*>(opSource -> Data());
		signed short* vpEnd = static_cast<signed short*>(opTarget -> Data()) + vMax;

		for(; vpTargetPtr != vpEnd; vpTargetPtr += 2, vpSourcePtr += 2)
		{
			(*vpTargetPtr) += static_cast<signed short>(static_cast<float>((*vpSourcePtr)) * vLeftPanFactor);
			(*(vpTargetPtr + 1)) += static_cast<signed short>(static_cast<float>((*(vpSourcePtr + 1))) * vRightPanFactor);
		}
		
		if(opSource -> m_vShouldBeDeleted)
			delete opSource;
		else
			opSource -> Recycle();
	}

	opTarget -> m_vFader = -1; //Fader applied!
	opTarget -> m_vPan = 0; //Pan applied!
	return opTarget;
}
