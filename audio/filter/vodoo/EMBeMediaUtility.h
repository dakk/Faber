/*******************************************************
* Creator: Richard Kronfalt
* Portability: Native
*-------------------------------------------------------
* 
*******************************************************/

#include "EMGlobals.h"
#include "EMMediaUtility.h"

#ifdef PLATFORM_BEOS

#ifndef __EM_BE_MEDIA_UTILITY
#define __EM_BE_MEDIA_UTILITY

#include "EMMediaGlobals.h"
#include "EMMediaFormat.h"

#include <MediaKit.h>
#include <list>
#include <map>

class BMediaRoster;

// NOTA dava errore e l'unico modo che ho trovato
// è stato quello di eliminare l'ereditarietà da EMMediaUtility
class EMBeMediaUtility /*: public EMMediaUtility*/ {
public:
	
	static int64 BytesPerFrame(const EMMediaFormat* p_opFormat);
	static int64 BytesToFrames(int64 p_vBytes, const EMMediaFormat* p_opFormat);
	static float FramesPerSecond(const EMMediaFormat* p_opFormat);
	static int64 FramesToBytes(int64 p_vFrames, const EMMediaFormat* p_opFormat);
	static int64 FramesToTime(int64 p_vFrames, const EMMediaFormat* p_opFormat);
	static int64 FramesPerBuffer(const EMMediaFormat* p_opFormat);
	static int64 FramesInBuffer(BBuffer* p_opBuffer, const EMMediaFormat* p_opFormat, EMMediaType p_eReturnType);
	static int64 TimeToFrames(int64 p_vTime, const EMMediaFormat* p_opFormat);
	static BMediaRoster* GetRosterE();
	static void DisplayBuffer(BBuffer* opBuffer);
	static int64 FindMaxNum(signed short* array, int64 length, int64 num);

	static string ParseFullPath(string p_oFileName, uint32 p_vDirectoryID);
	static string GetFileName(string p_oFullPath);
	static string GetPathName(string p_oFullPath);
	static string GetStringAfterLast(string p_oSourceString, char p_vDelimiter);
	static string GetStringBeforeLast(string p_oSourceString, char p_vDelimiter);
	static string MakePeaksFileName(string p_oSourceFileName);
	static string MakeExtractFileName(string p_oSourceFileName);
	static string MakeConvertFileName(string p_oSourceFileName);
	static string MakeRenderFileName(uint32 p_vSequenceNumber);
	static string MakeAudioRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID);
	static string MakeVideoRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID);
	
	static EMMediaFormat* GetSystemAudioFormat();
	static EMMediaFormat* GetSystemVideoFormat();
	
	static void ClearData();
	
	//For debugging
	static list<char*>* classes;
	static list<int64>* num;
	static list<map<int64, string> >* instances;
	static void push(const void* ptr, char* name);
	static void pop(char* name);
	static void show();
protected:
	EMBeMediaUtility();
	~EMBeMediaUtility();

	static sem_id vSem;
	static EMMediaFormat* m_opSystemAudioFormat;
	static EMMediaFormat* m_opSystemVideoFormat;
};

#endif
#endif
