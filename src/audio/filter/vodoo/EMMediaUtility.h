#ifndef __EM_MEDIA_UTILITY
#define __EM_MEDIA_UTILITY

#include "EMGlobals.h"
#include "EMMediaGlobals.h"

class EMMediaFormat;

class EMMediaUtility
{
public:
	static EMMediaUtility* Instance();
	static void Delete();

	int64 BytesPerFrame(const EMMediaFormat* p_opFormat);
	int64 BytesToFrames(int64 p_vBytes, const EMMediaFormat* p_opFormat);
	float FramesPerSecond(const EMMediaFormat* p_opFormat);
	int64 FramesToBytes(int64 p_vFrames, const EMMediaFormat* p_opFormat);
	int64 FramesToTime(int64 p_vFrames, const EMMediaFormat* p_opFormat);
	int64 FramesPerBuffer(const EMMediaFormat* p_opFormat);
//	int64 FramesInBuffer(void* p_opBuffer, const EMMediaFormat* p_opFormat, EMMediaType p_eReturnType);
	int64 TimeToFrames(int64 p_vTime, const EMMediaFormat* p_opFormat);
//	BMediaRoster* GetRosterE();
//	void DisplayBuffer(BBuffer* opBuffer);
	int64 FindMaxNum(signed short* array, int64 length, int64 num);

//	string ParseFullPath(const char* p_vpFileName, uint32 p_vDirectoryID);
//	string GetFileName(const char* p_vpFullPath);
//	string GetPathName(const char* p_vpFullPath);
//	string GetStringAfterLast(const char* p_vpSourceString, char p_vDelimiter);
//	string GetStringBeforeLast(const char* p_vpSourceString, char p_vDelimiter);
//	string MakePeaksFileName(const char* p_vpSourceFileName);
//	string MakeExtractFileName(const char* p_vpSourceFileName);
//	string MakeConvertFileName(const char* p_vpSourceFileName);
	string MakeRenderFileName(uint32 p_vSequenceNumber);
	string MakeAudioRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID);
	string MakeVideoRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID);
	
	EMMediaFormat* GetSystemAudioFormat();
	EMMediaFormat* GetSystemVideoFormat();

protected:
	EMMediaUtility();
	~EMMediaUtility();

	static EMMediaUtility* m_opInstance;
};

#endif
