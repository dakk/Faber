/*******************************************************
* Creator: Martin Johansson
* Portability: Non-Native
*-------------------------------------------------------
*
* This class contains the destructive plugin 
*
*******************************************************/

#ifndef __EM_DESTRUCTIVE_REVERSE
#define __EM_DESTRUCTIVE_REVERSE

#include "EMGlobals.h"
#include "EMMediaGlobals.h"
#include "EMDestructivePlugin.h"

#include <list>


class EMDestructiveReverse : public EMDestructivePlugin
{
public:
	EMDestructiveReverse();
	~EMDestructiveReverse();

//	const char* Process(const char* p_opFile, int64 p_vStart, int64 p_vStop);

	EMPlugin* Clone();
	void* GetNativeEffect() const;

protected:
	bool DoPlugin(char* p_opDataSource, char* p_opDataDest, int64 p_vLen, int64* p_vpParams, int64 p_vStart, int64 p_vStop);

private:
};

#endif