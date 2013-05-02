//NOTE this is windows code 
// just hiding it for now
#ifdef WINDOWS

#include "EMDestructivePlugin.h"

#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"

#include "EMWinMemoryMappedWaveReader.h"
#include "EMWinMemoryMappedWaveWriter.h"

#include <windows.h>


EMDestructivePlugin::EMDestructivePlugin(string p_oPluginName, EMPluginType p_vType)
  : EMNativePlugin(p_oPluginName, EM_TYPE_RAW_AUDIO, p_vType, EM_EFFECT),
	m_opReader(NULL),
	m_opWriter(NULL),
	m_opSourceFormat(NULL)
{
}

EMDestructivePlugin::~EMDestructivePlugin()
{
	if(m_opReader != NULL)
		delete m_opReader;

	if(m_opWriter != NULL)
		delete m_opWriter;
}

	
EMMediaDataBuffer* EMDestructivePlugin::ProcessBufferE(list<EMMediaDataBuffer*>* p_opBuffers)
{
	return NULL;
}


list<EMPluginPreset*>* EMDestructivePlugin::GetPresets()
{
	return NULL;
}

bool EMDestructivePlugin::ActivatePreset(int32 p_vID)
{
	return false;
}

bool EMDestructivePlugin::LoadSetting(EMProjectDataLoader* p_opLoader)
{
	return false;
}
	
bool EMDestructivePlugin::SaveSetting(EMProjectDataSaver* p_opSaver)
{
	return false;
}

const char* EMDestructivePlugin::Process(const char* p_opFile, int64 p_vStart, int64 p_vStop, int64* p_vpParams, int p_vFadeIn, int p_vFadeOut)
{
	m_opReader = EM_new EMWinMemoryMappedWaveReader(p_opFile);
	m_opReader -> InitCheckE();

	int64 vFileLen = m_opReader -> NumberOfFramesInFile();

	int64 vStart = p_vStart < p_vFadeIn ? p_vFadeIn : p_vStart;
	int64 vStop = p_vStop >= vFileLen - p_vFadeOut ? vFileLen - p_vFadeOut : p_vStop;

	m_oNewName = Init(string(p_opFile), p_vStop - p_vStart);

	m_opWriter = EM_new EMWinMemoryMappedWaveWriter(m_oNewName, m_opSourceFormat, vStop - vStart);
	m_opWriter -> InitCheckE();

	int64 vLenMid = (vStop - vStart) * m_vBytesPerFrame;

	DoPlugin(m_opReader -> GetDataPointer(vStart, vStop), m_opWriter -> GetDataPointer(0, 0), vLenMid, p_vpParams, p_vStart - p_vFadeIn, p_vStop - p_vFadeIn);

	delete m_opReader;
	delete m_opWriter;

	m_opReader = NULL;
	m_opWriter = NULL;

	return m_oNewName.c_str();
}



string EMDestructivePlugin::GenerateNewName(string p_oName, int* p_vpCnt)
{
	char vNewName[256];
	char vOldName[256];

	strcpy(vOldName, p_oName.c_str());

	char* vDot = strrchr(vOldName, '.');
	char* vTmp;

	int vCnt = *p_vpCnt;

	if(vDot[-1] == ']')
	{
		vTmp = strrchr(vOldName,'[');

		if(vTmp != NULL)
		{
			vDot = vTmp;
/*			vCnt = atoi(vDot + 1);

			if(vCnt == 0)
				vCnt = *p_vpCnt;
			else
				vCnt += 1;*/
		}
	}

	*vDot = 0;

	sprintf(vNewName,"%s[%d].wav", vOldName, vCnt);

	*p_vpCnt = vCnt;

//	SYSTEMTIME oTime;
//	GetSystemTime(&oTime);

//	sprintf(vTmp, "%s-%s.%d.%d.%d.%d.%d.%d.%d.wav", p_oName.c_str(), GetName().c_str(), oTime.wYear, oTime.wMonth, oTime.wDay, oTime.wHour, oTime.wMinute, oTime.wSecond, oTime.wMilliseconds);

	return string(vNewName);
}

string EMDestructivePlugin::Init(string &p_oName, int64 p_vNumFrames)
{
	int vCnt = 1;
	bool vRet;
	string oNewName;

	m_opSourceFormat = m_opReader -> GetFormat();
	m_vBytesPerFrame =  m_opSourceFormat -> m_vNumChannels * m_opSourceFormat -> m_vBytesPerSample;

	vRet = false;
	while(!vRet)
	{
		oNewName = GenerateNewName(p_oName, &vCnt);

		HANDLE hDummy = CreateFile(oNewName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hDummy != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hDummy);

			vRet = true;
		}
		else
		{
			vCnt ++;
		}
	}

	return oNewName;
}

bool EMDestructivePlugin::PrepareToPlayE()
{
	return false;
}


bool EMDestructivePlugin::Done()
{
	if(m_opReader != NULL)
	{
		delete m_opReader;
		m_opReader = NULL;
	}

	if(m_opWriter != NULL)
	{
		delete m_opWriter;
		m_opWriter = NULL;
	}

	return true;
}


int EMDestructivePlugin::NeedExtraParams()
{
	return 0;
}
#endif
