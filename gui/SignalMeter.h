/*******************************************************
* Creator: Martin Rudefelt
* Portability: non-native
*-------------------------------------------------------
*
*******************************************************/

#include "EMGlobals.h"

#ifndef __EM_SIGNAL_METER
#define __EM_SIGNAL_METER

#include "EMRect.h"

//#include <list>

class EMListenerRepository;
class EMView;

enum SignalMeterMode
{
	SIGNAL_METER_DRAW_NORMAL,
	SIGNAL_METER_DRAW_BLANK
};

class EMSignalMeter
{
public:
	EMSignalMeter(EMRect p_oFrame, EMView* p_opView);
	void Draw(EMRect p_oClippingRect);
	EMRect Frame();
	bool GetVisible();
	void SetFrame(EMRect m_oFrame);
	void SetMode(SignalMeterMode p_eMode);
	void SetSignal(float p_vSignal);
	void SetSignal(float* p_vpSignal);//list<float>* p_vSignal);
	void SetVisible(bool p_vVisible);

private:
	SignalMeterMode m_eMode;
	EMRect m_oFrame;
	EMRect m_oFrameLeft;
	EMRect m_oFrameRight;
	EMView* m_opView;
	float m_vSignalLeft;
	int m_vSignalPeakCounter;
	float m_vSignalPeakLeft;
	float m_vSignalPeakRight;
	float m_vSignalRight;
	bool m_vVisible;
};

#endif

