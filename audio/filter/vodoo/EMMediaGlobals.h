#ifndef __EM_MEDIA_GLOBALS
#define __EM_MEDIA_GLOBALS

#include <list>

#include "EMBeMediaUtility.h"

#include "EMGlobals.h"

class EMMediaDataBuffer;

const int64 EM_TICKS_PER_SECOND = 1000000L;

const int EM_LOW_RES_AUDIO_SKIP_RATE = 100; // i.e. the low res audio file contains 1/100 of the samples in the hi res audio file

typedef EMMediaDataBuffer* (*BufferMixerFunction) (list<EMMediaDataBuffer*>*);

const int32 EM_PRODUCTION_THREADPRIO_ACTIVE 	= 80;
const int32 EM_PRODUCTION_THREADPRIO_INACTIVE 	= 2;
const int32 EM_MEDIA_ERROR 						= -1;

const int32 EM_PORT_MESSAGE_SHUTDOWN 			= 0x60000000L;
const int32 EM_PORT_MESSAGE_INCOMING_BUFFER		= 0x60000001L;
const int32 EM_PORT_MESSAGE_FLUSH_QUEUE			= 0x60000002L;
const int32 EM_PORT_MESSAGE_BUFFER_FEED_SUSPENDING= 0x60000003L;
const int32 EM_PORT_MESSAGE_BUFFER_FEED_RESUMING= 0x60000004L;
const int32 EM_PORT_MESSAGE_RESET				= 0x60000005L;
//const int32 EM_NODE_PAUSE						= 0x60000003L;
//const int32 EM_NODE_RESUME					= 0x60000003L;

const int32 EM_AUDIO_FADER_MAX 					= 127;
const int32 EM_AUDIO_FADER_MIN 					= 0;
const int32 EM_AUDIO_PAN_MAX 					= 64;
const int32 EM_AUDIO_PAN_MIN 					= -64;

const int32 EM_AUDIO_BUFFER_SIZE 				= 4096; //8192 + 4096;
const int32 EM_MIDI_BUFFER_SIZE					= 400;
const int32 EM_AUDIO_NUM_CHANNELS				= 2;
const int32 EM_AUDIO_SAMPLESIZE					= 2;
const float EM_AUDIO_READAHEAD					= 0.75; //seconds
//const float EM_VIDEO_FRAME_RATE					= 30;//12.0097;

const int32 EM_OUTPUT_TYPE_ANY 					= 0xF;
const int32 EM_OUTPUT_TYPE_PHYSICAL 			= 0x1;
const int32 EM_OUTPUT_TYPE_EFFECTSLOT 			= 0x2;
const int32 EM_OUTPUT_TYPE_MASTER 				= 0x4;
const int32 EM_OUTPUT_TYPE_PREEFFECTS 			= 0x8;

const int32 EM_TIMED_EVENT_SHUTDOWN 			= BTimedEventQueue::B_USER_EVENT + 1;
const int32 EM_TIMED_EVENT_FLUSH_CASH			= BTimedEventQueue::B_USER_EVENT + 2;
const int32 EM_TIMED_EVENT_RESET				= BTimedEventQueue::B_USER_EVENT + 3;
//const int32 EM_EVENT_PAUSE					= BTimedEventQueue::B_USER_EVENT + 4;
//const int32 EM_EVENT_RESUME					= BTimedEventQueue::B_USER_EVENT + 5;

const uint32 EM_MESSAGE_BEGIN_BUFFER_UP			= 665;
const uint32 EM_MESSAGE_BEGIN_PLAYORRECORD		= 666;
const uint32 EM_MESSAGE_STOP_PLAYORRECORD		= 667;
const uint32 EM_MESSAGE_TIME_WAS_SEEKED			= 668;

//const uint32 EM_MESSAGE_TIMER_STARTED			= 666;
//const uint32 EM_MESSAGE_PAUSE					= 667;
//const uint32 EM_MESSAGE_RESUME				= 668;
//const uint32 EM_MESSAGE_TIMER_STOPPED			= 669;
//const uint32 EM_MESSAGE_START					= 669;
//const uint32 EM_MESSAGE_STOP					= 670;
const uint32 EM_MESSAGE_BUFFER_SIZE_CHANGED		= 671;
const uint32 EM_MESSAGE_AUDIO_TRACK_ACTIVATED	= 672;
const uint32 EM_MESSAGE_VIDEO_TRACK_ACTIVATED	= 673;
const uint32 EM_MESSAGE_AUDIO_TRACK_DEACTIVATED = 674;
const uint32 EM_MESSAGE_VIDEO_TRACK_DEACTIVATED = 675;
const uint32 EM_MESSAGE_BUFFER_DELETION			= 676;
const uint32 EM_MESSAGE_TRACK_DEST_CHANGE		= 677;
const uint32 EM_MESSAGE_MIDI_RECORDING_STARTED	= 678;
const uint32 EM_MESSAGE_MIDI_RECORDING_STOPPED	= 679;
const uint32 EM_MESSAGE_MIDI_EVENT				= 680;

const int32 EM_WRITE							= 0x01;
const int32 EM_READ								= 0x00;

#define EM_THREAD_NAME_AUDIO_RECORDER "Audio Recorder thread"
#define EM_THREAD_NAME_VIDEO_RECORDER "Video Capture thread"
#define EM_THREAD_NAME_RENDERING_NODE "Media Rendering thread"
#define EM_THREAD_NAME_AUDIO_PROCESSING "Audio Processing thread"
#define EM_THREAD_NAME_VIDEO_PROCESSING "Video Processing thread"
#define EM_THREAD_NAME_VIDEO_BUFFER_MONITOR "Video Buffer Monitor thread"
#define EM_THREAD_NAME_VIDEO_CONSUMER "Video previewing thread"

enum EMMediaTimerState
{
	EM_STOPPED,
	EM_RECORDING,
	EM_PLAYING,
	EM_COUNTING_IN_FOR_PLAYBACK,
	EM_COUNTING_IN_FOR_RECORDING
};

enum EMMuteState
{
	EM_MUTE_ON,
	EM_MUTE_ON_BY_SOLO,
	EM_MUTE_OFF
};

enum _EMMediaType
{
	EM_TYPE_UNKNOWN								= 0,			//000 000 000 000 000
	EM_TYPE_ANY 								= 0xFFFF,
	
	EM_TYPE_RAW_AUDIO_VIDEO						= 0x02D,		//000 101 101 (EM_TYPE_RAW_AUDIO | EM_TYPE_RAW_VIDEO)

	EM_TYPE_UNKNOWN_VIDEO						= 0x004,		//000 000 100 (Don't know if it's raw of encoded - just that it's some video)
	EM_TYPE_ANY_VIDEO 							= 0x007,		//000 000 111
	EM_TYPE_RAW_VIDEO 							= 0x005,		//000 000 101
	EM_TYPE_ENCODED_VIDEO 						= 0x006,		//000 000 110

	EM_TYPE_UNKNOWN_AUDIO						= 0x020,		//000 100 000 (Don't know if it's raw of encoded - just that it's some audio)
	EM_TYPE_ANY_AUDIO 							= 0x038,		//000 111 000
	EM_TYPE_RAW_AUDIO 							= 0x028,		//000 101 000
	EM_TYPE_ENCODED_AUDIO 						= 0x030,		//000 110 000

	EM_TYPE_EMPTY 								= 0x040,		//001 000 000
	EM_TYPE_MIDI 								= 0x080,		//010 000 000
	EM_TYPE_TRANSITION 							= 0x100,		//100 000 000

	EM_TYPE_BMP									= 0x1000,		//0001 0000 0000 0000
	EM_TYPE_GIF									= 0x2000,		//0010 0000 0000 0000
	EM_TYPE_JPG									= 0x4000,		//0100 0000 0000 0000
	EM_TYPE_TGA									= 0x8000,		//1000 0000 0000 0000
	EM_TYPE_ANY_IMAGE							= 0xf000,		//1111 0000 0000 0000

	EM_TYPE_ANY_GFX								= 0xf007		//1111 0000 0000 0111
};

enum EMMediaFamily 
{
	EM_FAMILY_AVI,
	EM_FAMILY_QUICKTIME,
	EM_FAMILY_TARGA_SEQUENCE,
	EM_FAMILY_BITMAP_SEQUENCE,
	EM_FAMILY_MPEG
};

enum EMPositionType
{
	EM_SAMPLES,
	EM_MICROSECONDS,
	EM_24_FRAMES,
	EM_25_FRAMES,
	EM_30_FRAMES,
	EM_30_DROP_FRAMES,
	EM_UNKNOWN
};

enum EMPluginType
{
	EM_WINDOWS_VST_AUDIO,
	EM_WINDOWS_DIRECTX_AUDIO,
	EM_WINDOWS_DIRECTX_VIDEO_TRANSITION,
	EM_BEOS_VST_AUDIO,
	EM_NATIVE_VIDEO_EFFECT,
	EM_NATIVE_VIDEO_TRANSITION,
	EM_NATIVE_AUDIO_EFFECT,
	EM_DESTRUCTIVE,
	EM_DESTRUCTIVE_NATIVE
};

enum EMDestinationPosition
{
	EM_CLIP = 0,
	EM_TRACK = 1,
	EM_EFFECT = 2,
	EM_OUTPUT = 3,
	EM_TRANSITION = 4,
	EM_TRANSITION_TRACK = 5
};

class EMCodecInfo
{
public:
	EMCodecInfo(string a, string b) { m_oLongName = a; m_oShortName = b; };
	string m_oLongName;
	string m_oShortName;
};

class EMCodecFormat
{
public:
	bool m_vRawAudio;
	bool m_vRawVideo;
		
	string m_oFormatShortName;
	string m_oFormatLongName;
		
	list<EMCodecInfo> m_oRawAudioCodecShortNames;
	list<EMCodecInfo> m_oRawVideoCodecShortNames;
};

class EMMediaTrackInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };
	bool IsMIDI() { return ((m_eType & EM_TYPE_MIDI) > 0); };
	bool IsTransition() { return ((m_eType & EM_TYPE_TRANSITION) > 0); };

	int32 m_vID;
	EMMediaType	m_eType;
	uint32 m_vFaderLevel;
	int32 m_vPanLevel;
	int64 m_vStartOffset;
	uint32 m_vPriority;
	EMMuteState m_eMuteState;
	bool m_vIsSoloActive;
	bool m_vIsArmActive;
	bool m_vIsRenderActive;
	string m_oName;
	int32 m_vInputID;
	int32 m_vOutputID;
	bool m_vOutputIsEffectTrack; //True if the m_vOutputID is an effect track instead of a physical output!
	list<int32> m_oMediaClipIDs;
	uint32 m_vMIDIChannel;		//If MIDI track... range from 1 - 16
	int32 m_vVelocityOffset;	//If MIDI track... range from -127 - +127 ?
	int32 m_vKeyOffset;			//If MIDI track... range from -127 - +127 ?
};

class EMMediaClipInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };
	bool IsMIDI() { return ((m_eType & EM_TYPE_MIDI) > 0); };
	bool IsTransition() { return ((m_eType & EM_TYPE_TRANSITION) > 0); };

	int32 m_vID;
	EMMediaType m_eType;
	int64 m_vActiveStart; //I.e. the actual song-start-time (in microseconds) of the data after mark-in has been accounted for.
	int64 m_vMarkInDuration; //Offset of number-of-microseconds-from-the-start-of-a-file
	int64 m_vMarkOutDuration; //Offset of number-of-microseconds-from-the-end-of-a-file
	int64 m_vPhysicalDuration; //number of microseconds
	int64 m_vActiveDuration; //number of microseconds
	string m_oName;
	string m_oFileName;
	int32 m_vMediaEntryID; //The media entry id number of the pool entry to which this clip is connected
	int32 m_vMediaTrackID; //The media track id number of the track to which this clip belongs
	int32 m_vMediaEffectTrackID; //The media effect track ID (if any) that this clip is sending to (-1 if none!)
};

class EMMediaPoolEntryInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };

	int32 m_vID;					//The media pool entry ID
	EMMediaType m_eType;			//The type of the entry
	string m_oFileName;				//The full path and filename of the entry
	string m_oOriginalFileName;		//The full path and filename of the original file that this entry is based upon, before extractions and conversions
	string m_oPeaksFileName;		//The full path and filename of the peaks-file (if any) that is associated with this entry (if and only if it's an audio entry!)
	int32 m_vUsageCount;			//The number of instances of this entry that exist in the project (clips)
	uint64 m_vLength;				//The length in microseconds of this file
	uint64 m_vSize;					//The size in bytes of this file
	uint64 m_vFrameRate;			//The framerate of this file (eg 25 for video or 44100 for audio)
	uint64 m_vBitDepth;				//The bit depth of the file (eg 32 for video or 16 for audio)
	list<int32> m_oClipIDs;			//List of all Media Clip ID's that stems from this entry
};

class EMMediaEffectTrackInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };
	bool IsMIDI() { return ((m_eType & EM_TYPE_MIDI) > 0); };
	bool IsTransition() { return ((m_eType & EM_TYPE_TRANSITION) > 0); };

	int32 m_vID;					//ID # of this FX track
	EMMediaType	m_eType;			//The type of this effect track
	string m_oName;					//The name of this effect track
	int32 m_vOutputID;				//The RealtimeOutputDescriptor ID number of the receiving physical output (if it's not an effect track - coz then this ID is an effect track ID)!
	bool m_vOutputIsEffectTrack;	//Flag to tell if the m_vOutputID is a RealtimeOutputDescriptor ID or a MediaEffectTrack ID (ie if the effect track is connected to another effect track or directly to a physical output).
	list<int32> m_oMediaEffectIDs;	//A list of EMPluginEntry IDs (ordered) that this FX track holds
};

class EMMediaEffectInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };
	bool IsMIDI() { return ((m_eType & EM_TYPE_MIDI) > 0); };
	bool IsTransition() { return ((m_eType & EM_TYPE_TRANSITION) > 0); };

	int32 m_vID;				//The ID number of the effect instance!
	EMMediaType m_eType;		//if it is audio, video, any, whatever...
	EMPluginType m_ePluginType; //if it is vst, direcx, for audio, for video, windows, beos, etc...
	string m_oPluginName;
	int32 m_vSlotNumber;		//Where in the track is this effect? range from 0 and up.
	int32 m_vEntryID;			//the ID number of the effect entry of which this object is an instance!
	float m_vDryWetMix;			//The dry/wet mix value ranging from 0.0 to 1.0.
};

class EMMediaEffectEntryInfo
{
public:
	bool IsAudio() { return ((m_eType & EM_TYPE_ANY_AUDIO) > 0); };
	bool IsVideo() { return ((m_eType & EM_TYPE_ANY_VIDEO) > 0); };
	bool IsMIDI() { return ((m_eType & EM_TYPE_MIDI) > 0); };
	bool IsTransition() { return ((m_eType & EM_TYPE_TRANSITION) > 0); };
	bool IsDestructive() { return  (m_ePluginType == EM_DESTRUCTIVE || m_ePluginType == EM_DESTRUCTIVE_NATIVE); };
	bool IsNative() { return  (m_ePluginType == EM_DESTRUCTIVE_NATIVE || m_ePluginType == EM_NATIVE_VIDEO_TRANSITION || m_ePluginType == EM_NATIVE_VIDEO_EFFECT); };

	int32 m_vID;				//The ID number of the effect entry!
	EMMediaType m_eType;		//if it is audio, video, any, whatever...
	EMPluginType m_ePluginType; //if it is vst, direcx, for audio, for video, windows, beos, etc...
	string m_oPluginName;
	int32 m_vInstanceCount;		//The number of instances there is of this effect entry!

};

#endif
