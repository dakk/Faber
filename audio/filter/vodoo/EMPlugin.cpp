#include "EMPlugin.h"

#include "EMPluginEntry.h"

EMPlugin::EMPlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType)
	:	EMBufferDestination(p_vMediaType, EM_EFFECT,"EMPlugin"),
		m_opCorrespondingEntry(NULL),
		m_oName(p_oPluginName),
		m_ePluginType(p_ePluginType),
		m_vDryWetMix(0.5),
		m_opPresets(NULL),
		m_vIsInitialized(false)
{
	m_opPresets = EM_new list<EMPluginPreset*>();
}

EMPlugin::EMPlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType, EMDestinationPosition p_ePosition)
	:	EMBufferDestination(p_vMediaType, p_ePosition,"EMPlugin"),
		m_opCorrespondingEntry(NULL),
		m_oName(p_oPluginName),
		m_ePluginType(p_ePluginType),
		m_vDryWetMix(0.5),
		m_opPresets(NULL),
		m_vIsInitialized(false)
{
	m_opPresets = EM_new list<EMPluginPreset*>();
}

EMPlugin::~EMPlugin()
{
	delete m_opPresets;
}

string EMPlugin::GetName() const
{
	return m_oName;
}

EMPluginType EMPlugin::GetPluginType() const
{
	return m_ePluginType;
}

int32 EMPlugin::GetEntryID() const
{
	return m_vPluginEntryID;
}

void EMPlugin::SetEntryID(int32 p_vEntryID)
{
	m_vPluginEntryID = p_vEntryID;
}

void EMPlugin::SetDryWetMix(float p_vDryWetMix)
{
	if(p_vDryWetMix > 1.0)
		p_vDryWetMix = 1.0;
	if(p_vDryWetMix < 0.0)
		p_vDryWetMix = 0.0;
	m_vDryWetMix = p_vDryWetMix;
}

float EMPlugin::GetDryWetMix() const
{
	return m_vDryWetMix;
}

void EMPlugin::SetID(int32 p_vID)
{
}