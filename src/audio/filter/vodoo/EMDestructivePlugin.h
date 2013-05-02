/*******************************************************
* Creator: Martin Johansson
* Portability: Non-Native
*-------------------------------------------------------
*
* This class contains the destructive plugin 
*
*******************************************************/

#ifndef __EM_DESTRUCTIVE_PLUGIN
#define __EM_DESTRUCTIVE_PLUGIN

#include "EMGlobals.h"
#include "EMMediaGlobals.h"
#include "EMNativePlugin.h"

#include <list>

class EMWinMemoryMappedWaveReader;
class EMWinMemoryMappedWaveWriter;

class EMPluginEntry;

class EMDestructivePlugin : public EMNativePlugin
{
public:
	virtual ~EMDestructivePlugin();

	const char* Process(const char* p_opFile, int64 p_vStart, int64 p_vStop, int64* p_vpParams, int p_vFadeIn, int p_vFadeOut);

	EMMediaDataBuffer* ProcessBufferE(list<EMMediaDataBuffer*>* p_opBuffers);

	bool PrepareToPlayE();
	bool Done();

	list<EMPluginPreset*>* GetPresets();
	bool ActivatePreset(int32 p_vID);
	/*bool LoadSetting(EMProjectDataLoader* p_opLoader);
	bool SaveSetting(EMProjectDataSaver* p_opSaver);*/

	virtual int NeedExtraParams();

protected:
	EMDestructivePlugin(string p_oPluginName, EMPluginType p_vType);

	string GenerateNewName(string p_oName, int* p_vpCnt);
	string Init(string &p_oName, int64 p_vNumFrames);

	virtual bool DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop) = 0;
	
protected:
	EMWinMemoryMappedWaveReader* m_opReader;
	EMWinMemoryMappedWaveWriter* m_opWriter;

	EMMediaFormat* m_opSourceFormat;
	int	m_vBytesPerFrame;

	string m_oNewName;

private:
};

#endif
