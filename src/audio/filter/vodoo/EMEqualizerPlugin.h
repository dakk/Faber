/*******************************************************
* Creator: Martin Johansson
* Portability: Non-Native
*-------------------------------------------------------
*
* This class contains the destructive plugin 
*
*******************************************************/

#ifndef __EM_EQUALIZER_PLUGIN
#define __EM_EQUALIZER_PLUGIN

#include "EMGlobals.h"
#include "EMMediaGlobals.h"
#include "EMNativePlugin.h"
#include "EMMediaFormat.h"
#include "EMEqualizerInterface.h"

#include <list>


class EMEqualizerPlugin : public EMEqualizerInterface, public EMNativePlugin
{
public:
	EMEqualizerPlugin();
	virtual ~EMEqualizerPlugin();

	EMMediaDataBuffer* ProcessBufferE(list<EMMediaDataBuffer*>* p_opBuffers);

	bool PrepareToPlayE();

	EMPlugin* Clone();

	list<EMPluginPreset*>* GetPresets();
	bool ActivatePreset(int32 p_vID);
	/*bool LoadSetting(EMProjectDataLoader* p_opLoader);
	bool SaveSetting(EMProjectDataSaver* p_opSaver);*/

	void* GetNativeEffect() const;

	void InitEq(float p_vBas, float p_vMid, float p_vTre);

	void GetSettings(int8& p_vBase, int8& p_vMid, int8& p_vTreb);
	void SetSettings(int8 p_vBase, int8 p_vMid, int8 p_vTreb);

	bool ShowDialog();
protected:

private:
	void RotateBuffers(EMMediaDataBuffer* p_opBuffer);
	void Equalize(EMMediaDataBuffer* p_opBuffer);

	float* CalcImpulseResponse(int M, double* H);

	double Fbas(int i, double BreakF);
	double Ftre(int i, double BreakF);
	double Fmid(int i, double BreakFLo, double BreakFHigh);

	void InitFreq(double* Hb, double hb, double hm, double ht, float Flo, float Fhigh);

	double dBtoLin(int p_vdB);

private:
	EMSemaphore* m_opSemaphore;

	int32 m_BufferSize;
	int16* m_opTempBuffers[3];
	EMMediaFormat m_oFormat;

	double m_vQ;
	double m_vPi;

	double* m_opHfreq;
	float* m_opH;
	int32 m_vM;

	int8 m_vBase;
	int8 m_vMid;
	int8 m_vTreb;
};

#endif
