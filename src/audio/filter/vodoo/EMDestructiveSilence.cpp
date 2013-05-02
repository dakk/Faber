#include "EMDestructiveSilence.h"
#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"
#include "EMMediaFormat.h"

EMDestructiveSilence::EMDestructiveSilence()
	: EMDestructivePlugin("EM SILENCE", EM_DESTRUCTIVE_NATIVE)
{
}

EMDestructiveSilence::~EMDestructiveSilence()
{
}

bool EMDestructiveSilence::DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop)
{
	memset(p_opDataDest, 0, p_vLen);

	return true;
}


EMPlugin* EMDestructiveSilence::Clone()
{
	return EM_new EMDestructiveSilence;
}

void* EMDestructiveSilence::GetNativeEffect() const
{
	return NULL;
}

