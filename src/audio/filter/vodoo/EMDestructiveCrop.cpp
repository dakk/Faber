#include "EMDestructiveCrop.h"
#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"
#include "EMMediaFormat.h"

EMDestructiveCrop::EMDestructiveCrop()
	: EMDestructivePlugin("EM CROP", EM_DESTRUCTIVE_NATIVE)
{
}

EMDestructiveCrop::~EMDestructiveCrop()
{
}

bool EMDestructiveCrop::DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop)
{
	for(int64 i = 0; i < p_vLen; ++i)
		p_opDataDest[i] = p_opDataSource[i];

	return true;
}

EMPlugin* EMDestructiveCrop::Clone()
{
	return EM_new EMDestructiveCrop;
}

void* EMDestructiveCrop::GetNativeEffect() const
{
	return NULL;
}

