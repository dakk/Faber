/*******************************************************
* Creator: Martin Johansson
* Portability: Native
*-------------------------------------------------------
*
*
*******************************************************/

#ifndef __EM_NATIVE_PLUGIN
#define __EM_NATIVE_PLUGIN

#include "EMPlugin.h"


#include <list>

class EMNativePlugin : public EMPlugin
{
public:
	virtual ~EMNativePlugin();

	virtual void* GetDefaultProperties() {return NULL;};
protected:
	EMNativePlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType);
	EMNativePlugin(string p_oPluginName, EMMediaType p_vMediaType, EMPluginType p_ePluginType, EMDestinationPosition p_ePosition);

private:
	EMNativePlugin();
};

#endif