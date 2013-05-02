#include "EMEqualizerPlugin.h"
//#include "EMSettingsRepository.h"
//#include "EMSettingIDs.h"
#include "EMMediaUtility.h"
#include "EMAudioMixFunctions.h"
/*#include "EMProjectDataLoader.h"
#include "EMProjectDataSaver.h"*/
//#include "EMCommandRepository.h"
//#include "CommandIDs.h"
#include "EMSemaphore.h"

#include <math.h>
#include <stdio.h>

EMEqualizerPlugin::EMEqualizerPlugin()
	: EMNativePlugin("EM Equalilzer", EM_TYPE_RAW_AUDIO, EM_NATIVE_AUDIO_EFFECT),
	m_oFormat(EM_TYPE_ANY_AUDIO),
	m_opH(NULL),
	m_opHfreq(NULL),
	m_vM(0),
	m_vBase(0),
	m_vMid(0),
	m_vTreb(0),
	m_opSemaphore(NULL)
{
	m_vQ = 57.0;
	m_vPi = 3.1415926535;

	//m_BufferSize = *(static_cast<int32*>(EMSettingsRepository::Instance() -> GetSetting(SETTING_AUDIO_BUFFER_SIZE))) / 2;

	for(int i = 0; i < 3; ++i)
	{
		m_opTempBuffers[i] = EM_new int16[m_BufferSize];
		memset(m_opTempBuffers[i], 0, 2*m_BufferSize);
	}

	m_oFormat = *(EMMediaUtility::Instance() -> GetSystemAudioFormat());

	m_opSemaphore = EMSemaphore::CreateEMSemaphore(NULL, 1, 1);

	InitEq(1, 1, 1);
}


EMEqualizerPlugin::~EMEqualizerPlugin()
{
	for(int i = 0; i < 3; ++i)
		if(m_opTempBuffers[i] != NULL)
			delete m_opTempBuffers[i];

	delete m_opSemaphore;
}

EMMediaDataBuffer* EMEqualizerPlugin::ProcessBufferE(list<EMMediaDataBuffer*>* p_opBuffers)
{
	EMMediaDataBuffer* opBuffer = EMAudioMixFunctions::AudioMixPostFader(p_opBuffers);

	RotateBuffers(opBuffer);

	Equalize(opBuffer);

	opBuffer -> m_opDestination = GetDestination();
	return opBuffer;
}

void EMEqualizerPlugin::InitEq(float p_vBas, float p_vMid, float p_vTre)
{
	m_opSemaphore -> Acquire();

	if(m_opHfreq != NULL)
		delete m_opHfreq;

	m_vM = 399;
	m_opHfreq = EM_new double[m_vM];

	InitFreq(m_opHfreq, p_vBas, p_vMid, p_vTre, 250.0f, 4000.0f);


	if(m_opH != NULL)
		delete m_opH;

	m_opH = CalcImpulseResponse(m_vM, m_opHfreq);



/*	FILE* fp = fopen("A.tab", "w");

	for(int i = 0; i < m_vM; ++i)
		fprintf(fp, "%f\n", m_opH[i]);

	fclose(fp);*/

	m_opSemaphore -> Release();
}


void EMEqualizerPlugin::RotateBuffers(EMMediaDataBuffer* p_opBuffer)
{
	int16* opTmp = m_opTempBuffers[0];

	m_opTempBuffers[0] = m_opTempBuffers[1];
	m_opTempBuffers[1] = m_opTempBuffers[2];
	m_opTempBuffers[2] = static_cast<short*>(p_opBuffer -> SwapBuffer(opTmp));
}


void EMEqualizerPlugin::Equalize(EMMediaDataBuffer* p_opBuffer)
{
	int i;
	int j;
	float vLeft;
	float vRight;

	int16* opTarget = static_cast<int16*>(p_opBuffer -> Data());

	int vBytesPerFrame = p_opBuffer -> m_oFormat.m_vNumChannels * p_opBuffer -> m_oFormat.m_vBytesPerSample;
	int vNumSamples = p_opBuffer -> m_vSizeUsed / vBytesPerFrame;

	if(vBytesPerFrame != 4)
	{
		EMDebugger("EMEqualizerPlugin::Equalize -> Supports only 44.1 16 right now.. ToDo: Fix..");
		return;
	}

	int16* opSource1 =&m_opTempBuffers[0][m_BufferSize - (m_vM - 1)];
	int16* opSource2 = m_opTempBuffers[1];
	int16* opSource3 = m_opTempBuffers[2];

	int16* opSource1End = &m_opTempBuffers[0][m_BufferSize];
	int16* opSource2End = &m_opTempBuffers[1][m_BufferSize];

	int16* opSrcOld  = opSource1;
	int16* opSrc;


	m_opSemaphore -> Acquire();

	float* opH = m_opH;
	int vSel = 0;
	int vSelOld = 0;

	for(i = 0; i < vNumSamples; ++i)
	{
		opH = m_opH;
		vLeft = 0;
		vRight = 0;

		opSrc = opSrcOld;
		vSel = vSelOld;

		for(j = 0; j < m_vM; ++j)
		{
			vLeft += static_cast<float>(*opSrc) * *opH;
			vRight += static_cast<float>(*(opSrc + 1)) * *opH;
			
			++opH;
			opSrc += 2;

			switch(vSel)
			{
			case 0:
				if(opSrc == opSource1End)
				{
					opSrc = opSource2;
					vSel = 1;
				}
				break;
			case 1:
				if(opSrc == opSource2End)
				{
					opSrc = opSource3;
					vSel = 2;
				}
			}
		}

		*opTarget = vLeft;
		*(opTarget + 1) = vRight;

		opTarget += 2;

		opSrcOld += 2;
		switch(vSelOld)
		{
		case 0:
			if(opSrcOld == opSource1End)
			{
				opSrcOld = opSource2;
				vSelOld = 1;
			}
			break;
		case 1:
			if(opSrcOld == opSource2End)
			{
				opSrcOld = opSource3;
				vSelOld = 2;
			}
		}
	}

	m_opSemaphore -> Release();
}


float* EMEqualizerPlugin::CalcImpulseResponse(int M, double* H)
{
	int n;
	int k;
	double dM = M;
	double tH;

	int nM = (M - 1) / 2;

	float* h = new float[M];

	for(n = 0; n < M; ++n)
	{
		tH = 0;

		for(k = 0; k < M; ++k)
		{
			float q = (k - nM)*(n - nM);

			tH += H[k] * cos(2 * m_vPi * q / dM);
		}
		tH /= dM;
		h[n] = tH;
	}

	return h;
}



double EMEqualizerPlugin::Fbas(int i, double BreakF)
{
	double dx = (double(i) / 200.0) - BreakF;

	return 1 / (1 + exp(m_vQ * dx));

}

double EMEqualizerPlugin::Ftre(int i, double BreakF)
{
	double dx = (double(i) / 200.0)- BreakF;

	return 1 / (1 + exp(-m_vQ * dx));
}

double EMEqualizerPlugin::Fmid(int i, double BreakFLo, double BreakFHigh)
{

	return 1 - Fbas(i, BreakFLo) - Ftre(i, BreakFHigh);
}


void EMEqualizerPlugin::InitFreq(double* Hb, double hb, double hm, double ht, float Flo, float Fhigh)
{
	int i;

	double F1 = Flo / 22050.0;
	double F2 = Fhigh / 22050.0;

	for(i = 0; i < 200; ++i)
	{
		Hb[200 - i - 1] = hb * Fbas(i, F1) + hm * Fmid(i, F1, F2) + ht * Ftre(i, F2);
		Hb[200 + i - 1] = hb * Fbas(i, F1) + hm * Fmid(i, F1, F2) + ht * Ftre(i, F2);
	}
}

EMPlugin* EMEqualizerPlugin::Clone()
{
	EMEqualizerPlugin* opPlug = EM_new EMEqualizerPlugin();
	
	opPlug -> SetSettings(-m_vBase, -m_vMid, -m_vTreb);

	return opPlug;
}

bool EMEqualizerPlugin::PrepareToPlayE()
{
	return true;
}

list<EMPluginPreset*>* EMEqualizerPlugin::GetPresets()
{
	return NULL;
}

bool EMEqualizerPlugin::ActivatePreset(int32 p_vID)
{
	return false;
}
	
/*
bool EMEqualizerPlugin::LoadSetting(EMProjectDataLoader* p_opLoader)
{
	//EMBufferDestination::LoadData(p_opLoader);

	m_vBase = p_opLoader -> LoadInt8();
	m_vMid = p_opLoader -> LoadInt8();
	m_vTreb = p_opLoader -> LoadInt8();

	InitEq(dBtoLin(m_vBase), dBtoLin(m_vMid), dBtoLin(m_vTreb));

	return true;
}
	

bool EMEqualizerPlugin::SaveSetting(EMProjectDataSaver* p_opSaver)
{
	EMBufferDestination::SaveData(p_opSaver);

	p_opSaver -> SaveInt8(m_vBase);
	p_opSaver -> SaveInt8(m_vMid);
	p_opSaver -> SaveInt8(m_vTreb);

	return true;
}*/

void* EMEqualizerPlugin::GetNativeEffect() const
{
	return (void*)this;
}


bool EMEqualizerPlugin::ShowDialog()
{
	//EMCommandRepository::Instance() -> ExecuteCommand(COMMAND_DISPLAY_EQ_DIALOG, this);
	return true;
}


void EMEqualizerPlugin::GetSettings(int8& p_vBase, int8& p_vMid, int8& p_vTreb)
{
	p_vBase = -m_vBase;
	p_vMid = -m_vMid;
	p_vTreb = -m_vTreb;
}

double EMEqualizerPlugin::dBtoLin(int p_vdB)
{
	double vA = p_vdB < 0 ? 0.89089871814034 : 1.12246204830937; // A = (1 / 2) ^ 1/6 || 2 ^ 1/6
	double vP = 1;

	for(int n = 0; n < abs(p_vdB); ++n)
		vP *= vA;

	return vP;
}

void EMEqualizerPlugin::SetSettings(int8 p_vBase, int8 p_vMid, int8 p_vTreb)
{
	m_vBase = -p_vBase;
	m_vMid = -p_vMid;
	m_vTreb = -p_vTreb;

	eo << "EMEqualizerPlugin::SetSettings(" << m_vBase << ", " << m_vMid << ", " << m_vTreb << ");" << ef;

	InitEq(dBtoLin(m_vBase), dBtoLin(m_vMid), dBtoLin(m_vTreb));
}
