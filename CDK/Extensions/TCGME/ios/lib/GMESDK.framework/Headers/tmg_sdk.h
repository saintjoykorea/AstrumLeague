/****************************************************************************
CopyRight (c) Tencent Technology(ShenZhen) Co., Ltd
TMGSDK Header
*****************************************************************************/
#pragma once

#if defined (__APPLE__)
#define QAVSDK_API __attribute__((visibility("default"))) extern "C"
#define QAVSDK_CALL
#elif defined(ANDROID) || defined(__linux__)
#define QAVSDK_API extern "C"  __attribute__ ((visibility("default")))
#define QAVSDK_CALL
#elif defined(_WIN32)
	#ifdef _AV_IMPLEMENT_
		#define AV_EXPORT __declspec(dllexport)
	#else
		#define AV_EXPORT __declspec(dllimport)
	#endif
#define QAVSDK_API extern "C" AV_EXPORT
#define QAVSDK_CALL __cdecl
#endif

#include "av_error.h"
#include "auth_buffer.h"

typedef enum {
    ITMG_PERMISSION_GRANTED = 0 ,//麦克风已授权
    ITMG_PERMISSION_Denied = 1,//麦克风被禁用
    ITMG_PERMISSION_NotDetermined = 2 ,//尚未弹出权限框向用户申请权限
    ITMG_PERMISSION_ERROR = 3,//接口调用错误
}ITMG_RECORD_PERMISSION;

typedef enum {
	ITMG_ROOM_TYPE_FLUENCY = 1,
	ITMG_ROOM_TYPE_STANDARD = 2,
	ITMG_ROOM_TYPE_HIGHQUALITY = 3,
}ITMG_ROOM_TYPE;

typedef enum {
	ITMG_ROOM_CHANGE_EVENT_ENTERROOM = 1,
	ITMG_ROOM_CHANGE_EVENT_START = 2,
	ITMG_ROOM_CHANGE_EVENT_COMPLETE = 3,
	ITMG_ROOM_CHANGE_EVENT_REQUEST = 4
}ITMG_ROOM_TYPE_SUB_EVENT;


/*
 *	//TMG event enumeration that from a callback event
 */
typedef enum {
	ITMG_MAIN_EVENT_TYPE_NONE = 0,

	//Notification of entering a room, triggered by EnterRoom API.
	ITMG_MAIN_EVENT_TYPE_ENTER_ROOM = 1,
	//Notification of exiting a room, triggered by ExitRoom API.
	ITMG_MAIN_EVENT_TYPE_EXIT_ROOM = 2,
	//Notification of room disconnection due to network or other reasons, which will trigger automatically.
	ITMG_MAIN_EVENT_TYPE_ROOM_DISCONNECT = 3,
	//Notification of the updates of room members, the notification contains detailed information, refer to ITMG_EVENT_ID_USER_UPDATE.
	ITMG_MAIN_EVNET_TYPE_USER_UPDATE = 4,


	ITMG_MAIN_EVENT_TYPE_NUMBER_OF_USERS_UPDATE=7,// number of users in current room
	ITMG_MAIN_EVENT_TYPE_NUMBER_OF_AUDIOSTREAMS_UPDATE = 8,// number of audioStreams in current room
	//Notification of room reconnection happened, which indicates services will be temporarily unavailable.
	ITMG_MAIN_EVENT_TYPE_RECONNECT_START = 11,
	//Notification of room reconnection succeeded, which indicates services have recovered.
	ITMG_MAIN_EVENT_TYPE_RECONNECT_SUCCESS = 12,
	//Notification of RoomType have been Changed by Other EndUser
	ITMG_MAIN_EVENT_TYPE_CHANGE_ROOM_TYPE = 21,

	ITMG_MAIN_EVENT_TYPE_AUDIO_DATA_EMPTY = 22,

	//Notify user the default speaker device is changed in the PC, refresh the Speaker devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_SPEAKER_DEFAULT_DEVICE_CHANGED = 1008,
	//Notify user new Speaker device in the PC, refresh the Speaker devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_SPEAKER_NEW_DEVICE = 1009,
	//Notify user speaker device lost from the PC, refresh the Speaker devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_SPEAKER_LOST_DEVICE = 1010,
	//Notify user new mic device in the PC, refresh the Speaker devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_MIC_NEW_DEVICE = 1011,
	//Notify user mic device lost from the PC, refresh the Speaker devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_MIC_LOST_DEVICE = 1012,
	//Notify user the default mic device is changed in the PC, refresh the mic devices when you recv this event.
	ITMG_MAIN_EVENT_TYPE_MIC_DEFAULT_DEVICE_CHANGED = 1013,

	//Notification of volumes of users in room
	ITMG_MAIN_EVENT_TYPE_USER_VOLUMES = 1020,
	
	//quality information
	ITMG_MAIN_EVENT_TYPE_CHANGE_ROOM_QUALITY = 1022,  

	//Notification of accompany finished
	ITMG_MAIN_EVENT_TYPE_ACCOMPANY_FINISH = 1090,
	
    ITMG_MAIN_EVNET_TYPE_AUDIO_READY = 2000,
	
	// Notification of PTT Record
	ITMG_MAIN_EVNET_TYPE_PTT_RECORD_COMPLETE = 5001,
	// Notification of PTT Upload
    ITMG_MAIN_EVNET_TYPE_PTT_UPLOAD_COMPLETE = 5002,
	// Notification of PTT Download
    ITMG_MAIN_EVNET_TYPE_PTT_DOWNLOAD_COMPLETE = 5003,
	// Notification of PTT Play
    ITMG_MAIN_EVNET_TYPE_PTT_PLAY_COMPLETE = 5004,
	// Notification of PTT 2Text
    ITMG_MAIN_EVNET_TYPE_PTT_SPEECH2TEXT_COMPLETE = 5005,
    // Notification of StreamRecognition
    ITMG_MAIN_EVNET_TYPE_PTT_STREAMINGRECOGNITION_COMPLETE = 5006,

} ITMG_MAIN_EVENT_TYPE;

/*
 *	对应，ITMG_MAIN_EVENT_TYPE::ITMG_MAIN_EVNET_TYPE_USER_UPDATE//correspond,ITMG_MAIN_EVENT_TYPE::ITMG_MAIN_EVNET_TYPE_USER_UPDATE
 *  详情枚举//Details enumeration
 */
typedef enum {
	//Notification of entering a room
	ITMG_EVENT_ID_USER_ENTER=1,
	//Notification of exiting a room
	ITMG_EVENT_ID_USER_EXIT=2,
	//Notification of member up video
	ITMG_EVENT_ID_USER_HAS_CAMERA_VIDEO=3,
	//Notification of member not up video any more
	ITMG_EVENT_ID_USER_NO_CAMERA_VIDEO=4,

	//Notification of member audio event
	ITMG_EVENT_ID_USER_HAS_AUDIO=5,
	//Notification of no member audio event is received for 2 seconds
	ITMG_EVENT_ID_USER_NO_AUDIO=6,
} ITMG_EVENT_ID_USER_UPDATE;

typedef	enum {
	//Do not print the log
	TMG_LOG_LEVEL_NONE	  = 0, 
	//Used for critical log
	TMG_LOG_LEVEL_ERROR   = 1,
	//Used to prompt for information
	TMG_LOG_LEVEL_INFO    = 2,
	//For development and debugging
	TMG_LOG_LEVEL_DEBUG   = 3,
	//For high-frequency printing information
	TMG_LOG_LEVEL_VERBOSE = 4,
} ITMG_LOG_LEVEL;

#define DEVICEID_DEFAULT "0"
/*
 *voice change types,
 */
typedef enum {
    ITMG_VOICE_TYPE_ORIGINAL_SOUND = 0,
    ITMG_VOICE_TYPE_LOLITA = 1,
    ITMG_VOICE_TYPE_UNCLE = 2,
	ITMG_VOICE_TYPE_INTANGIBLE = 3,
	ITMG_VOICE_TYPE_DEAD_FATBOY = 4,
    ITMG_VOICE_TYPE_HEAVY_MENTAL = 5,
	ITMG_VOICE_TYPE_DIALECT = 6,
	ITMG_VOICE_TYPE_INFLUENZA = 7,
	ITMG_VOICE_TYPE_CAGED_ANIMAL = 8,
    ITMG_VOICE_TYPE_HEAVY_MACHINE = 9,
    ITMG_VOICE_TYPE_STRONG_CURRENT = 10,
	ITMG_VOICE_TYPE_KINDER_GARTEN = 11,
	ITMG_VOICE_TYPE_HUANG = 12,
} ITMG_VOICE_TYPE;

typedef enum {
	ITMG_KARAOKE_TYPE_ORIGINAL = 0,
	ITMG_KARAOKE_TYPE_POP = 1,
	ITMG_KARAOKE_TYPE_ROCK = 2,
	ITMG_KARAOKE_TYPE_RB = 3,
	ITMG_KARAOKE_TYPE_DANCE = 4,
	ITMG_KARAOKE_TYPE_HEAVEN = 5,
	ITMG_KARAOKE_TYPE_TTS = 6,
} ITMG_KARAOKE_TYPE;

typedef enum
{
	ITMG_RANGE_AUDIO_MODE_WORLD = 0x0,
	ITMG_RANGE_AUDIO_MODE_TEAM = 0x1,
} ITMG_RANGE_AUDIO_MODE;

class ITMGRoom;
class ITMGAudioCtrl;
class ITMGAudioEffectCtrl;
class ITMGPTT;
class ITMGContext;
class ITMGDelegate;

//////////////////////////////////////////////////////////////////////////
// Interface definition section
//////////////////////////////////////////////////////////////////////////
QAVSDK_API ITMGContext* QAVSDK_CALL ITMGContextGetInstance();

class ITMGContext {
protected:
    virtual ~ITMGContext() {}
    
public:
	//////////////////////////////////////////////////////////////////////////
	// basic API

	// Trigger OnEvent in the thread needed
	// should be called in a timer or in some render Update Event, or SDK won't work
	virtual void Poll()= 0;
	// Totally Pause SDK, including releasing Physical Devices Occupation
	virtual int Pause() = 0;
	// Return Back to the Status before Pause() if it can
	virtual int Resume() = 0;

    //Set the Log level
	//		[in]levelWrite -> the level of log writen to logfile, default TMG_LOG_LEVEL_INFO 
	//		[in]levelPrint -> the level of log printed to console, default TMG_LOG_LEVEL_ERROR 
	virtual int SetLogLevel( ITMG_LOG_LEVEL levelWrite, ITMG_LOG_LEVEL levelPrint) = 0;

	// Set a folder path to locate logs
	virtual int SetLogPath(const char* logDir) = 0;

	// Get the folder path where logs locate
	virtual const char* GetLogPath() = 0;

	// Set TMG callback(must be set up first)
	//		[in]delegate --> TMGSDK callback you can search in ITMGDelegate
	virtual void SetTMGDelegate(ITMGDelegate* delegate) = 0;

	// Get the SDK version
	virtual const char* GetSDKVersion()=0;

	// Optional, Give Application informations, just for backstage statistics, etc.
	virtual void SetAppVersion(const char* appVersion) = 0;

	// Init SDK with relevant informations
	// sdkAppId Application's ID 
	// openId EndUsers's openId, it should be an INT64 which is bigger than 10000.
	virtual int Init(const char* sdkAppId, const char* openId) = 0;

	// Uninit SDK, Release all resources, should be openId is about to change or Application is about to exit
	virtual int Uninit() = 0;
	
	//////////////////////////////////////////////////////////////////////////
	// RealTime API

	// Enter a room for communication
	// [in]roomID	-->	Indicating a communication
	// [in]roomType	-->	Special the quality of the room
	// [in]authBuff	-->	Authentication code in Tencent Cloud, 
    // [in]buffLen		-->	String length of Authentication cod
    /// if return value is AV_OK then means the enter event post success，
    /// then u can wait the ITMG_MAIN_EVENT_TYPE_ENTER_ROOM event.
	virtual int EnterRoom(const char* roomID, ITMG_ROOM_TYPE roomType, const char* authBuff, int buffLen) = 0;


	/// Exit the room
	/// It's ansy-method more info refer to ITMGRoomDelegate
	/// if return value is AV_OK then means the exit event post success，
	/// then u can wait the ITMG_MAIN_EVENT_TYPE_EXIT_ROOM event.
	virtual int ExitRoom() = 0;

	// Check if the user enter the room or not
	virtual bool IsRoomEntered() = 0;

	// Get a room API instance, only available when in room
	virtual ITMGRoom* GetRoom() = 0;

	// Get a AudioCtrl API instance, only available when in room
	virtual ITMGAudioCtrl* GetAudioCtrl() = 0;

	// Get a AudioEffectCtrl API instance, only available when in room
	virtual ITMGAudioEffectCtrl* GetAudioEffectCtrl() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Recording API
	//Get a PttCtrl API instance
	virtual ITMGPTT* GetPTT() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Advanced API, don't use unless you know what would happen

	//// warning : never call this API for any reason, it's only for internal use
	virtual int SetTestEnv(bool isTestEnv) = 0;

	virtual int SetRecvMixStreamCount(int nCount) = 0;
    
    // these two APIs are associated to Range Audio
    virtual int SetRangeAudioMode(ITMG_RANGE_AUDIO_MODE gameAudioMode) = 0;
    virtual int SetRangeAudioTeamID(int teamID) = 0;


	virtual int SetAdvanceParams(const char* key, const char* object) = 0;
	virtual const char* GetAdvanceParams(const char* key) = 0;

    
    virtual ITMG_RECORD_PERMISSION CheckMicPermission() = 0;
};

//////////////////////////////////////////////////////////////////////////
// TMG The whole callback
class ITMGDelegate {
public:
    virtual ~ITMGDelegate() {};
public:
	//EventType callback,you can search in ITMGContext.TMGDelegate
	//Data is a JSON string format under Windows platform, you can read more about key-value in Developer Manual
	//		[in]eventType -->	Event types 
	//		[in]data	  -->	Detailed description of the event(json format),you can read more about data in Developer Manual

    virtual void OnEvent(ITMG_MAIN_EVENT_TYPE eventType, const char* data) = 0;
};

class ITMGRoom {
public:
	virtual ~ITMGRoom() {} ;

	// Get the quality tips,by this function you can check out SDK quality information.
	virtual const char* GetQualityTips() = 0;
	virtual void ChangeRoomType(ITMG_ROOM_TYPE roomType) = 0;
	virtual int GetRoomType() = 0;

	// range : if Spatializer is enabled or WorldMode is selected:
	//		user can't hear the speaker if the distance between them is larger than the range;
	//		by default, range = 0. which means without calling UpdateAudioRecvRange no audio would be available.
	virtual int UpdateAudioRecvRange(int range) = 0;

	// Tell Self's position and rotation information to GME for function: Spatializer && WorldMode
	// position and rotate should be under the world coordinate system specified by forward, rightward, upward direction.
	// for example: in Unreal(forward->X, rightward->Y, upward->Z); in Unity(forward->Z, rightward->X, upward->Y)
	// position: self's position
	// axisForward: the forward axis of self's camera rotation
	// axisRightward: the rightward axis of self's camera rotation
	// axisUpward: the upward axis of self's camera rotation
	virtual int UpdateSelfPosition(int position[3], float axisForward[3], float axisRightward[3], float axisUpward[3]) = 0;
};

//////////////////////////////////////////////////////////////////////////
//Audio control instances
class ITMGAudioCtrl {
public:
	class TMGAudioDeviceInfo
	{
	public:
		const char* pDeviceID;
		const char* pDeviceName;
	};
public:
	virtual ~ITMGAudioCtrl() {};
    
	// a recommended way of accessing microphone. EnableMic(value) = EnableAudioCaptureDevice(value) + EnableAudioSend(value)
	virtual int EnableMic(bool enable) = 0;
	// a shortcut of mic state [0 is off; 1 is on] = IsAudioSendEnabled() && IsAudioCaptureDeviceEnabled()
	virtual int GetMicState() = 0;

	// a recommended way of accessing speaker. EnableSpeaker(value) = EnableAudioPlayDevice(value) + EnableAudioRecv(value)
	virtual int EnableSpeaker(bool enable) = 0;
	// a shortcut of speaker state [0 is off; 1 is on] = IsAudioRecvEnabled() && IsAudioPlayDeviceEnabled()
	virtual int GetSpeakerState() = 0;
	
    // Enable Audio Device, GME won't automatically open capture and play devices unless you open it.
    // note: it can only be called when in room, ExitRoom will automatically close devices
    // note: when capture device is about to open in kinds of phones, Authority is needed and AudioCategory will change
    // note: if currently no device is available, GME will automatically retry until open successfully if Enable(true) is called
    //
    // Cases:
    //      1. when user click mic/speaker buttons:
    //          option 1: call EnableAudioCaptureDevice&&EnableAudioSend together when mic button clicked and EnableAudioPlayDevice&&EnableAudioRecv when speaker button clicked
	//			option 1 is recommended for most of game Apps.
    //          option 2: call EnableAudioCaptureDevice(true) && EnableAudioPlayDevice(true) just once(when enterRoom), just use EnableAudioSend/Recv to control stream.
	//			option 2 is recommended for some kind of social Apps
    //      2. if you want to release devices for other modules' usage, it's better to use PauseAudio/ResumeAudio.
    virtual int EnableAudioCaptureDevice(bool enable) = 0;
    virtual int EnableAudioPlayDevice(bool enable) = 0;
    
    // Get the state of microphone && speaker device
    virtual bool IsAudioCaptureDeviceEnabled() = 0;
    virtual bool IsAudioPlayDeviceEnabled() = 0;
    
	// Enable/Disable sending audio data, only take effect when capture device opened, @see EnableAudioCaptureDevice
	// ExitRoom will automatically call EnableAudioSend(false)
	virtual int EnableAudioSend(bool bEnable) = 0;
	// Enable/Disable receiving audio data, only take effect when player device opened, @see EnableAudioPlayDevice
	// ExitRoom will automatically call EnableAudioRecv(false)
	virtual int EnableAudioRecv(bool enable) = 0;

	// Get the state of sending audio data or receiving audio data
	virtual bool IsAudioSendEnabled() = 0;
	virtual bool IsAudioRecvEnabled() = 0;

	// Get the energy value of the selected microphone(For example, you can use the energy value for drawing an audio column diagram)
	virtual int GetMicLevel() = 0;

	// Set the volume of the microphone
	//		[in]volume--->Audio Volume reference range[0 to 200],default value is 100
	virtual int SetMicVolume(int vol)=0;

	// Get the volume of microphone
	virtual int GetMicVolume()=0;

	// Get the energy value of the selected speaker(For example,you can use the energy value for drawing an audio column diagram)
	virtual int GetSpeakerLevel() = 0;

	// Set the volume of the speaker
	//		[in]nVolume	-->	Audio Volume reference range[0 to 200],default value is 100
	virtual int SetSpeakerVolume(int vol) = 0;

	// Get the volume of speaker
	virtual int GetSpeakerVolume() = 0;
    
    // Only available in Windows
    // Get the number of microphone devices
    //        [return]    Get the number of microphone devices
    virtual int GetMicListCount() = 0;
    
    // Only available in Windows
    // Get the list of microphone devices
    //        [in/out]ppDeviceInfoList---> The list of microphone devices that has been allocated memory
    //        [in]nCount---> The length of ppDeviceInfoList，you can get it by function GetMicListCount
    virtual int GetMicList(TMGAudioDeviceInfo* ppDeviceInfoList, int nCount) = 0;
    
    // Only available in Windows
    // Select a microphone device
    virtual int SelectMic(const char* pMicID) = 0;
    
    // Only available in Windows
    // Get the number of speaker devices
    //        [return] -->The number of speaker devices
    virtual int GetSpeakerListCount() = 0;
    
    // Only available in Windows
    // Get the list of speaker devices, devices that has been allocated memory
    //        [in/out]ppDeviceInfoList --> the list of speaker devices, devices that has been allocated memory
    //        [in]nCount --> The length of ppDeviceInfoList，you can get it by function GetSpeakerListCount
    virtual int GetSpeakerList(TMGAudioDeviceInfo* ppDeviceInfoList, int nCount) = 0;
    
    // Only available in Windows
    // Select a speaker
    virtual int SelectSpeaker(const char* pSpeakerID) = 0;
    
	// Enable or disable monitor
	//		[in]bEnable	-->	Enable or disable ear back(listen self's voice)
	virtual int EnableLoopBack(bool enable) = 0;

	// Add An OpenId to BlackList to block his audio
	virtual int AddAudioBlackList(const char* openId) = 0;

	// Remove An OpenId from BlackList to let his audio pass
	virtual int RemoveAudioBlackList(const char* openId) = 0;

	// Init Spatializer, If you want to use EnableSpatializer, InitSpatializer should be called on both speaker and listener's client side
	virtual int InitSpatializer(const char* modelPath) = 0;

	// Enable or disable the spatial audio
	//[in]bEnable	--> Enable or disable the spatial audio
    //[in]applyTeam	--> indicates whether to use spatial audio in the same team or not
	// note: InitSpatializer should be called on both speaker and listener's client side
	virtual int EnableSpatializer(bool bEnable, bool applyTeam) = 0;

	// Get the current spatial audio state
	virtual bool IsEnableSpatializer() = 0;
};

class ITMGAudioEffectCtrl {
public:
    virtual ~ITMGAudioEffectCtrl(){};
    
    virtual int StartAccompany(const char* filePath, bool loopBack, int loopCount, int msTime) = 0;
    virtual int StopAccompany(int duckerTime) = 0;
    virtual bool IsAccompanyPlayEnd() = 0;
    
    virtual int PauseAccompany() = 0;
    virtual int ResumeAccompany() = 0;

    virtual int SetAccompanyVolume(int vol) = 0;
    virtual int GetAccompanyVolume() = 0;
    
    virtual int GetAccompanyFileTotalTimeByMs() = 0;
    virtual int GetAccompanyFileCurrentPlayedTimeByMs() = 0;
    virtual int SetAccompanyFileCurrentPlayedTimeByMs(unsigned int time)  = 0;
    
    virtual int SetVoiceType(ITMG_VOICE_TYPE voiceType) = 0;
	virtual int SetKaraokeType(ITMG_KARAOKE_TYPE type) = 0;
};

class ITMGPTT{
public:
    virtual ~ITMGPTT(){};
    
	virtual int ApplyPTTAuthbuffer(const char* authBuffer, int authBufferLen) = 0;
    virtual int SetMaxMessageLength(int msTime)= 0;
    
    virtual void StartRecording(const char* filePath)= 0;
    virtual int StopRecording() = 0;
    virtual int CancelRecording()= 0;
    
    virtual void UploadRecordedFile(const char* filePath) = 0;
    virtual void DownloadRecordedFile(const char* fileId, const char* filePath) = 0;

    virtual void PlayRecordedFile(const char* filePath) = 0;
    virtual int StopPlayFile() = 0;

    virtual int GetMicLevel() = 0;
    virtual int SetMicVolume(int vol) = 0;
    virtual int GetMicVolume() = 0;
    
    virtual int GetSpeakerLevel() = 0;
    virtual int SetSpeakerVolume(int vol) = 0;
    virtual int GetSpeakerVolume() = 0;
    
    virtual void SpeechToText(const char* fileID) = 0;
    virtual void SpeechToText(const char* fileID,const char* speechLanguage) = 0;
	virtual void SpeechToText(const char* fileID,const char* speechLanguage,const char* translateLanguage) = 0;
    
    virtual int GetFileSize(const char* filePath) = 0;
    virtual int GetVoiceFileDuration(const char* filePath) = 0;
    
    virtual int StartRecordingWithStreamingRecognition(const char* filePath) = 0;
	virtual int StartRecordingWithStreamingRecognition(const char* filePath,const char*speechLanguage) = 0;
    virtual int StartRecordingWithStreamingRecognition(const char* filePath,const char*speechLanguage,const char*translateLanguage) = 0;
};

