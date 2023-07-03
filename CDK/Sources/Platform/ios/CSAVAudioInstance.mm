//
//  CSAVAudioInstance.mm
//  CDK
//
//  Created by Kim Chan on 2016. 11. 22..
//  Copyright © 2016년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAVAudioInstance.h"

#import <Foundation/Foundation.h>

enum AVAudioState {
    AVAudioStateStopped,
    AVAudioStatePlaying,
    AVAudioStatePaused,
    AVAudioStateError
};

@interface CSAVAudioInstance () {
    AVAudioState _state;
}
@end

@implementation CSAVAudioInstance

-(id)initWithPath:(NSString*)path {
    if (self = [super initWithPath:path]) {
        NSURL* url = [NSURL fileURLWithPath:path];
        
        NSError* error;
        _component = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
        
        if (!error && [_component prepareToPlay]) {
            [_component setDelegate:self];
        }
        else {
            [self release];
            self = nil;
        }
    }
    return self;
}

-(void)dealloc {
    [_component setDelegate:nil];
    [_component stop];
    [_component release];
    
    [super dealloc];
}

-(void)audioPlayerDidFinishPlaying:(AVAudioPlayer*)player successfully:(BOOL)flag {
    _state = AVAudioStateStopped;
    [_delegate audioFinish:self];
}

-(void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer*)player error:(NSError*)error {
    _state = AVAudioStateError;
    [_delegate audioError:self];
}

-(void)applyVolume {
    [_component setVolume:_volume * _subVolume];
}

-(BOOL)isLooping {
    return _component.numberOfLoops == -1;
}

-(BOOL)isPlaying {
    return _state == AVAudioStatePlaying;
}

-(void)play:(BOOL)loop volume:(float)volume {
    if (_state == AVAudioStateStopped) {
        _state = AVAudioStatePlaying;

        _subVolume = volume;
        [self applyVolume];
        
        [_component setNumberOfLoops:loop ? -1 : 0];
        [_component setCurrentTime:0.0];
        [_component play];
    }
}

-(void)stop {
    if (_state == AVAudioStatePlaying) {
        _state = AVAudioStateStopped;

        [_component stop];
    }
}

-(void)pause {
    if (_state == AVAudioStatePlaying) {
        _state = AVAudioStatePaused;
        
        [_component pause];
    }
}

-(void)resume {
    if (_state == AVAudioStatePaused) {
        _state = AVAudioStatePlaying;
        
        [_component play];
    }
}

@end

#endif
