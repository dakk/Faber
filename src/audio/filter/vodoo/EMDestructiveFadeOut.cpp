#include "EMDestructiveFadeOut.h"
#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"
#include "EMMediaFormat.h"

EMDestructiveFadeOut::EMDestructiveFadeOut()
	: EMDestructivePlugin("EM FadeOut", EM_DESTRUCTIVE_NATIVE)
{
}

EMDestructiveFadeOut::~EMDestructiveFadeOut()
{
}

bool EMDestructiveFadeOut::DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop)
{
	int16* opSrc = reinterpret_cast<int16*>(p_opDataSource);
	int16* opDst = reinterpret_cast<int16*>(p_opDataDest);

	float k = 1.0f / static_cast<float>(p_vStop - p_vStart);
	float x = 1.0f - ((p_vStart < 0) ? -k * p_vStart : 0);
	
	for(int64 i = 0; i < p_vLen; i += m_vBytesPerFrame)
	{
		for(int j = 0; j < m_opSourceFormat -> m_vNumChannels; ++j)
		{
			*opDst = x * static_cast<float>(*opSrc);
			++ opSrc;
			++ opDst;
		}
		x -= k;
	}

	return true;
}


EMPlugin* EMDestructiveFadeOut::Clone()
{
	return EM_new EMDestructiveFadeOut;
}

void* EMDestructiveFadeOut::GetNativeEffect() const
{
	return NULL;
}

