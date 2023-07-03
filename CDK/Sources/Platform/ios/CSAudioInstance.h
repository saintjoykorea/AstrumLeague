//
//  CSAudioInstance.h
//  CDK
//
//  Created by Kim Chan on 2017. 7. 5..
//  Copyright © 2017년 brgames. All rights reserved.
//

#if defined(CDK_IOS) && defined(CDK_IMPL)

#import <Foundation/Foundation.h>

@class CSAudioInstance;

@protocol CSAudioInstanceDelegate <NSObject>

-(void)audioError:(CSAudioInstance*)instance;
-(void)audioFinish:(CSAudioInstance*)instance;

@end

@interface CSAudioInstance : NSObject {
    NSObject<CSAudioInstanceDelegate>* _delegate;
    NSString* _path;
    int _control;
    float _volume;
    float _subVolume;
    int _priority;
    int _category;
	BOOL _single;
    BOOL _userPaused;
}
@property (nonatomic, readwrite, assign) NSObject<CSAudioInstanceDelegate>* delegate;
@property (nonatomic, readonly) NSString* path;
@property (nonatomic, readwrite) int control;
@property (nonatomic, readwrite) float volume;
@property (nonatomic, readonly) float subVolume;
@property (nonatomic, readwrite) int priority;
@property (nonatomic, readwrite) int category;
@property (nonatomic, readwrite) BOOL single;
@property (nonatomic, readonly) BOOL isLooping;
@property (nonatomic, readonly, getter = isPlaying) BOOL playing;
@property (nonatomic, readwrite, getter = isUserPaused) BOOL userPaused;

+(CSAudioInstance*)createWithPath:(NSString*)path;
+(CSAudioInstance*)instanceWithPath:(NSString*)path;

-(id)initWithPath:(NSString*)path;

-(void)play:(BOOL)loop volume:(float)volume;
-(void)stop;
-(void)pause;
-(void)resume;
-(void)applyVolume;

@end

#endif
