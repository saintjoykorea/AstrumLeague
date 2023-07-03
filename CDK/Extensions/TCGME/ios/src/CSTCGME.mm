//
//  CSTCGME.mm
//  CDK
//
//  Created by Kim Chan on 2019. 7. 3..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "GMESDK/TMGEngine.h"
#import "GMESDK/QAVAuthBuffer.h"

#import "CSTCGME.h"

#import "CSLog.h"

@interface CSTCGMEImpl : NSObject<ITMGDelegate> {
    NSString* _appId;
    NSString* _appKey;
    NSString* _userId;
    NSMutableSet* _talkings;
}

@property CSTCGMEDelegate* delegate;
@property (readonly) BOOL roomEntered;
@property BOOL micEnabled;
@property BOOL speakerEnabled;
@property float micVolume;
@property float speakerVolume;

-(id)init;
-(void)dealloc;
-(void)connect:(int64)userId;
-(void)disconnect;
-(void)enterRoom:(NSString*)roomId soundQuality:(CSTCGMESoundQuality)quality;
-(void)exitRoom;
-(void)pause;
-(void)resume;
-(void)poll;

@end


@implementation CSTCGMEImpl

@synthesize delegate;

-(id)init {
    if (self = [super init]) {
        _appId = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"TCGMEAppID"] retain];
        _appKey = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"TCGMEAppSecret"] retain];
        _talkings = [[NSMutableSet alloc] init];
        
        [[ITMGContext GetInstance] SetLogLevel:ITMG_LOG_LEVEL_NONE :ITMG_LOG_LEVEL_NONE];
        
        CSLog("CSTCGME init:%s, %s", _appId.UTF8String, _appKey.UTF8String);
    }
    return self;
}

-(void)dealloc {
    [_appId release];
    [_appKey release];
    [_userId release];
    [_talkings release];
    
    [super dealloc];
}

-(void)connect:(int64)userId {
    ITMGContext* context = [ITMGContext GetInstance];
    if (_userId) {
        if (_userId.longLongValue == userId) {
            return;
        }
        [context Uninit];
        [_userId release];
        _userId = nil;
    }
    _userId = [[NSString alloc] initWithFormat:@"%" PRId64, userId];
    context.TMGDelegate = self;
    [context InitEngine:_appId openID:_userId];
    
    [self poll];
}

-(void)disconnect {
    if (_userId) {
        [_talkings removeAllObjects];
        
        ITMGContext* context = [ITMGContext GetInstance];
        context.TMGDelegate = nil;
        [context Uninit];
        
        [_userId release];
        _userId = nil;
    }
}

-(void)enterRoom:(NSString*)roomId soundQuality:(CSTCGMESoundQuality)quality {
    if (_userId) {
        NSData* authBuffer = [QAVAuthBuffer GenAuthBuffer:_appId.intValue roomID:roomId openID:_userId key:_appKey];
        
        [[ITMGContext GetInstance] EnterRoom:roomId roomType:quality authBuffer:authBuffer];
    }
}

-(void)exitRoom {
    if (_userId) {
        [[ITMGContext GetInstance] ExitRoom];
    }
}

-(BOOL)roomEntered {
    return [[ITMGContext GetInstance] IsRoomEntered];
}

-(void)setMicEnabled:(BOOL)micEnabled {
    [[[ITMGContext GetInstance] GetAudioCtrl] EnableMic:micEnabled];
}

-(BOOL)micEnabled {
    return [[[ITMGContext GetInstance] GetAudioCtrl] GetMicState] != 0;
}

-(void)setSpeakerEnabled:(BOOL)speakerEnabled {
    [[[ITMGContext GetInstance] GetAudioCtrl] EnableSpeaker:speakerEnabled];
}

-(BOOL)speakerEnabled {
    return [[[ITMGContext GetInstance] GetAudioCtrl] GetSpeakerState] != 0;
}

-(void)setMicVolume:(float)micVolume {
    [[[ITMGContext GetInstance] GetAudioCtrl] SetMicVolume:micVolume * 200];
}

-(float)micVolume {
    return [[[ITMGContext GetInstance] GetAudioCtrl] GetMicVolume] / 200.0f;
}

-(void)setSpeakerVolume:(float)speakerVolume {
    [[[ITMGContext GetInstance] GetAudioCtrl] SetSpeakerVolume:speakerVolume * 200];
}

-(float)speakerVolume {
    return [[[ITMGContext GetInstance] GetAudioCtrl] GetSpeakerVolume] / 200.0f;
}

-(void)pause {
    [[ITMGContext GetInstance] Pause];
}

-(void)resume {
    [[ITMGContext GetInstance] Resume];
}

-(void)poll {
    if (_userId) {
        [[ITMGContext GetInstance] Poll];
        
        [self performSelector:@selector(poll) withObject:nil afterDelay:0.2];
    }
}

-(void)OnEvent:(ITMG_MAIN_EVENT_TYPE)eventType data:(NSDictionary*)data {
    CSLog("onEvent:%d", eventType);
    
    if (delegate) {
        switch (eventType) {
            case ITMG_MAIN_EVENT_TYPE_ENTER_ROOM:
                {
                    int errorCode = [[data objectForKey:@"result"] intValue];
                    
                    if (errorCode == QAV_OK) {
                        [[[ITMGContext GetInstance] GetAudioCtrl] TrackingVolume:0.5f];
                        
                        delegate->onTCGMEEnterRoom(true);
                    }
                    else {
                        delegate->onTCGMEEnterRoom(false);
                    }
                }
                break;
            case ITMG_MAIN_EVENT_TYPE_EXIT_ROOM:
                [_talkings removeAllObjects];
                delegate->onTCGMEExitRoom(true);
                break;
            case ITMG_MAIN_EVENT_TYPE_ROOM_DISCONNECT:
                [_talkings removeAllObjects];
                delegate->onTCGMEExitRoom(false);
                break;
            case ITMG_MAIN_EVNET_TYPE_USER_UPDATE:
                {
                    ITMG_EVENT_ID_USER eventId = (ITMG_EVENT_ID_USER)[[data objectForKey:@"event_id"] unsignedIntegerValue];
                    NSArray* userIds = [data objectForKey:@"user_list"];

                    switch (eventId) {
                        case ITMG_EVENT_ID_USER_ENTER:
                            for (NSString* userId in userIds) {
                                delegate->onTCGMENotifyEnterRoom(userId.longLongValue);
                            }
                            break;
                        case ITMG_EVENT_ID_USER_EXIT:
                            for (NSString* userId in userIds) {
                                delegate->onTCGMENotifyExitRoom(userId.longLongValue);
                            }
                            break;
                            /*
                        case ITMG_EVENT_ID_USER_HAS_AUDIO:
                            for (NSString* userId in userIds) {
                                delegate->onTCGMETalk(userId.longLongValue, true);
                            }
                            break;
                        case ITMG_EVENT_ID_USER_NO_AUDIO:
                            for (NSString* userId in userIds) {
                                delegate->onTCGMETalk(userId.longLongValue, false);
                            }
                            break;
                             */
                            //ITMG_EVENT_ID_USER_HAS_AUDIO, ITMG_EVENT_ID_USER_NO_AUDIO 잘 동작하지 않음
                    }
                }
                break;
            case ITMG_MAIN_EVNET_TYPE_USER_VOLUMES:
                for (NSString* userId in data.allKeys) {
                    int volume = [[data objectForKey:userId] intValue];
                    
                    if (volume) {
                        if (![_talkings containsObject:userId]) {
                            [_talkings addObject:userId];
                            
                            delegate->onTCGMETalk(userId.longLongValue, true);
                        }
                    }
                    else {
                        if ([_talkings containsObject:userId]) {
                            [_talkings removeObject:userId];
                            
                            delegate->onTCGMETalk(userId.longLongValue, false);
                        }
                    }
                }
                break;
            case ITMG_MAIN_EVENT_TYPE_RECONNECT_START:
                delegate->onTCGMEReconnect(false);
                break;
            case ITMG_MAIN_EVENT_TYPE_RECONNECT_SUCCESS:
                delegate->onTCGMEReconnect(true);
                break;
            
        }
    }
}

@end

static CSTCGMEImpl* _impl = nil;

bool CSTCGME::isEnabled() {
    return true;
}

void CSTCGME::initialize() {
    if (!_impl) {
        _impl = [[CSTCGMEImpl alloc] init];
    }
}

void CSTCGME::finalize() {
    [_impl disconnect];
    [_impl release];
    _impl = nil;
}

void CSTCGME::connect(int64 userId) {
    [_impl connect:userId];
}

void CSTCGME::disconnect() {
    [_impl release];
    _impl = nil;
}

void CSTCGME::setDelegate(CSTCGMEDelegate* delegate) {
    _impl.delegate = delegate;
}

void CSTCGME::enterRoom(const char* roomId, CSTCGMESoundQuality quality) {
    [_impl enterRoom:[NSString stringWithUTF8String:roomId] soundQuality:quality];
}

bool CSTCGME::isRoomEntered() {
    return _impl.roomEntered;
}

void CSTCGME::exitRoom() {
    [_impl exitRoom];
}

void CSTCGME::setMicEnabled(bool enabled) {
    _impl.micEnabled = enabled;
}

bool CSTCGME::isMicEnabled() {
    return _impl.micEnabled;
}

void CSTCGME::setSpeakerEnabled(bool enabled) {
    _impl.speakerEnabled = enabled;
}

bool CSTCGME::isSpeakerEnabled() {
    return _impl.speakerEnabled;
}

void CSTCGME::setMicVolume(float volume) {
    _impl.micVolume = volume;
}

float CSTCGME::micVolume() {
    return _impl.micVolume;
}

void CSTCGME::setSpeakerVolume(float volume) {
    _impl.speakerVolume = volume;
}

float CSTCGME::speakerVolume() {
    return _impl.speakerVolume;
}

void CSTCGME::pause() {
    [_impl pause];
}

void CSTCGME::resume() {
    [_impl resume];
}

#endif

