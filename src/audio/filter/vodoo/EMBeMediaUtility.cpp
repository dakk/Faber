#include "EMGlobals.h"

#ifdef PLATFORM_BEOS

#include "EMBeMediaUtility.h"
#include "EMMediaFormat.h"
//#include "EMCommandRepository.h"
//#include "EMSettingsRepository.h"
//#include "CommandIDs.h"
//#include "DirectoryIDs.h"
//#include "EMMediaEngine.h"
//#include "EMSettingIDs.h"

#include <iostream>
#include <MediaKit.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <string.h>
#include <map>

sem_id EMBeMediaUtility::vSem = create_sem(1, "debug sem");
list<char*>* EMBeMediaUtility::classes = new list<char*>();
list<int64>* EMBeMediaUtility::num = new list<int64>();
list<map<int64, string> >* EMBeMediaUtility::instances = new list<map<int64, string> >();

EMMediaFormat* EMBeMediaUtility::m_opSystemAudioFormat = NULL;
EMMediaFormat* EMBeMediaUtility::m_opSystemVideoFormat = NULL;

void EMBeMediaUtility::push(const void* ptr, char* name)
{
	status_t vAcquireResult = acquire_sem(vSem);

	map<int64, string> oAddressNameMap;
	oAddressNameMap[(int64) ptr] = string(name);
	instances -> push_back(oAddressNameMap);
//	push(name);
//}
//
//void EMBeMediaUtility::push(char* name)
//{

	if(vAcquireResult != B_NO_ERROR)
		EMDebugger("ERROR! EMBeMediaUtility could not acquire semaphore for push-protection!");

	list<char*>::const_iterator names;
	list<int64>::iterator nums;
	names = classes -> begin();
	nums = num -> begin();
	bool found = false;
	for(; names != classes -> end() && nums != num -> end(); names++, nums++)
	{
		if(strcmp((*names), name) == 0)
		{
			(*nums)++;
			found = true;
			break;
		}
	}

	if(! found)
	{
		num -> push_back(1);
		classes -> push_back(name);
	}
	release_sem(vSem);
}

void EMBeMediaUtility::pop(char* name)
{
	status_t vAcquireResult = acquire_sem(vSem);
	if(vAcquireResult != B_NO_ERROR)
		EMDebugger("ERROR! EMBeMediaUtility could not acquire semaphore for push-protection!");
//	classes -> remove(name);

	list<char*>::const_iterator names;
	list<int64>::iterator nums;
	names = classes -> begin();
	nums = num -> begin();
	bool found = false;
	for(; names != classes -> end() && nums != num -> end(); names++, nums++)
	{
		if(strcmp((*names), name) == 0)
		{
			(*nums)--;
			found = true;
			break;
		}
	}

//	if(! found)
//		nums -> push_back(1);

	release_sem(vSem);
}

void EMBeMediaUtility::show()
{
	status_t vAcquireResult = acquire_sem(vSem);
	if(vAcquireResult != B_NO_ERROR)
		EMDebugger("ERROR! EMBeMediaUtility could not acquire semaphore for push-protection!");

	;//cout_commented_out_4_release << endl << endl;
	;//cout_commented_out_4_release << "** Classes currently instantiated:" << endl;
	list<char*>::const_iterator i;
	list<int64>::const_iterator j = num -> begin();
	int64 sum = 0;
	for(i = classes -> begin(); i != classes -> end() && j != num -> end(); i++, j++)
	{
		;//cout_commented_out_4_release << "** " << (*j) << " instance(s) of class " << (*i) << endl;
		sum += (*j);
	}
	;//cout_commented_out_4_release << "** Total number of instances left is " << sum << endl;
	;//cout_commented_out_4_release << "** End of list" << endl;
	
	
/*	;//cout_commented_out_4_release << endl << endl;
	;//cout_commented_out_4_release << "** Classes and their object addresses:" << endl;

	list<map<int64, string> >::iterator it;
	for(it = instances -> begin(); it != instances -> end(); it++)
	{
		map<int64, string>::iterator mapIter;
		mapIter = (*it).begin();
		;//cout_commented_out_4_release << "** " << (*mapIter).first << " named \"" << (*mapIter).second << endl;
	}
	
	;//cout_commented_out_4_release << "** End of list" << endl; */
	release_sem(vSem);
}

EMBeMediaUtility::EMBeMediaUtility()
{
}

EMBeMediaUtility::~EMBeMediaUtility()
{
}

BMediaRoster* EMBeMediaUtility::GetRosterE()
{
	status_t vErrorCode;
	BMediaRoster* opRoster = BMediaRoster::Roster(&vErrorCode);
	if(opRoster == NULL || vErrorCode)
	{
		 opRoster = BMediaRoster::CurrentRoster();
		 if(opRoster == NULL)
		 {
		 	//Throw exception instead!
		 	//emerr << "ERROR! Could not get roster!" << endl;
		 	return NULL;
		 }
	}
	return opRoster;
}

void EMBeMediaUtility::DisplayBuffer(BBuffer* opBuffer)
{
	;//emout_commented_out_4_release << "== BBuffer contents ==" << endl;
	;//emout_commented_out_4_release << "   Size available: " << opBuffer -> SizeAvailable() << endl;
	;//emout_commented_out_4_release << "   Size used: " << opBuffer -> SizeUsed() << endl;
	;//emout_commented_out_4_release << "   ID: " << opBuffer -> ID() << endl;
	;//emout_commented_out_4_release << "   Flags: " << opBuffer -> Flags() << endl;
	;//emout_commented_out_4_release << "   Start time: " << opBuffer -> Header() -> start_time << endl;
	;//emout_commented_out_4_release << "   Type: " << static_cast<int>(opBuffer -> Header() -> type) << endl;
	;//emout_commented_out_4_release << "   First sample: " << static_cast<signed short*>(opBuffer -> Data())[0] << endl;
	;//emout_commented_out_4_release << "" << endl;
}

int64 EMBeMediaUtility::FramesToTime(int64 p_vFrames, const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return static_cast<int64>((1000000.0 * static_cast<float>(p_vFrames)) / static_cast<float>(p_opFormat -> m_vFrameRate));
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
		return static_cast<int64>(static_cast<float>(p_vFrames) * 1000000.0 / static_cast<float>(p_opFormat -> m_vFrameRate));
	else
		EMDebugger("ERROR! FramesToTime is only implemented for audio and vide, this far!");
	return 0;
}

int64 EMBeMediaUtility::TimeToFrames(int64 p_vTime, const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return static_cast<int64>(floor((p_vTime * static_cast<float>(static_cast<float>(p_opFormat -> m_vFrameRate)) / 1000000.0)));
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
		return static_cast<int64>(floor((static_cast<float>(p_vTime) * static_cast<float>(p_opFormat -> m_vFrameRate) / 1000000.0)));
	else
		EMDebugger("ERROR! TimeToFrames is only implemented for audio and video, this far!");
	return 0;
}

int64 EMBeMediaUtility::BytesToFrames(int64 p_vBytes, const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return static_cast<int64>(static_cast<float>(p_vBytes) / (static_cast<float>(p_opFormat -> m_vNumChannels) * p_opFormat -> m_vBytesPerSample));
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
		return static_cast<int64>(floor(static_cast<float>(p_vBytes) / (static_cast<float>(p_opFormat -> m_vHeight) * static_cast<float>(p_opFormat -> m_vBytesPerRow))));
	else
		EMDebugger("ERROR! BytesToFrames is only implemented for audio and video, this far!");
	return 0;
}

int64 EMBeMediaUtility::FramesToBytes(int64 p_vFrames, const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return p_vFrames * (p_opFormat -> m_vNumChannels * p_opFormat -> m_vBytesPerSample);
	else
		EMDebugger("ERROR! FramesToBytes is only implemented for audio, this far!");
	return 0;
}

int64 EMBeMediaUtility::FramesPerBuffer(const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return p_opFormat -> m_vBufferSizeBytes / (p_opFormat -> m_vBytesPerSample * p_opFormat -> m_vNumChannels);
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
		return 1;
	else
		EMDebugger("ERROR! FramesPerBuffer is only implemented for audio and video, this far!");
	return 0;
}

int64 EMBeMediaUtility::FramesInBuffer(BBuffer* p_opBuffer, const EMMediaFormat* p_opFormat, EMMediaType p_eReturnType)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
	{
		int64 vFrames = 0;
		if((p_eReturnType & p_opFormat -> m_eType) > 0)
			vFrames = static_cast<int64>((p_opBuffer -> SizeUsed()) / ((p_opFormat -> m_vBytesPerSample) * p_opFormat -> m_vNumChannels));
		else
		{
			if(m_opSystemAudioFormat == NULL) m_opSystemAudioFormat = new EMMediaFormat(EM_TYPE_RAW_AUDIO);
			if(m_opSystemVideoFormat == NULL) m_opSystemVideoFormat = new EMMediaFormat(EM_TYPE_RAW_VIDEO);
			
			vFrames = TimeToFrames(FramesToTime((static_cast<int64>((p_opBuffer -> SizeUsed()) / ((p_opFormat -> m_vBytesPerSample) * p_opFormat -> m_vNumChannels))), m_opSystemAudioFormat), m_opSystemVideoFormat);
		}
		return vFrames;
	}
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
	{
		int64 vFrames = 0;
		if((p_eReturnType & p_opFormat -> m_eType) > 0)
			vFrames = 1;
		else
			vFrames = TimeToFrames(FramesToTime(1, p_opFormat), m_opSystemAudioFormat);
		return vFrames;
	}
	else
		EMDebugger("ERROR! FramesInBuffer is only implemented for audio and video, this far!");
	return 0;
}

int64 EMBeMediaUtility::BytesPerFrame(const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return static_cast<int64>(p_opFormat -> m_vNumChannels * p_opFormat -> m_vBytesPerSample);
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
		return p_opFormat -> m_vHeight * p_opFormat -> m_vBytesPerRow;
	else
		EMDebugger("ERROR! BytesPerFrame is only implemented for audio and video, this far!");
	return 0;
}

float EMBeMediaUtility::FramesPerSecond(const EMMediaFormat* p_opFormat)
{
	if((p_opFormat -> m_eType & EM_TYPE_RAW_AUDIO) > 0)
		return p_opFormat -> m_vFrameRate;
	else if((p_opFormat -> m_eType & EM_TYPE_RAW_VIDEO) > 0)
	{
		//Should be calculated through the preferences and the acctual file videofile field_rate
		return p_opFormat -> m_vFrameRate; //Static
	}
	else
		EMDebugger("ERROR! FramesPerSecond is only implemented for audio and video, this far!");
	return 0;
}

int64 EMBeMediaUtility::FindMaxNum(signed short* array, int64 length, int64 num)
{
	signed short* max = new signed short[num];
	for(register int i = 0; i < num; i++)
		max[i] = 0;
		
	for(register int i = 0; i < length; i++)
	{
		for(register int j = num - 1; j >= 0; j--)
		{
			if(array[i] > max[j])
			{
				max[j] = array[i];
				break;
			}
		}
	}
	
	signed short val = max[0];
	delete [] max;
	return static_cast<int64>(val);
}

string EMBeMediaUtility::GetStringBeforeLast(string p_oSourceString, char p_vDelimiter)
{
	int32 vSeekedPos = -1;
	int32 vCharPosition = 0;
	for(string::iterator oStringIterator = p_oSourceString.begin(); oStringIterator != p_oSourceString.end(); oStringIterator++, vCharPosition++)
		if((*oStringIterator) == p_vDelimiter) vSeekedPos = vCharPosition;
	if(vSeekedPos == -1) return string("");
	return p_oSourceString.substr(0, vSeekedPos);
}

string EMBeMediaUtility::GetStringAfterLast(string p_oSourceString, char p_vDelimiter)
{
	int32 vSeekedPos = -1;
	int32 vCharPosition = 0;
	for(string::iterator oStringIterator = p_oSourceString.begin(); oStringIterator != p_oSourceString.end(); oStringIterator++, vCharPosition++)
		if((*oStringIterator) == p_vDelimiter) vSeekedPos = vCharPosition;
	if(vSeekedPos == -1) return string("");
	return p_oSourceString.substr(vSeekedPos + 1, p_oSourceString.size() - (vSeekedPos + 1));
}

string EMBeMediaUtility::GetFileName(string p_oFullPath)
{	
	return GetStringAfterLast(p_oFullPath, '/');
}

string EMBeMediaUtility::GetPathName(string p_oFullPath)
{
	return GetStringBeforeLast(p_oFullPath, '/');
}

string EMBeMediaUtility::ParseFullPath(string p_oFileName, uint32 p_vDirectoryID)
{
	/*int vProjectID = *(static_cast<int*>(EMMediaEngine::Instance() -> GetSettingsRepository() -> GetSetting(SETTING_PROJECT_ID)));
	string oPath = *static_cast<string*>(EMMediaEngine::Instance() -> GetCommandRepository() -> ExecuteCommand(PROJECT_COMMAND_GET_DIRECTORY, &vProjectID, &p_vDirectoryID));
	oPath += string("/") + p_oFileName;
	return oPath;*/
	return "";
}

string EMBeMediaUtility::MakePeaksFileName(string p_oSourceFileName)
{
	string oSourceFile = GetFileName(p_oSourceFileName);
	string oSourceFileNoExt = GetStringBeforeLast(oSourceFile, '.');
	string oSourceFileExt = GetStringAfterLast(oSourceFile, '.');
	string oResultFile = oSourceFileNoExt + string("_WaveFormPeaks.") + oSourceFileExt;
	string oResultFullPath/* = ParseFullPath(oResultFile, DIRECTORY_ID_AUDIO_DATA_PEAKS)*/;
	return oResultFullPath;
}

string EMBeMediaUtility::MakeExtractFileName(string p_oSourceFileName)
{
	/*string oSourceFile = GetFileName(p_oSourceFileName);
	string oSourceFileNoExt = GetStringBeforeLast(oSourceFile, '.');
	string oResultFile = oSourceFileNoExt + string("_ExtractedAudio.wav");
	string oResultFullPath = ParseFullPath(oResultFile, DIRECTORY_ID_AUDIO_DATA_USED);
	return oResultFullPath;*/
	return "";
}

string EMBeMediaUtility::MakeConvertFileName(string p_oSourceFileName)
{
	/*string oSourceFile = GetFileName(p_oSourceFileName);
	string oSourceFileNoExt = GetStringBeforeLast(oSourceFile, '.');
	string oSourceFileExt = GetStringAfterLast(oSourceFile, '.');
	string oResultFile = oSourceFileNoExt + string("_ConvertedAudio.") + oSourceFileExt;
	string oResultFullPath = ParseFullPath(oResultFile, DIRECTORY_ID_AUDIO_DATA_USED);
	return oResultFullPath;*/
	return "";
}

string EMBeMediaUtility::MakeRenderFileName(uint32 p_vSequenceNumber)
{
	/*string oExtension;
	string oFamilyPrettyName = *static_cast<string*>(EMMediaEngine::Instance() -> GetSettingsRepository() -> GetSetting(SETTING_RENDER_CODEC_FAMILY));

	int32 vCookie = 0;
	media_file_format sFileFormatInformation;
	while(get_next_file_format(&vCookie, &sFileFormatInformation) == B_OK)
		if(string(sFileFormatInformation.pretty_name) == oFamilyPrettyName)
		{
			oExtension = string(".") + sFileFormatInformation.file_extension;
			break;
		}

	string oZeroes = "";
	if(p_vSequenceNumber < 10)
		oZeroes += "0000";
	else if(p_vSequenceNumber < 100)
		oZeroes += "000";
	else if(p_vSequenceNumber < 1000)
		oZeroes += "00";
	else if(p_vSequenceNumber < 10000)
		oZeroes += "0";

	char vpSeqNumber[32];
	sprintf(vpSeqNumber, "%d", (int) p_vSequenceNumber);
	string oFile = "RenderOutput" + oZeroes + string(vpSeqNumber) + oExtension;
	string oRenderFile/* = ParseFullPath(oFile, DIRECTORY_ID_RENDER_TARGET);
	return oRenderFile;*/
	return "";
}

string EMBeMediaUtility::MakeAudioRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID)
{
	string oZeroes = "";
	if(p_vSequenceNumber < 10)
		oZeroes += "0000";
	else if(p_vSequenceNumber < 100)
		oZeroes += "000";
	else if(p_vSequenceNumber < 1000)
		oZeroes += "00";
	else if(p_vSequenceNumber < 10000)
		oZeroes += "0";

	char vpSeqNumber[32];
	sprintf(vpSeqNumber, "%d", (int) p_vSequenceNumber);

	char vpIDNumber[32];
	sprintf(vpIDNumber, "%d", (int) p_vUniqueID);

	string oFile = string(vpIDNumber) + "take" + oZeroes + string(vpSeqNumber) + string(".wav");
	return /*ParseFullPath(oFile, DIRECTORY_ID_AUDIO_DATA_USED)*/ "";
}

string EMBeMediaUtility::MakeVideoRecordingFileName(uint32 p_vSequenceNumber, int32 p_vUniqueID)
{
	string oZeroes = "";
	if(p_vSequenceNumber < 10)
		oZeroes += "0000";
	else if(p_vSequenceNumber < 100)
		oZeroes += "000";
	else if(p_vSequenceNumber < 1000)
		oZeroes += "00";
	else if(p_vSequenceNumber < 10000)
		oZeroes += "0";

	char vpSeqNumber[32];
	sprintf(vpSeqNumber, "%d", (int) p_vSequenceNumber);

	char vpIDNumber[32];
	sprintf(vpIDNumber, "%d", (int) p_vUniqueID);

	string oFile = string(vpIDNumber) + "footage" + oZeroes + string(vpSeqNumber);
	return /*ParseFullPath(oFile, DIRECTORY_ID_VIDEO_DATA)*/"";
}

EMMediaFormat* EMBeMediaUtility::GetSystemAudioFormat()
{
	if(m_opSystemAudioFormat == NULL) m_opSystemAudioFormat = new EMMediaFormat(EM_TYPE_RAW_AUDIO);
	return m_opSystemAudioFormat;
}

EMMediaFormat* EMBeMediaUtility::GetSystemVideoFormat()
{
	if(m_opSystemVideoFormat == NULL) m_opSystemVideoFormat = new EMMediaFormat(EM_TYPE_RAW_VIDEO);
	return m_opSystemVideoFormat;
}

void EMBeMediaUtility::ClearData()
{
	delete m_opSystemAudioFormat;
	m_opSystemAudioFormat = NULL;
	delete m_opSystemVideoFormat;
	m_opSystemVideoFormat = NULL;
}

#endif
