#include "EMSignalMeter.h"

#include "EMGUIGlobals.h"
#include "EMView.h"

const float SIGNAL_DECAY_RATE = static_cast<const float>(0.02);
const int SIGNAL_PEAK_TIME = 50;

EMSignalMeter::EMSignalMeter(EMRect p_oFrame, EMView* p_opView) :
m_eMode(SIGNAL_METER_DRAW_NORMAL),
m_oFrame(p_oFrame),
m_oFrameLeft(m_oFrame.m_vLeft, m_oFrame.m_vTop, m_oFrame.m_vRight, m_oFrame.m_vTop + (m_oFrame.GetHeight() / 2)),
m_oFrameRight(m_oFrame.m_vLeft, m_oFrameLeft.m_vBottom + 1, m_oFrame.m_vRight, m_oFrame.m_vBottom),
m_opView(p_opView),
m_vSignalLeft(0),
m_vSignalPeakCounter(SIGNAL_PEAK_TIME),
m_vSignalPeakLeft(0),
m_vSignalPeakRight(0),
m_vSignalRight(0),
m_vVisible(true)
{
}

void EMSignalMeter::Draw(EMRect p_oClippingRect)
{
	if(!m_vVisible)
		return;
	
	if(!m_oFrame.Intersects(p_oClippingRect))
		return;

	if(m_eMode == SIGNAL_METER_DRAW_NORMAL)
	{
		EMRect oSignalArea(m_oFrameLeft.m_vLeft, m_oFrameLeft.m_vTop + 1, m_oFrameLeft.m_vLeft + m_oFrameLeft.GetWidth() * m_vSignalLeft, m_oFrameLeft.m_vBottom - 1);
		EMRect oEmptyArea(oSignalArea.m_vRight + 1, m_oFrameLeft.m_vTop,  m_oFrameLeft.m_vRight, m_oFrameLeft.m_vBottom);
		m_opView -> SetHighColor(EMColor(255, 130, 130, 255));
		m_opView -> StrokeLine(m_oFrameLeft.m_vLeft, m_oFrameLeft.m_vTop, m_oFrameLeft.m_vLeft + m_oFrameLeft.GetWidth() * m_vSignalLeft, m_oFrameLeft.m_vTop);
		m_opView -> SetHighColor(EMColor(100, 0, 0, 255));
		m_opView -> StrokeLine(m_oFrameLeft.m_vLeft, m_oFrameLeft.m_vBottom, m_oFrameLeft.m_vLeft + m_oFrameLeft.GetWidth() * m_vSignalLeft, m_oFrameLeft.m_vBottom);
		m_opView -> SetHighColor(EMColor(255, 0, 0, 255));
		m_opView -> FillRect(oSignalArea);
		m_opView -> SetHighColor(EMColor(0, 0, 0, 255));
		m_opView -> FillRect(oEmptyArea);
		m_opView -> SetHighColor(EMColor(255, 0, 0, 255));
		m_opView -> StrokeLine(m_oFrameLeft.m_vLeft + m_oFrameLeft.GetWidth() * m_vSignalPeakLeft, m_oFrameLeft.m_vTop, m_oFrameLeft.m_vLeft + m_oFrameLeft.GetWidth() * m_vSignalPeakLeft, m_oFrameLeft.m_vBottom);

		oSignalArea = EMRect(m_oFrameRight.m_vLeft, m_oFrameRight.m_vTop + 1, m_oFrameRight.m_vLeft + m_oFrameRight.GetWidth() * m_vSignalRight, m_oFrameRight.m_vBottom - 1);
		oEmptyArea = EMRect(oSignalArea.m_vRight + 1, m_oFrameRight.m_vTop,  m_oFrameRight.m_vRight, m_oFrameRight.m_vBottom);
		m_opView -> SetHighColor(EMColor(255, 130, 130, 255));
		m_opView -> StrokeLine(m_oFrameRight.m_vLeft, m_oFrameRight.m_vTop, m_oFrameRight.m_vLeft + m_oFrameRight.GetWidth() * m_vSignalRight, m_oFrameRight.m_vTop);
		m_opView -> SetHighColor(EMColor(100, 0, 0, 255));
		m_opView -> StrokeLine(m_oFrameRight.m_vLeft, m_oFrameRight.m_vBottom, m_oFrameRight.m_vLeft + m_oFrameRight.GetWidth() * m_vSignalRight, m_oFrameRight.m_vBottom);
		m_opView -> SetHighColor(EMColor(255, 0, 0, 255));
		m_opView -> FillRect(oSignalArea);
		m_opView -> SetHighColor(EMColor(0, 0, 0, 255));
		m_opView -> FillRect(oEmptyArea);
		m_opView -> SetHighColor(EMColor(255, 0, 0, 255));
		m_opView -> StrokeLine(m_oFrameRight.m_vLeft + m_oFrameRight.GetWidth() * m_vSignalPeakRight, m_oFrameRight.m_vTop, m_oFrameRight.m_vLeft + m_oFrameRight.GetWidth() * m_vSignalPeakRight, m_oFrameRight.m_vBottom);
	}
	else if(m_eMode == SIGNAL_METER_DRAW_BLANK)
	{
		m_opView -> SetHighColor(EM_COLOR_VIEW_BACKGROUND);
		m_opView -> FillRect(m_oFrame);
	}
}

EMRect EMSignalMeter::Frame()
{
	return m_oFrame;
}

bool EMSignalMeter::GetVisible()
{
	return m_vVisible;
}

void EMSignalMeter::SetFrame(EMRect p_oFrame)
{
	m_oFrame = p_oFrame;
	m_oFrameLeft = EMRect(m_oFrame.m_vLeft, m_oFrame.m_vTop, m_oFrame.m_vRight, m_oFrame.m_vTop + (EM_METRICS_SIGNAL_METER_HEIGHT / 2) - 1);
	m_oFrameRight = EMRect(m_oFrame.m_vLeft, m_oFrameLeft.m_vBottom + 1, m_oFrame.m_vRight, m_oFrame.m_vBottom);
}

void EMSignalMeter::SetMode(SignalMeterMode p_eMode)
{
	m_eMode = p_eMode;
}

// Sends a signal value to all channels
void EMSignalMeter::SetSignal(float p_vSignal)
{
	float vOldSignalLeft = m_vSignalLeft;
	float vOldSignalRight = m_vSignalRight;
	float vOldSignalPeakLeft = m_vSignalPeakLeft;
	float vOldSignalPeakRight = m_vSignalPeakRight;
	float vLeftSignal(p_vSignal);
	float vRightSignal(p_vSignal);
	float vLeftDecayValue = m_vSignalLeft - SIGNAL_DECAY_RATE;
	float vRightDecayValue = m_vSignalRight - SIGNAL_DECAY_RATE;

	if(vLeftDecayValue > vLeftSignal)
		m_vSignalLeft = vLeftDecayValue;
	else
		m_vSignalLeft = vLeftSignal;
	if(vRightDecayValue > vRightSignal)
		m_vSignalRight = vRightDecayValue;
	else
		m_vSignalRight = vRightSignal;	

	if(m_vSignalPeakCounter == 0)
	{
		m_vSignalPeakLeft = m_vSignalLeft;
		m_vSignalPeakRight = m_vSignalRight;
		m_vSignalPeakCounter = SIGNAL_PEAK_TIME;
	}
	else
	{
		if(vLeftSignal > m_vSignalPeakLeft)
			m_vSignalPeakLeft = m_vSignalLeft;
		if(vRightSignal > m_vSignalPeakRight)
			m_vSignalPeakRight = m_vSignalRight;
		m_vSignalPeakCounter--;
	}

	if(m_vSignalLeft != vOldSignalLeft || m_vSignalRight != vOldSignalRight || m_vSignalPeakLeft != vOldSignalPeakLeft || m_vSignalPeakRight != vOldSignalPeakRight)
	{
		m_opView -> BeginPaint();
		Draw(m_oFrame);
		m_opView -> EndPaint();
	}
}

void EMSignalMeter::SetSignal(float* p_vpSignal)//list<float>* p_vSignal)
{
	// I've hard-coded this to stereo here, dunno if "dad" wants more or less channels
	float vOldSignalLeft = m_vSignalLeft;
	float vOldSignalRight = m_vSignalRight;
	float vOldSignalPeakLeft = m_vSignalPeakLeft;
	float vOldSignalPeakRight = m_vSignalPeakRight;
//	list<float>::iterator oIterator = p_vSignal -> begin();
//	float vLeftSignal = *oIterator;
//	oIterator++;
//	float vRightSignal = *oIterator;
	float vLeftSignal = p_vpSignal[0];
	if(p_vpSignal[1] == -1)
	{
		eo << "ERROR! Bad signal meter value in /Framework/Gui/EMSignalMeter::SetSignal()" << ef;
		p_vpSignal[1] = 0; //Hack!
	}
	float vRightSignal = p_vpSignal[1];
	float vLeftDecayValue = m_vSignalLeft - SIGNAL_DECAY_RATE;
	float vRightDecayValue = m_vSignalRight - SIGNAL_DECAY_RATE;

	if(vLeftDecayValue > vLeftSignal)
		m_vSignalLeft = vLeftDecayValue;
	else
		m_vSignalLeft = vLeftSignal;
	if(vRightDecayValue > vRightSignal)
		m_vSignalRight = vRightDecayValue;
	else
		m_vSignalRight = vRightSignal;	

	if(m_vSignalPeakCounter == 0)
	{
		m_vSignalPeakLeft = m_vSignalLeft;
		m_vSignalPeakRight = m_vSignalRight;
		m_vSignalPeakCounter = SIGNAL_PEAK_TIME;
	}
	else
	{
		if(vLeftSignal > m_vSignalPeakLeft)
			m_vSignalPeakLeft = m_vSignalLeft;
		if(vRightSignal > m_vSignalPeakRight)
			m_vSignalPeakRight = m_vSignalRight;
		m_vSignalPeakCounter--;
	}

	if(m_vSignalLeft != vOldSignalLeft || m_vSignalRight != vOldSignalRight || m_vSignalPeakLeft != vOldSignalPeakLeft || m_vSignalPeakRight != vOldSignalPeakRight)
	{
		m_opView -> BeginPaint();
		Draw(m_oFrame);
		m_opView -> EndPaint();
	}
}

void EMSignalMeter::SetVisible(bool p_vVisible)
{
	m_vVisible = p_vVisible;
}

