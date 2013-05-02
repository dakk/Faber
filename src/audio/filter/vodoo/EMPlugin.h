/*******************************************************
* Creator: Richard Kronfalt
* Portability: Non-Native
*-------------------------------------------------------
*
* This class contains the plugin (for windows this would
* mean either a DirectX-plugin or a VST-plugin). It is
* non-native, meaning a sub-class specifies the native
* things, such as pointers to system dependent plugin
* and/or plugin wrappers.
*
*******************************************************/

#ifndef __EM_PLUGIN
#define __EM_PLUGIN

#include "EMMediaIDManager.h"

class EMPluginPreset
{
public:
	EMPluginPreset() { m_vID = EMMediaIDManager::MakeID(); };
	~EMPluginPreset() { delete m_vpData; };
	string m_oName;
	char* m_vpData;
	uint64 m_vDataSize;
	int32 m_vID;
	int32 m_vType;

private:

};

#include "EMGlobals.h"
#include "EMMediaGlobals.h"
#include "EMBufferDestination.h"
#include "EMMediaDataBuffer.h"

#include <list>

class EMPluginEntry;

class EMPlugin : public EMBufferDestination
{
public:
	virtual ~EMPlugin();
	string GetName() const;

	EMPluginType GetPluginType() const;
	int32 GetEntryID() const;
	void SetEntryID(int32 p_vEntryID);
	
	virtual EMMediaDataBuffer* ProcessBufferE(list<EMMediaDataBuffer*>* p_opBuffers) = 0;
	virtual bool PrepareToPlayE() = 0;

	void SetDryWetMix(float p_vDryWetMix);
	float GetDryWetMix() const;

	virtual list<EMPluginPreset*>* GetPresets() = 0;
	virtual bool ActivatePreset(int32 p_vID) = 0;
	/*virtual bool LoadSetting(EMProjectDataLoader* p_opLoader) = 0;
	virtual bool SaveSetting(EMProjectDataSaver* p_opSaver) = 0;*/

	virtual EMPlugin* Clone() = 0;

	void SetID(int32 p_vID);
	virtual void* GetNativeEffect() const = 0;
	virtual bool ShowDialog() {return false;};

protected:
	EMPlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType);
	EMPlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType, EMDestinationPosition p_ePosition);
	float m_vDryWetMix; //0.0 = no sound passes through plugin. 1.0 all sound passes through.
	list<EMPluginPreset*>* m_opPresets;
	bool m_vIsInitialized;

private:
	EMPluginEntry* m_opCorrespondingEntry;
	string m_oName;
	EMPluginType m_ePluginType;
	int32 m_vPluginEntryID;

};

#endif
