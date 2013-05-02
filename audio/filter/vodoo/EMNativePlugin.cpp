#include "EMNativePlugin.h"


EMNativePlugin::EMNativePlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType)
: EMPlugin(p_oPluginName, p_vMediaType, p_ePluginType)
{
}

EMNativePlugin::EMNativePlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType, EMDestinationPosition p_ePosition)
: EMPlugin(p_oPluginName, p_vMediaType, p_ePluginType, p_ePosition)
{
}

EMNativePlugin::~EMNativePlugin()
{
}

