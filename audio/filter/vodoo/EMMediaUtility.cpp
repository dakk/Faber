#include "EMMediaUtility.h"

#include "EMGlobals.h"
#include "EMMediaFormat.h"

#include "EMBeMediaUtility.h"

EMMediaUtility* EMMediaUtility::m_opInstance = NULL;

EMMediaUtility* EMMediaUtility::Instance()
{
	// NOTA per il momento
	// ritorna questo oggetto vedi EMBeMediaUtility per le motivazioni
	// in ogni caso questa astrazione è da rimuovere
	return new EMMediaUtility();
	/*if(m_opInstance == NULL)
		m_opInstance = new EMBeMediaUtility();
	return m_opInstance;*/
}

void EMMediaUtility::Delete()
{
	if(EMMediaUtility::m_opInstance != NULL)
		delete EMMediaUtility::m_opInstance;

	EMMediaUtility::m_opInstance = NULL;
}

EMMediaUtility::EMMediaUtility()
{
}

EMMediaUtility::~EMMediaUtility()
{
}
