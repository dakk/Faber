#include "EMDestructiveNormalize.h"
#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"
#include "EMMediaFormat.h"

EMDestructiveNormalize::EMDestructiveNormalize()
	: EMDestructivePlugin("EM Normalize", EM_DESTRUCTIVE_NATIVE),
	m_vLevel(0x7fff)
{
}

EMDestructiveNormalize::~EMDestructiveNormalize()
{
}

bool EMDestructiveNormalize::DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop)
{
	int16* opSrc = reinterpret_cast<int16*>(p_opDataSource);
	int16* opDst = reinterpret_cast<int16*>(p_opDataDest);
	int16 vMax = -1;

	double vA = 0.89089871814034; // A = (1 / 2) ^ 1/6
	double vP = 1;

	for(int n = 0; n < -*p_vpParams; ++n)
		vP *= vA;

	int64 i = 0;
	for(i = 0; i < p_vLen; i += m_opSourceFormat -> m_vBytesPerSample)
	{
		if(vMax < abs(*opSrc))
			vMax = abs(*opSrc);

		++ opSrc;
	}

	float k = vP * m_vLevel / static_cast<float>(vMax);

	opSrc = reinterpret_cast<int16*>(p_opDataSource);

	for(i = 0; i < p_vLen; i += m_opSourceFormat -> m_vBytesPerSample)
	{
		*opDst = k * static_cast<float>(*opSrc);
		++ opSrc;
		++ opDst;
	}

	return true;
}



EMPlugin* EMDestructiveNormalize::Clone()
{
	return EM_new EMDestructiveNormalize;
}

void* EMDestructiveNormalize::GetNativeEffect() const
{
	return NULL;
}

int EMDestructiveNormalize::NeedExtraParams()
{
	return 1;
}
