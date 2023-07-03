//
//  CSAudio.mm
//  CDK
//
//  Created by Kim Chan on 2017. 7. 4..
//  Copyright © 2017년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAudio.h"

#import "CSAudioInstance.h"

#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioServices.h>

#define MaxChannels     12

@interface CSAudioImpl : NSObject<CSAudioInstanceDelegate> {
    NSMutableArray* _instances;
    NSMutableArray* _mutes;
    NSMutableDictionary* _volumes;
    NSThread* _thread;
    BOOL _alive;
    int _singleCounter;
}

@property (nonatomic, readonly) int singleCounter;

-(id)init;

-(void)run;
-(void)start;
-(void)finish;
-(void)play:(int)control path:(NSString*)path volume:(float)volume loop:(BOOL)loop priority:(int)priority category:(int)category single:(BOOL)single;
-(void)stop:(int)control;
-(void)pause:(int)control;
-(void)resume:(int)control;
-(void)setVolume:(int)volumeCategory volume:(float)volume;
-(void)autoPause;
-(void)autoResume;

-(BOOL)ready:(int)priority;
-(void)playImpl:(NSArray*)params;
-(void)stopImpl:(NSNumber*)control;
-(void)pauseImpl:(NSNumber*)control;
-(void)resumeImpl:(NSNumber*)control;
-(void)setVolumeImpl:(NSArray*)params;
-(void)autoPauseImpl;
-(void)autoResumeImpl;
-(void)finishImpl;

@end

@implementation CSAudioImpl

@synthesize singleCounter = _singleCounter;

-(id)init {
    if (self = [super init]) {
        _instances = [[NSMutableArray alloc] initWithCapacity:MaxChannels];
        _mutes = [[NSMutableArray alloc] init];
        _volumes = [[NSMutableDictionary alloc] init];
        _thread = [[NSThread alloc] initWithTarget:self selector:@selector(run) object:nil];
    }
    return self;
}

-(void)dealloc {
    [_instances release];
    [_mutes release];
    [_volumes release];
    [_thread release];
    
    [super dealloc];
}

-(void)run {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    NSRunLoop* runLoop = [NSRunLoop currentRunLoop];
    [runLoop addPort:[NSMachPort port] forMode:NSDefaultRunLoopMode];
    while (_alive && [runLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]]) {
        [pool release];
        pool = [[NSAutoreleasePool alloc] init];
    }
    [pool release];
}

-(void)start {
    _alive = YES;
    [_thread start];
}

-(void)finish {
    [self performSelector:@selector(finishImpl) onThread:_thread withObject:nil waitUntilDone:NO];
}

-(void)play:(int)control path:(NSString*)path volume:(float)volume loop:(BOOL)loop priority:(int)priority category:(int)category single:(BOOL)single {
    NSArray* params = [NSArray arrayWithObjects:
                       [NSNumber numberWithInt:control],
                       path,
                       [NSNumber numberWithFloat:volume],
                       [NSNumber numberWithBool:loop],
                       [NSNumber numberWithInt:priority],
                       [NSNumber numberWithInt:category],
					   [NSNumber numberWithBool:single],
                       nil];
    [self performSelector:@selector(playImpl:) onThread:_thread withObject:params waitUntilDone:NO];
}

-(void)stop:(int)control {
    [self performSelector:@selector(stopImpl:) onThread:_thread withObject:[NSNumber numberWithInt:control] waitUntilDone:NO];
}

-(void)pause:(int)control {
    [self performSelector:@selector(pauseImpl:) onThread:_thread withObject:[NSNumber numberWithInt:control] waitUntilDone:NO];
}

-(void)resume:(int)control {
    [self performSelector:@selector(resumeImpl:) onThread:_thread withObject:[NSNumber numberWithInt:control] waitUntilDone:NO];
}

-(void)setVolume:(int)category volume:(float)volume {
    NSArray* params = [NSArray arrayWithObjects:
                       [NSNumber numberWithInt:category],
                       [NSNumber numberWithFloat:volume],
                       nil];
    
    [self performSelector:@selector(setVolumeImpl:) onThread:_thread withObject:params waitUntilDone:NO];
}

-(void)autoPause {
    [self performSelector:@selector(autoPauseImpl) onThread:_thread withObject:nil waitUntilDone:NO];
}

-(void)autoResume {
    [self performSelector:@selector(autoResumeImpl) onThread:_thread withObject:nil waitUntilDone:NO];
}

-(BOOL)ready:(int)priority {
    if (_instances.count < MaxChannels) {
        return YES;
    }
    int index = -1;
    
    for (int i = 0; i < _instances.count; i++) {
        CSAudioInstance* instance = [_instances objectAtIndex:i];
        
        if (!instance.isPlaying && (index == -1 || instance.priority < priority)) {
            priority = instance.priority;
            index = i;
        }
    }
    if (index == -1) {
        for (int i = 0; i < _instances.count; i++) {
            CSAudioInstance* instance = [_instances objectAtIndex:i];
            
            if (instance.priority < priority) {
                priority = instance.priority;
                index = i;
            }
        }
    }
    if (index != -1) {
        [_instances removeObjectAtIndex:index];
        return YES;
    }
    return NO;
}

-(void)playImpl:(NSArray*)nsparams {
    int control = [[nsparams objectAtIndex:0] intValue];
    NSString* path = [nsparams objectAtIndex:1];
    float subVolume = [[nsparams objectAtIndex:2] floatValue];
    BOOL loop = [[nsparams objectAtIndex:3] boolValue];
    int priority = [[nsparams objectAtIndex:4] intValue];
    NSNumber* nscategory = [nsparams objectAtIndex:5];
    NSNumber* nsvolume = [_volumes objectForKey:nscategory];
    float mainVolume = nsvolume ? nsvolume.floatValue : 1.0f;
	BOOL single = [[nsparams objectAtIndex:6] boolValue];
    
	if (single) {
		for (CSAudioInstance* otherInstance in _instances) {
			if (otherInstance.single && otherInstance.isPlaying) {
				return;
			}
		}
		_singleCounter++;
	}
    if (!subVolume) {
        return;
    }
    if (!mainVolume) {
        if (loop) {
            [_mutes addObject:nsparams];
        }
        return;
    }

    CSAudioInstance* instance = nil;
    
    for (CSAudioInstance* otherInstance in _instances) {
        if ([otherInstance.path isEqualToString:path] && !otherInstance.isPlaying) {
            instance = otherInstance;
            break;
        }
    }
    if (!instance) {
        if ([self ready:priority]){
            instance = [CSAudioInstance instanceWithPath:path];
            if (!instance) {
                return;
            }
            instance.delegate = self;
            [_instances addObject:instance];
        }
        else {
            return;
        }
    }
    instance.control = control;
    instance.volume = mainVolume;
    instance.priority = priority;
    instance.category = nscategory.intValue;
	instance.single = single;
    [instance play:loop volume:subVolume];
}

-(void)stopImpl:(NSNumber*)nscontrol {
    int control = nscontrol.intValue;
    for (CSAudioInstance* instance in _instances) {
        if (instance.control == control) {
            [instance stop];
        }
    }
    int i = 0;
    while (i < [_mutes count]) {
        NSArray* muteParams = [_mutes objectAtIndex:i];
        
        int muteControl = [[muteParams objectAtIndex:0] intValue];
        
        if (muteControl == control) {
            [_mutes removeObjectAtIndex:i];
        }
        else {
            i++;
        }
    }
}

-(void)pauseImpl:(NSNumber*)nscontrol {
    int control = nscontrol.intValue;
    for (CSAudioInstance* instance in _instances) {
        if (instance.control == control) {
            instance.userPaused = YES;
            [instance pause];
        }
    }
}

-(void)resumeImpl:(NSNumber*)nscontrol {
    int control = nscontrol.intValue;
    for (CSAudioInstance* instance in _instances) {
        if (instance.control == control) {
            instance.userPaused = NO;
            [instance resume];
        }
    }
}

-(void)setVolumeImpl:(NSArray*)params {
    NSNumber* nscategory = [params objectAtIndex:0];
    int category = nscategory.intValue;
    float volume = [[params objectAtIndex:1] floatValue];
    
    if (volume == 1.0f) {
        [_volumes removeObjectForKey:nscategory];
    }
    else {
        [_volumes setObject:[NSNumber numberWithFloat:volume] forKey:nscategory];
    }
    
    for (CSAudioInstance* instance in _instances) {
        if (instance.category == category) {
            instance.volume = volume;
            
            if (!volume && instance.isPlaying) {
                if (instance.isLooping) {
                    NSArray* muteParams = [NSArray arrayWithObjects:
                                           [NSNumber numberWithInt:instance.control],
                                           instance.path,
                                           [NSNumber numberWithFloat:instance.subVolume],
                                           [NSNumber numberWithBool:YES],
                                           [NSNumber numberWithInt:instance.priority],
                                           nscategory,
                                           [NSNumber numberWithBool:instance.single],
                                           nil];
                    
                    [_mutes addObject:muteParams];
                }
                [instance stop];
            }
        }
    }
    if (volume) {
        int i = 0;
        while (i < [_mutes count]) {
            NSArray* muteParams = [_mutes objectAtIndex:i];
            
            int muteCategory = [[muteParams objectAtIndex:5] intValue];
            
            if (muteCategory == category) {
                [self playImpl:muteParams];
                
                [_mutes removeObjectAtIndex:i];
            }
            else {
                i++;
            }
        }
    }
}

-(void)autoPauseImpl {
    for (CSAudioInstance* instance in _instances) {
        [instance pause];
    }
}

-(void)autoResumeImpl {
    for (CSAudioInstance* instance in _instances) {
        if (!instance.isUserPaused) {
            [instance resume];
        }
    }
}

-(void)finishImpl {
    for (CSAudioInstance* instance in _instances) {
        [instance stop];
    }
    _alive = NO;
}

-(void)audioErrorImpl:(NSValue*)nsinstance {
    CSAudioInstance* instance = (CSAudioInstance*)nsinstance.pointerValue;
    
    for (int i = 0; i < _instances.count; i++) {
        CSAudioInstance* otherInstance = [_instances objectAtIndex:i];
        
        if (instance == otherInstance) {
            [_instances removeObjectAtIndex:i];
            break;
        }
    }
}

-(void)audioError:(CSAudioInstance*)instance {
    [self performSelector:@selector(audioErrorImpl:) onThread:_thread withObject:[NSValue valueWithPointer:instance] waitUntilDone:NO];
}

-(void)audioFinish:(CSAudioInstance*)instance {

}

@end

static CSAudioImpl* __sharedImpl = nil;

void CSAudio::initialize() {
    if (!__sharedImpl) {
        restoreCategory();
        
        __sharedImpl = [[CSAudioImpl alloc] init];
        [__sharedImpl start];
    }
}

void CSAudio::finalize() {
    if (__sharedImpl) {
        [__sharedImpl finish];
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSAudio::autoPause() {
    [__sharedImpl autoPause];
}

void CSAudio::autoResume() {
    [__sharedImpl autoResume];
}

void CSAudio::restoreCategory() {
    AVAudioSession* audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryAmbient mode:AVAudioSessionModeDefault options:AVAudioSessionCategoryOptionMixWithOthers error:nil];
    [audioSession setActive:YES error:nil];
}

void CSAudio::play(int control, const char* path, float volume, bool loop, int priority, int category, bool fx, bool single) {
    [__sharedImpl play:control path:[NSString stringWithUTF8String:path] volume:volume loop:loop priority:priority category:category single:single];
}

void CSAudio::stop(int control) {
    [__sharedImpl stop:control];
}

void CSAudio::pause(int control) {
    [__sharedImpl pause:control];
}

void CSAudio::resume(int control) {
    [__sharedImpl resume:control];
}

void CSAudio::setVolume(int category, float volume) {
    [__sharedImpl setVolume:category volume:volume];
}

int CSAudio::singleCounter() {
    return __sharedImpl.singleCounter;
}

void CSAudio::vibrate(float time) {
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

#endif
