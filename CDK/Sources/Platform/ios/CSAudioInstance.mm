//
//  CSAudioInstance.m
//  CDK
//
//  Created by Kim Chan on 2017. 7. 5..
//  Copyright © 2017년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAudioInstance.h"

#import "CSAVAudioInstance.h"
#import "CSOggAudioInstance.h"

@implementation CSAudioInstance

@synthesize delegate = _delegate;
@synthesize path = _path;
@synthesize control = _control;
@synthesize volume = _volume;
@synthesize subVolume = _subVolume;
@synthesize priority = _priority;
@synthesize category = _category;
@synthesize single = _single;
@synthesize userPaused = _userPaused;

+(CSAudioInstance*)createWithPath:(NSString*)path {
    if ([path hasSuffix:@".ogg"]) {
        return [[CSOggAudioInstance alloc] initWithPath:path];
    }
    else {
        return [[CSAVAudioInstance alloc] initWithPath:path];
    }
}

+(CSAudioInstance*)instanceWithPath:(NSString*)path {
    return [[CSAudioInstance createWithPath:path] autorelease];
}

-(id)initWithPath:(NSString*)path {
    if (self = [super init]) {
        _path = [path retain];
        _volume = 1.0f;
        _subVolume = 1.0f;
    }
    return self;
}

-(void)dealloc {
    [_path release];
    
    [super dealloc];
}

-(BOOL)isLooping {
    [self doesNotRecognizeSelector:_cmd];
    return NO;
}

-(BOOL)isPlaying {
    [self doesNotRecognizeSelector:_cmd];
    return NO;
}

-(void)play:(BOOL)loop volume:(float)volume {
    [self doesNotRecognizeSelector:_cmd];
}

-(void)stop {
    [self doesNotRecognizeSelector:_cmd];
}

-(void)pause {
    [self doesNotRecognizeSelector:_cmd];
}

-(void)resume {
    [self doesNotRecognizeSelector:_cmd];
}

-(void)applyVolume {
    [self doesNotRecognizeSelector:_cmd];
}

-(void)setVolume:(float)volume {
    _volume = volume;
    
    [self applyVolume];
}

@end

#endif
