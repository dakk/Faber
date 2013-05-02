#include "EMDestructiveReverse.h"
#include "EMWaveFileReader.h"
#include "EMWaveFileWriter.h"
#include "EMMediaFormat.h"

EMDestructiveReverse::EMDestructiveReverse()
	: EMDestructivePlugin("EM Reverse", EM_DESTRUCTIVE_NATIVE)
{
}

EMDestructiveReverse::~EMDestructiveReverse()
{
}

bool EMDestructiveReverse::DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop)
{
	int16* opSrc = reinterpret_cast<int16*>(p_opDataSource);
	int16* opDst = reinterpret_cast<int16*>(p_opDataDest + p_vLen - m_vBytesPerFrame);

	while(opDst >= reinterpret_cast<int16*>(p_opDataDest)) //for(int64 i = 0; i < p_vLen; i += m_vBytesPerFrame)
	{
//		for(int j = 0; j < m_opSourceFormat -> m_vNumChannels; ++j)
//		{
			*opDst = *opSrc;
			
			++ opSrc;
			-- opDst;
//		}
	}

	return true;
}

EMPlugin* EMDestructiveReverse::Clone()
{
	return EM_new EMDestructiveReverse;
}

void* EMDestructiveReverse::GetNativeEffect() const
{
	return NULL;
}

