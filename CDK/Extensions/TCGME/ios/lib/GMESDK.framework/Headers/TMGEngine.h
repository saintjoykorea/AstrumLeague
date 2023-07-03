//
//  ITMGEngine.h
//  QAVSDK
//
//  Created by tobinchen on 23/10/2017.
//  Copyright © 2017 tencent. All rights reserved.
//


#import <Foundation/Foundation.h>
#import "QAVError.h"



#if (TARGET_OS_OSX == 1)
#define TMG_NO_PTT_SUPPORT
#endif

typedef NS_ENUM(NSUInteger, ITMG_RECORD_PERMISSION) {
        ITMG_PERMISSION_GRANTED = 0 ,//麦克风已授权
        ITMG_PERMISSION_Denied = 1,//麦克风被禁用
        ITMG_PERMISSION_NotDetermined = 2,//尚未弹出权限框向用户申请权限
        ITMG_PERMISSION_ERROR = 3,//接口调用错误
};


typedef NS_ENUM(NSUInteger, ITMG_EVENT_ID_USER) {
    ITMG_EVENT_ID_USER_ENTER=1,// 成员进入房间事件
    ITMG_EVENT_ID_USER_EXIT=2,// 成员退出房间事件
    ITMG_EVENT_ID_USER_HAS_CAMERA_VIDEO=3,// 成员发送摄像头视频事件
    ITMG_EVENT_ID_USER_NO_CAMERA_VIDEO=4,// 成员停止发送摄像头视频事件
    ITMG_EVENT_ID_USER_HAS_AUDIO=5,// 收到成员音频事件
    ITMG_EVENT_ID_USER_NO_AUDIO=6,// 连续2秒未收到成员音频事件
};

typedef NS_ENUM(NSUInteger, ITMG_VOICE_TYPE) {
    ITMG_VOICE_TYPE_ORIGINAL_SOUND = 0,  /// 原声
    ITMG_VOICE_TYPE_LOLITA = 1,        /// 萝莉
    ITMG_VOICE_TYPE_UNCLE = 2,            /// 大叔
    ITMG_VOICE_TYPE_INTANGIBLE = 3,       /// 空灵
    ITMG_VOICE_TYPE_DEAD_FATBOY = 4,    /// 死肥宅
    ITMG_VOICE_TYPE_HEAVY_MENTAL = 5,   /// 重金属
    ITMG_VOICE_TYPE_DIALECT = 6,        /// 歪果仁
    ITMG_VOICE_TYPE_INFLUENZA = 7,      /// 感冒
    ITMG_VOICE_TYPE_CAGED_ANIMAL = 8,   /// 困兽
    ITMG_VOICE_TYPE_HEAVY_MACHINE = 9,  /// 重机器
    ITMG_VOICE_TYPE_STRONG_CURRENT = 10,    /// 强电流
    ITMG_VOICE_TYPE_KINDER_GARTEN = 11, /// 幼稚园
    ITMG_VOICE_TYPE_HUANG = 12,         /// 小黄人
};

typedef NS_ENUM(NSUInteger, ITMG_LOG_LEVEL) {
    ITMG_LOG_LEVEL_NONE = 0,   //Do not print the log
    ITMG_LOG_LEVEL_ERROR = 1,  //Used for critical log
    ITMG_LOG_LEVEL_INFO = 2, //Used to prompt for information
    ITMG_LOG_LEVEL_DEBUG = 3, //For development and debugging
    ITMG_LOG_LEVEL_VERBOSE = 4, //For high-frequency printing information
};


typedef NS_ENUM(NSUInteger, ITMG_MAIN_EVENT_TYPE) {
    ITMG_MAIN_EVENT_TYPE_NONE = 0,
    
    ITMG_MAIN_EVENT_TYPE_ENTER_ROOM = 1,//进入音视频房间
    ITMG_MAIN_EVENT_TYPE_EXIT_ROOM = 2,//退出音视频房间
    ITMG_MAIN_EVENT_TYPE_ROOM_DISCONNECT = 3,//房间因为网络等原因断开，
    ITMG_MAIN_EVNET_TYPE_USER_UPDATE = 4,//房间成员更新
    
    ITMG_MAIN_EVENT_TYPE_NUMBER_OF_USERS_UPDATE=7,//当前房间内人数更新
    ITMG_MAIN_EVENT_TYPE_NUMBER_OF_AUDIOSTREAMS_UPDATE = 8,//当前房间内音频流数量更新
    
    ITMG_MAIN_EVENT_TYPE_RECONNECT_START = 11,
    ITMG_MAIN_EVENT_TYPE_RECONNECT_SUCCESS = 12,
    ITMG_MAIN_EVENT_TYPE_CHANGE_ROOM_TYPE = 21, // 房间类型切换,sub_event_type子事件类型表示当前具体切换环节
    ITMG_MAIN_EVENT_TYPE_AUDIO_DATA_EMPTY = 22, 

	ITMG_MAIN_EVNET_TYPE_USER_VOLUMES = 1020,    //成员当前音量
	ITMG_MAIN_EVENT_TYPE_CHANGE_ROOM_QUALITY = 1022,    // 房间质量信息报告

    ITMG_MAIN_EVENT_TYPE_ACCOMPANY_FINISH = 1090,//伴奏播放完成通知

    ITMG_MAIN_EVNET_TYPE_PTT_RECORD_COMPLETE = 5001,//PTT录音完成
    ITMG_MAIN_EVNET_TYPE_PTT_UPLOAD_COMPLETE = 5002,//上传PTT完成
    ITMG_MAIN_EVNET_TYPE_PTT_DOWNLOAD_COMPLETE = 5003,//下载PTT完成
    ITMG_MAIN_EVNET_TYPE_PTT_PLAY_COMPLETE = 5004,//播放PTT完成
    ITMG_MAIN_EVNET_TYPE_PTT_SPEECH2TEXT_COMPLETE = 5005,//语音转文字完成
    ITMG_MAIN_EVNET_TYPE_PTT_STREAMINGRECOGNITION_COMPLETE = 5006,//流式语音转文字完成
    
};

typedef NS_ENUM(NSInteger,ITMG_ROOM_CHANGE_EVENT) {
    ITMG_ROOM_CHANGE_EVENT_NONE    = 0,
    ITMG_ROOM_CHANGE_EVENT_ENTERROOM = 1, //进房时候，被房间改变
    ITMG_ROOM_CHANGE_EVENT_START = 2, //已经在房间房间，类型开始切换
    ITMG_ROOM_CHANGE_EVENT_COMPLETE = 3,    //已经在房间，切换完成
    ITMG_ROOM_CHANGE_EVENT_REQUEST = 4,    //调用ChangeRoomType请求切换房间类型
};

typedef NS_ENUM(NSInteger, ITMG_ROOM_TYPE) {
    ITMG_ROOM_TYPE_FLUENCY      = 1,
    ITMG_ROOM_TYPE_STANDARD     = 2,
    ITMG_ROOM_TYPE_HIGHQUALITY  = 3,
};

typedef NS_ENUM(NSUInteger, ITMG_AUDIO_CATEGORY) {
    ITMG_CATEGORY_AMBIENT = 0,
    ITMG_CATEGORY_PLAYBACK = 1,
};

typedef NS_ENUM(NSUInteger, ITMG_KARAOKE_TYPE){
    ITMG_KARAOKE_TYPE_ORIGINAL = 0,
    ITMG_KARAOKE_TYPE_POP = 1,
    ITMG_KARAOKE_TYPE_ROCK = 2,
    ITMG_KARAOKE_TYPE_RB = 3,
    ITMG_KARAOKE_TYPE_DANCE = 4,
    ITMG_KARAOKE_TYPE_HEAVEN = 5,
    ITMG_KARAOKE_TYPE_TTS = 6,
} ;


@class ITMGRoom;
@class ITMGAudioCtrl;
@class ITMGAudioEffectCtrl;

#ifndef TMG_NO_PTT_SUPPORT
@class ITMGPTT;
#endif//TMG_NO_PTT_SUPPORT



@protocol ITMGDelegate <NSObject>

/*!
 @abstract      sdk事件通知
 @discussion    用于通知TMG事件
 @param         eventType            事件类型
 @param         data  事件数据，是一个字典，具体键值对参考接入文档
 */
- (void)OnEvent:(ITMG_MAIN_EVENT_TYPE)eventType data:(NSDictionary*)data;
@end



@interface ITMGContext : NSObject
/**
 *  房间状态通知delegate
 */
{
    id<ITMGDelegate> _TMGDelegate;
}
@property  (assign, nonatomic)  id<ITMGDelegate> TMGDelegate;

+(ITMGContext*) GetInstance;

/**
 *  设置app信息
 *
 *  @param sdkAppID 设置sdkAppID
 *  @param openID   设置当前用户的openID
 */
-(int)InitEngine:(NSString*)sdkAppID openID:(NSString*)openID;

-(void)Uninit;

-(void)Poll;

-(QAVResult)Pause;
-(QAVResult)Resume;

-(int)SetLogLevel:(ITMG_LOG_LEVEL)levelWrite :(ITMG_LOG_LEVEL)levelPrint;
-(int)SetLogPath:(NSString*)logDir;


/**
 *  获取sdk版本信息
 *
 *  @return 版本号
 */
-(NSString*)GetSDKVersion;

/**
 *  设置app的版本信息
 *
 *  @param appVersion app的版本
 */
-(void)SetAppVersion:(NSString*)appVersion;

/**
 *  当前是否在音视频房间
 *
 *  @return 在房间返回YES
 */
-(BOOL)IsRoomEntered;

/**
 *  进入音视频房间
 *
 *  @param roomID 房间标识
 *  @param roomType 房间类型 ITMG_ROOM_TYPE
 *  @param authBuffer 加密权限标志
 *
 */
-(void)EnterRoom:(NSString*)roomID roomType:(int)roomType authBuffer:(NSData*)authBuffer;


/**
 *  退出房间
 *  @return 当返回值为AV_OK时，异步执行退房并通过异步回调ExitRoomComplete返回。返回值为非AV_OK时表示执行失败，没有回调。
 */
-(int)ExitRoom;


/**
 *  设置最大混音路数，默认6路，如果需要修改请在SetAppInfo之前调用
 *
 */
-(QAVResult)SetRecvMixStreamCount:(int)count;

/**
 *  扩展参数设置
 *
 */
-(QAVResult)SetAdvanceParams:(NSString*)strKey value:(NSString*)strValue;

-(NSString *)GetAdvanceParams:(NSString*)strKey;

/**
 * 本方法只在观众状态下生效，默认是Ambient，可以支持设置为PlayBack。
 * 具体实现为 修改 kAudioSessionProperty_AudioCategory，具体特性 可参照 Apple官方文档
 */
-(QAVResult)SetDefaultAudienceAudioCategory:(ITMG_AUDIO_CATEGORY)audioCategory;


/**
 *  查询麦克风权限
 *
 */
-(ITMG_RECORD_PERMISSION)CheckMicPermission;

-(ITMGRoom*)GetRoom;

-(ITMGAudioCtrl*)GetAudioCtrl;

-(ITMGAudioEffectCtrl*)GetAudioEffectCtrl;

#ifndef TMG_NO_PTT_SUPPORT
-(ITMGPTT*)GetPTT;
#endif//TMG_NO_PTT_SUPPORT

-(int) SetTestEnv:(bool) isTestEnv;     //// warning : never call this API for any reason, it's only for internal use




@end



@interface ITMGRoom :NSObject

/**
 *  获取质量信息
 *
 *  @return 质量信息tips
 */
-(NSString*)GetQualityTips;

/**
 * 进入房间后，管理员可以调用此方法修改房间的音质类型，会影响房间内的所有已加入和未加入成员
 *  @param roomType 房间类型 ITMG_ROOM_TYPE
 */
-(void)ChangeRoomType:(int)roomType;

/**
 * 进入房间后，可以调用此方法获取当前房间的音质类型；
 * @return roomType 房间类型 ITMG_ROOM_TYPE
 */
-(int)GetRoomType;

@end

@interface ITMGAudioCtrl : NSObject

/**
 *  卡拉ok音效接口
 */
-(QAVResult)SetKaraokeType:(ITMG_KARAOKE_TYPE) type;

// 推荐使用此方法开关麦克风。EnalbeMic(value) = EnableAudioCaptureDevice(value) + EnableAudioSend(value)
-(QAVResult)EnableMic:(BOOL)enable;
// 只是快捷方式。[0 is off; 1 is on] GetMicState() = IsAudioSendEnabled() && IsAudioCaptureDeviceEnabled()
-(int) GetMicState;

// 推荐使用此方法开关扬声器。EnableSpeaker(value) = EnableAudioPlayDevice(value) + EnableAudioRecv(value)
-(QAVResult)EnableSpeaker:(BOOL)enable;
// 只是快捷方式。[0 is off; 1 is on] GetSpeakerState() = IsAudioRecvEnabled() && IsAudioPlayDeviceEnabled()
-(int) GetSpeakerState;

/**
 * 开启采集和播放设备。默认情况下，GME不会打开设备。
 * 注意：只能在进房后调用此API，退房会自动关闭设备
 * 注意：在移动端，打开采集设备通常会伴随权限申请，音量类型调整等操作。
 *
 * 调用场景举例：
 * 1、当用户界面点击打开/关闭麦克风/扬声器按钮时，建议：
 *     Option 1: 对于大部分的游戏类App，总是应该同时调用EnableAudioCaptureDevice/EnableAudioSend 和 EnableAudioPlayDevice/EnableAudioRecv
 *     Option 2: 只有特定的社交类App，需要在进房后一次性调用EnableAudioCapture/PlayDevice(true)，后续只使用EnableAudioSend/Recv进行音频流控制
 * 2、如目的是互斥（释放录音权限给其他模块使用），建议使用PauseAudio/ResumeAudio。
 */
-(QAVResult)EnableAudioCaptureDevice:(BOOL)enabled;
-(QAVResult)EnableAudioPlayDevice:(BOOL)enabled;

-(BOOL)IsAudioCaptureDeviceEnabled;
-(BOOL)IsAudioPlayDeviceEnabled;

/**
 * 打开/关闭音频上行，如果采集设备已经打开，那么会发送采集到的音频数据。如果采集设备没有打开，那么仍旧无声。参见EnableAudioCaptureDevice
 * ExitRoom will automatically call EnableAudioSend(false)
 */
-(QAVResult)EnableAudioSend:(BOOL)enable;
/**
 * 打开/关闭音频下行，如果播放设备已经打开，那么会接收并播放房间里其他人的音频数据。如果播放设备没有打开，那么仍旧无声。参见EnableAudioPlayDevice
 * ExitRoom will automatically call EnableAudioRecv(false)
 */
-(QAVResult)EnableAudioRecv:(BOOL)enable;

-(BOOL)IsAudioSendEnabled;
-(BOOL)IsAudioRecvEnabled;


/**
 *  获取麦克风实时音量，可以用来绘制波形图
 *
 *  @return 音量值，取值范围[0,100]
 */
-(int)GetMicLevel;


/**
 *  设置麦克风软件音量
 *
 *  @param volume 音量值，取值范围[0,200]
 */
-(QAVResult)SetMicVolume:(int) volume;

/**
 *  获取麦克风软件音量，默认100
 */
-(int) GetMicVolume;

/**
 *  获取扬声器实时音量，可以用来绘制波形图
 *
 *  @return 音量值，取值范围[0,100]
 */
-(int)GetSpeakerLevel;

/**
 *  设置扬声器软件音量
 *
 *  @param volume 音量值，取值范围[0,200]
 */
-(QAVResult)SetSpeakerVolume:(int)volume;
/**
 *  获取扬声器软件音量
 *
 *  @return 音量值，取值范围[0,200]
 */
-(int)GetSpeakerVolume;

/**
 *  打开或关闭声音回环，打开后演示器会播放自己的声音
 *
 *  @param enable 是否打开
 *
 *  @return 参考QAVResult
 */
-(QAVResult)EnableLoopBack:(BOOL)enable;

/**
 * 音频黑名单
 */
-(QAVResult)AddAudioBlackList:(NSString*)openID;

-(QAVResult)RemoveAudioBlackList:(NSString*)openID;

/**
 *  跟踪房间成员音量变化
 *
 *  @param interval 通知间隔
 *
 *  @return 参考QAVResult
 */
-(QAVResult)TrackingVolume:(float)interval;

/**
 *  停止跟踪房间成员音量变化
 *
*  @return 参考QAVResult
 */
-(QAVResult)StopTrackingVolume;
@end

@interface ITMGAudioEffectCtrl:NSObject
/**
 *  开始伴奏
 *
 *  @param filePath  伴奏文件
 *  @param loopBack  是否其他人也可以听到，false为其他人听不到，true是其他人也能听到。
 *  @param loopCount 循环的次数，-1为无限循环。
*  @return 参考QAVResult
 */
-(QAVResult)StartAccompany:(NSString*)filePath loopBack:(BOOL)loopBack loopCount:(int)loopCount;
/**
 *  停止伴奏
 *
 *  @param duckerTime 填0
 *
 *  @return 参考QAVResult
 */
-(QAVResult)StopAccompany:(int)duckerTime;

/**
 *  伴奏是否结束
 *
 *  @return true表示已经结束
 */
-(bool)IsAccompanyPlayEnd;
/**
 *  暂停伴奏
 *
 *  @return 参考QAVResult
 */
-(QAVResult)PauseAccompany;
/**
 *  恢复伴奏
 *
 *  @return 参考QAVResult
 */
-(QAVResult)ResumeAccompany;

/**
 *  设置是否自己可以听到伴奏
 *
 *  @param enable false为自己听不到，true是自己能听到
 *
 *  @return 参考QAVResult
 */
-(QAVResult)EnableAccompanyPlay:(BOOL)enable;

/**
 *  设置是否其他人也可以听到伴奏。
 *
 *  @param enable alse为其他人听不到，true是其他人也能听到
 *
 *  @return 参考QAVResult
 */
-(QAVResult)EnableAccompanyLoopBack:(BOOL)enable;

/**
 *  设置伴奏音量，为线性音量
 *
 *  @param vol 默认值为100，大于100增益，小于100减益
 *
 *  @return 参考QAVResult
 */
-(QAVResult)SetAccompanyVolume:(int)vol;
/**
 *  获取伴奏音量
 *
 *  @return 音量值
 */
-(int)GetAccompanyVolume;


/**
 *  获取总时长
 *
 *  @return 时长
 */
-(int)GetAccompanyFileTotalTimeByMs;
/**
 *  获取当前播放进度
 *
 *  @return 当前进度
 */
-(int)GetAccompanyFileCurrentPlayedTimeByMs;

/**
 *  设置当前播放位置
 *
 *  @param time 播放位置,毫秒为单位
 *
 *  @return  参考QAVResult
 */
-(QAVResult)SetAccompanyFileCurrentPlayedTimeByMs:(uint) time;


/**
 *  获取音效音量
 *
 *  @return 音效音量，默认值为100，大于100增益，小于100减益
 */
-(int)GetEffectsVolume;
/**
 *  设置音效音量
 *
 *  @param volume 音量值，默认值为100，大于100增益，小于100减益
 *
 *  @return 参考QAVResult
 */
-(QAVResult)SetEffectsVolume:(int)volume;

/**
 *  播放音效文件
 *
 *  @param soundId  设置当前文件的soundId，后续通过soundId来控制该音效
 *  @param filePath 音效文件路径
 *  @param loop     是否循环播放
 *
 *  @return 参考QAVResult
 */
-(QAVResult)PlayEffect:(int)soundId filePath:(NSString*)filePath loop:(BOOL)loop;
/**
 *  暂停音效播放
 *
 *  @param soundId 音效soundId
 *
 *  @return 参考QAVResult
 */
-(QAVResult)PauseEffect:(int)soundId;

/**
 *  暂停所有音效播放
 *
 *  @return 参考QAVResult
 */
-(QAVResult)PauseAllEffects;

/**
 *  恢复单个音效播放
 *
 *  @param soundId 音效soundId
 *
 *  @return 参考QAVResult
 */
-(QAVResult)ResumeEffect:(int)soundId;
/**
 *  恢复所有音效播放
 *
 *  @return 参考QAVResult
 */
-(QAVResult)ResumeAllEffects;

/**
 *  停止某个音效播放
 *
 *  @param soundId  音效soundId
 *
 *  @return 参考QAVResult
 */
-(QAVResult)StopEffect:(int)soundId;
/**
 *  停止所有音效播放
 *
 *  @return 参考QAVResult
 */
-(QAVResult)StopAllEffects;

-(QAVResult)SetVoiceType:(ITMG_VOICE_TYPE) type;

@end

#ifndef TMG_NO_PTT_SUPPORT

@interface ITMGPTT : NSObject

-(QAVResult)SetMaxMessageLength:(int)msTime;

-(void)StartRecordingWithStreamingRecognition:(NSString *)filePath;
-(void)StartRecordingWithStreamingRecognition:(NSString *)filePath language:(NSString*)speechLanguage;
-(void)StartRecordingWithStreamingRecognition:(NSString *)filePath language:(NSString*)speechLanguage translatelanguage:(NSString*)translatelanguage;

-(void)StartRecording:(NSString*)filePath;
-(QAVResult)StopRecording;
-(QAVResult)CancelRecording;

-(int)PauseRecording;
-(int)ResumeRecording;

-(int) GetMicLevel;
-(QAVResult)SetMicVolume:(int) volume;
-(int) GetMicVolume;

-(int) GetSpeakerLevel;
-(QAVResult)SetSpeakerVolume:(int)volume;
-(int)GetSpeakerVolume;

-(QAVResult)ApplyPTTAuthbuffer:(NSData *)authBuffer;

-(void)UploadRecordedFile:(NSString*)filePath;

-(void)DownloadRecordedFile:(NSString*)fileId filePath:(NSString*)filePath;

-(void)PlayRecordedFile:(NSString*)filePath;

-(int)StopPlayFile;


-(void)SpeechToText:(NSString*)fileID;
-(void)SpeechToText:(NSString*)fileID language:(NSString*)speechLanguage;
-(void)SpeechToText:(NSString*)fileID language:(NSString*)speechLanguage  translateLanguage:(NSString*)translateLanguage;


-(int)GetFileSize:(NSString*)filePath;

-(int)GetVoiceFileDuration:(NSString*)filePath;
@end
#endif//TMG_NO_PTT_SUPPORT
