//
//  CSOggAudioInstance.mm
//  CDK
//
//  Created by Kim Chan on 2016. 11. 22..
//  Copyright © 2016년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSOggAudioInstance.h"

#import <AudioToolbox/AudioToolbox.h>
#import <Vorbis/vorbisfile.h>

#define AudioQueueBufferCount    3
#define AudioQueueBufferSize     (128 * 1024)

enum OggAudioState {
    OggAudioStateStopped,
    OggAudioStatePlaying,
    OggAudioStatePaused,
    OggAudioStateStopping,
    OggAudioStateError
};

@interface CSOggAudioInstance () {
    FILE* _file;
    OggVorbis_File _oggVorbisFile;
    AudioQueueRef _queue;
    AudioQueueBufferRef _buffers[AudioQueueBufferCount];
    OggAudioState _state;
    BOOL _loop;
}

-(BOOL)readBuffer:(AudioQueueBufferRef)buffer;
-(void)enqueueBuffer:(AudioQueueBufferRef)buffer;
-(void)stoppedListener;

@end

@implementation CSOggAudioInstance

static void OutputCallback(void* userData, AudioQueueRef queue, AudioQueueBufferRef buffer) {
    CSOggAudioInstance* handle = (__bridge CSOggAudioInstance*)userData;
    
    [handle enqueueBuffer:buffer];
}

static void PropertyListener(void* userData, AudioQueueRef queue, AudioQueuePropertyID propertyId) {
    if (propertyId == kAudioQueueProperty_IsRunning) {
        UInt32 running = 0;
        UInt32 ioSize = sizeof(running);
        AudioQueueGetProperty(queue, kAudioQueueProperty_IsRunning, &running, &ioSize);
        
        if (!running) {
            CSOggAudioInstance* handle = (__bridge CSOggAudioInstance*)userData;
            
            [handle stoppedListener];
        }
    }
}

-(id)initWithPath:(NSString*)path {
    if (self = [super initWithPath:path]) {
        _file = fopen(path.UTF8String, "r");
        
        if (!_file) {
            NSLog(@"file open fail:%@", path);
            [self release];
            return nil;
        }
        int rtn = ov_open_callbacks(_file, &_oggVorbisFile, NULL, 0, OV_CALLBACKS_NOCLOSE);
        if (rtn < 0) {
            NSLog(@"ov_open_callbacks fail:%d", rtn);
            [self release];
            return nil;
        }
        vorbis_info* info = ov_info(&_oggVorbisFile, -1);
        
        AudioStreamBasicDescription dataFormat;
        
        FillOutASBDForLPCM(dataFormat,
                           (Float64)info->rate,
                           (UInt32)info->channels,
                           16,
                           16,
                           false,
                           false);
        
        OSStatus status = AudioQueueNewOutput(&dataFormat, OutputCallback,
                                              (__bridge void*)self,
                                              CFRunLoopGetCurrent(),
                                              kCFRunLoopCommonModes,
                                              0,
                                              &_queue);
        
        if (status != noErr) {
            NSLog(@"AudioQueueNewOutput fail:%d", rtn);
            [self release];
            return nil;
        }
        AudioQueueSetParameter(_queue, kAudioQueueParam_Volume, 1.0f);
        
        status = AudioQueueAddPropertyListener(_queue, kAudioQueueProperty_IsRunning, PropertyListener, (__bridge void*)self);
        
        if (status != noErr) {
            NSLog(@"AudioQueueAddPropertyListener fail:%d", rtn);
            [self release];
            return nil;
        }
        for (int i = 0; i < AudioQueueBufferCount; ++i) {
            status = AudioQueueAllocateBuffer(_queue, AudioQueueBufferSize, &_buffers[i]);
            if (status != noErr) {
                NSLog(@"AudioQueueAllocateBuffer fail:%d", rtn);
                [self release];
                return nil;
            }
        }
    }
    return self;
}

-(void)dealloc {
    _state = OggAudioStateError;
    
    if (_queue) {
        AudioQueueStop(_queue, true);
        for (int i = 0; i < AudioQueueBufferCount; ++i) {
            if (_buffers[i]) {
                AudioQueueFreeBuffer(_queue, _buffers[i]);
            }
        }
        AudioQueueDispose(_queue, true);
    }
    ov_clear(&_oggVorbisFile);
    if (_file) {
        fclose(_file);
    }
    [super dealloc];
}

-(BOOL)isLooping {
    return _loop;
}

-(BOOL)isPlaying {
    return _state == OggAudioStatePlaying;
}

- (void)play:(BOOL)loop volume:(float)volume {
    switch(_state) {
        case OggAudioStateStopped:
            for (int i = 0; i < AudioQueueBufferCount; ++i) {
                [self enqueueBuffer:_buffers[i]];
            }
            break;
        case OggAudioStatePlaying:
        case OggAudioStateError:
            return;
    }
    _loop = loop;
    _subVolume = volume;
    [self applyVolume];
    
    OSStatus status = AudioQueueStart(_queue, NULL);
    if (status == noErr) {
        _state = OggAudioStatePlaying;
    }
    else {
        NSLog(@"AudioQueueStart fail:%d", (int)status);
        
        _state = OggAudioStateError;
        [_delegate audioError:self];
    }
}

-(void)stop {
    if (_state == OggAudioStatePlaying || _state == OggAudioStatePaused) {
        _state = OggAudioStateStopping;
        OSStatus status = AudioQueueStop(_queue, true);
        if (status != noErr) {
            NSLog(@"AudioQueueStop fail:%d", (int)status);
            
            _state = OggAudioStateError;
            [_delegate audioError:self];
        }
    }
}

-(void)pause {
    if (_state == OggAudioStatePlaying) {
        OSStatus status = AudioQueuePause(_queue);
        
        if (status == noErr) {
            _state = OggAudioStatePaused;
        }
        else {
            NSLog(@"AudioQueuePause fail:%d", (int)status);
            
            _state = OggAudioStateError;
            [_delegate audioError:self];
        }
    }
}

-(void)resume {
    if (_state == OggAudioStatePaused) {
        OSStatus status = AudioQueueStart(_queue, NULL);
        
        if (status == noErr) {
            _state = OggAudioStatePlaying;
        }
        else {
            NSLog(@"AudioQueueStart fail:%d", (int)status);
            
            _state = OggAudioStateError;
            [_delegate audioError:self];
        }
    }
}

-(void)applyVolume {
    OSStatus status = AudioQueueSetParameter(_queue, kAudioQueueParam_Volume, _volume * _subVolume);
    if (status != noErr) {
        NSLog(@"AudioQueueSetParameter fail:%d", (int)status);
    }
}

-(BOOL)readBuffer:(AudioQueueBufferRef)buffer {
    int currentSection = -1;
    uint nTotalBytesRead = 0;
    int nBytesRead = 0;
    
    do {
        nBytesRead = ov_read(&_oggVorbisFile,
                             (char*)buffer->mAudioData + nTotalBytesRead,
                             (int)(buffer->mAudioDataBytesCapacity - nTotalBytesRead),
                             0, 2, 1, &currentSection);
                             
        if (nBytesRead  <= 0)
            break;
        nTotalBytesRead += nBytesRead;
    } while(nTotalBytesRead < buffer->mAudioDataBytesCapacity);

    buffer->mAudioDataByteSize = nTotalBytesRead;
    buffer->mPacketDescriptionCount = 0;
    return nTotalBytesRead > 0;
}

-(void)enqueueBuffer:(AudioQueueBufferRef)buffer {
    switch (_state) {
        case OggAudioStateStopping:
        case OggAudioStateError:
            return;
    }
    if ([self readBuffer:buffer]) {
        OSStatus status = AudioQueueEnqueueBuffer(_queue, buffer, 0, 0);
        if (status != noErr) {
            NSLog(@"AudioQueueEnqueueBuffer fail:%d", (int)status);

            AudioQueueStop(_queue, true);
            _state = OggAudioStateError;
            
            [_delegate audioError:self];
        }
    }
    else {
        OSStatus status = AudioQueueStop(_queue, false);
        if (status != noErr) {
            NSLog(@"AudioQueueStop fail:%d", (int)status);
            
            _state = OggAudioStateError;

            [_delegate audioError:self];
        }
    }
}

-(void)stoppedListener {
    if (_state == OggAudioStateError) {
        return;
    }
    int result = ov_time_seek(&_oggVorbisFile, 0.0);
    OSStatus status = AudioQueueReset(_queue);
    
    if (result) {
        NSLog(@"ov_time_seek fail:%d", result);
        _state = OggAudioStateError;
        [_delegate audioError:self];
    }
    else if (status != noErr) {
        NSLog(@"AudioQueueReset fail:%d", (int)status);
        _state = OggAudioStateError;
        [_delegate audioError:self];
    }
    else if (_state == OggAudioStatePlaying) {
        if (_loop) {
            for (int i = 0; i < AudioQueueBufferCount; ++i) {
                [self enqueueBuffer:_buffers[i]];
            }
            OSStatus status = AudioQueueStart(_queue, NULL);
            
            if (status != noErr) {
                NSLog(@"AudioQueueStart fail:%d", (int)status);
                _state = OggAudioStateError;
                [_delegate audioError:self];
            }
        }
        else {
            _state = OggAudioStateStopped;
            [_delegate audioFinish:self];
        }
    }
    else {
        _state = OggAudioStateStopped;
    }
}

@end

#endif
