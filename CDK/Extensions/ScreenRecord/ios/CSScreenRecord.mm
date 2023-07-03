//
//  ASScreenRecorder.m
//  ScreenRecorder
//
//  Created by Alan Skipp on 23/04/2014.
//  Copyright (c) 2014 Alan Skipp. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppDelegate.h"
#import "CSScreenRecord.h"

#import <UIKit/UIKit.h>
#import <Photos/Photos.h>
#import <ReplayKit/ReplayKit.h>

#define documentsDirectory [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]

typedef void (^VideoCompletionBlock)(void);


@protocol ASScreenRecorderDelegate;

@interface CSScreenRecordImpl : NSObject<UIApplicationDelegate> {
    enum {
        ENC_AAC = 1,
        ENC_ALAC = 2,
        ENC_IMA4 = 3,
        ENC_ILBC = 4,
        ENC_ULAW = 5,
        ENC_PCM = 6,
    } encodingTypes;
    
    dispatch_queue_t _render_queue;
    dispatch_queue_t _append_pixelBuffer_queue;
    dispatch_semaphore_t _frameRenderingSemaphore;
    dispatch_semaphore_t _pixelAppendSemaphore;
    
    CGSize _viewSize;
    CGFloat _scale;
    
    CGColorSpaceRef _rgbColorSpace;
    CVPixelBufferPoolRef _outputBufferPool;
}

@property (strong, nonatomic) AVAssetWriter *videoWriter;
@property (strong, nonatomic) AVAssetWriterInput *videoWriterInput;
@property (strong, nonatomic) AVAssetWriterInputPixelBufferAdaptor *avAdaptor;
@property (strong, nonatomic) CADisplayLink *displayLink;
@property (strong, nonatomic) NSDictionary *outputBufferPoolAuxAttributes;
@property (nonatomic) CFTimeInterval firstTimeStamp;
@property (nonatomic) BOOL isRecording;
@property (strong, nonatomic) NSURL *videoURL;
@property (nonatomic, weak) id <ASScreenRecorderDelegate> delegate;

@property (strong, nonatomic) RPScreenRecorder *screenRecorder;
@property (strong, nonatomic) AVAssetWriter *assetWriter;
@property (strong, nonatomic) AVAssetWriterInput *assetVideoInput;
@property (strong, nonatomic) AVAssetWriterInput *assetAudioInput;
@end

@protocol ASScreenRecorderDelegate <NSObject>
- (void)writeBackgroundFrameInContext:(CGContextRef*)contextRef;
@end

@interface CSScreenRecordImpl() <RPPreviewViewControllerDelegate>
@end

@implementation CSScreenRecordImpl

#pragma mark - initializers

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    /*
    CSAppDelegate *appDelegate = (CSAppDelegate*)[[UIApplication sharedApplication] delegate];
    _viewSize = appDelegate.view.bounds.size;
    
    // record half size resolution for retina iPads
    if ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) && _scale > 1) {
        _scale = 1.0;
    }
    _isRecording = NO;
    
    _append_pixelBuffer_queue = dispatch_queue_create("ASScreenRecorder.append_queue", DISPATCH_QUEUE_SERIAL);
    _render_queue = dispatch_queue_create("ASScreenRecorder.render_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(_render_queue, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    _frameRenderingSemaphore = dispatch_semaphore_create(1);
    _pixelAppendSemaphore = dispatch_semaphore_create(1);
    */
    return YES;
}

#pragma mark - public

- (void)setVideoURL:(NSURL *)videoURL
{
    NSAssert(!_isRecording, @"videoURL can not be changed whilst recording is in progress");
    _videoURL = videoURL;
}

-(BOOL)application {
    return _isRecording;
}

- (NSURL*)tempFileURL
{
    NSString *outputPath = [NSHomeDirectory() stringByAppendingPathComponent:@"tmp/screenCapture.mp4"];
    [self removeTempFilePath:outputPath];
    return [NSURL fileURLWithPath:outputPath];
}

- (IBAction) startRecording:(NSInteger)displayWidth displayHeight:(NSInteger)displayHeight micEnabled:(BOOL)micEnabled
{
    PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatus];
    
    if (status == PHAuthorizationStatusAuthorized) {
        // Access has been granted.
        [self acceptRecording:displayWidth displayHeight:displayHeight micEnabled:micEnabled];
    }
    else if (status == PHAuthorizationStatusNotDetermined) {
        // Access has not been determined.
        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status1) {
            if (status1 == PHAuthorizationStatusAuthorized) {
                // Access has been granted.
                [self acceptRecording:displayWidth displayHeight:displayHeight micEnabled:micEnabled];
            }
            else {
                // Access has been denied.
            }
        }];
    }
    else {
        // Access has been denied.
    }
}

- (void)acceptRecording:(NSInteger)displayWidth displayHeight:(NSInteger)displayHeight micEnabled:(BOOL)micEnabled
{
    self.screenRecorder = [RPScreenRecorder sharedRecorder];
    // 음성도 같이 녹화할 경우 사용.
    [self.screenRecorder setMicrophoneEnabled:micEnabled];
    _isRecording = true;
    
    if (@available(iOS 11, *)) {
        NSError *error = nil;
        self.assetWriter = [AVAssetWriter assetWriterWithURL:self.tempFileURL fileType:AVFileTypeMPEG4 error:&error];
        
        // Video Compression에 대한 설정.
        NSDictionary *compressionProperties = @{AVVideoProfileLevelKey         : AVVideoProfileLevelH264HighAutoLevel,
                                                AVVideoH264EntropyModeKey      : AVVideoH264EntropyModeCABAC,
                                                AVVideoAverageBitRateKey       : @(displayWidth * displayHeight * 11.4),
                                                AVVideoMaxKeyFrameIntervalKey  : @30,
                                                AVVideoAllowFrameReorderingKey : @NO};
        // Video Configurate 설정.
        NSDictionary *videoSettings = @{AVVideoCompressionPropertiesKey : compressionProperties,
                                        AVVideoCodecKey                 : AVVideoCodecH264,
                                        AVVideoScalingModeKey           : AVVideoScalingModeResizeAspect,
                                        AVVideoWidthKey                 : @(displayWidth),
                                        AVVideoHeightKey                : @(displayHeight)};
        
        self.assetVideoInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
        
        // assetWriter에 videoInput을 설정.
        if([self.assetWriter canAddInput:self.assetVideoInput])
        {
            NSLog(@"AssetWriter Can AddInput");
            [self.assetVideoInput setMediaTimeScale:60];
            [self.assetVideoInput setExpectsMediaDataInRealTime:YES];
            [self.assetWriter addInput:self.assetVideoInput];
            [self.assetWriter setMovieTimeScale:60];
        }
        
        
        // Audio Channel에 대한 설정.
        AudioChannelLayout stereoChannelLayout = {
            .mChannelLayoutTag = kAudioChannelLayoutTag_Stereo,
            .mChannelBitmap = 0,
            .mNumberChannelDescriptions = 0
        };
        NSData *channelLayoutAsData = [NSData dataWithBytes:&stereoChannelLayout length:offsetof(AudioChannelLayout, mChannelDescriptions)];
        
        // Audio Configurate 설정.
        NSDictionary *audioSettings = @{
                                        AVFormatIDKey         : [NSNumber numberWithUnsignedInt:kAudioFormatMPEG4AAC],
                                        AVEncoderBitRateKey   : [NSNumber numberWithInteger:64000],
                                        AVSampleRateKey       : [NSNumber numberWithInteger:44100],
                                        AVChannelLayoutKey    : channelLayoutAsData,
                                        AVNumberOfChannelsKey : [NSNumber numberWithUnsignedInteger:2]
                                        };
        
        self.assetAudioInput = [[AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:audioSettings] retain];
        
        // assetWriter에 audioInput을 설정.
        if([self.assetWriter canAddInput:self.assetAudioInput])
        {
            [self.assetAudioInput setExpectsMediaDataInRealTime:YES];
            [self.assetWriter addInput:self.assetAudioInput];
        }
        
        [self.screenRecorder startCaptureWithHandler:^(CMSampleBufferRef  _Nonnull sampleBuffer, RPSampleBufferType bufferType, NSError * _Nullable error) {
            if (CMSampleBufferDataIsReady(sampleBuffer)) {
                if (self.assetWriter.status == AVAssetWriterStatusUnknown && bufferType == RPSampleBufferTypeVideo) {                
                    if([self.assetWriter startWriting])
                    {
                        [self.assetWriter startSessionAtSourceTime:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
                    }
                }
                
                if (self.assetWriter.status == AVAssetWriterStatusFailed) {
                    NSLog(@"An error occured.");
                    return;
                }
                
                if (bufferType == RPSampleBufferTypeVideo) {
                    if (self.assetVideoInput.isReadyForMoreMediaData) {
                        [self.assetVideoInput appendSampleBuffer:sampleBuffer];
                    }
                }
                if (micEnabled) {
                    if (bufferType == RPSampleBufferTypeAudioMic)
                    {
                        if (self.assetAudioInput.isReadyForMoreMediaData) {
                            [self.assetAudioInput appendSampleBuffer:sampleBuffer];
                        }
                    }
                }
                else {
                    if (bufferType == RPSampleBufferTypeAudioApp)
                    {
                        if (self.assetAudioInput.isReadyForMoreMediaData) {
                            [self.assetAudioInput appendSampleBuffer:sampleBuffer];
                        }
                    }
                }
                
            }
        } completionHandler:^(NSError * _Nullable error) {
            if (!error) {
                NSLog(@"Recording started successfully.");
            }
            else
            {
                NSLog(@"Recod Start Error : %@", error.localizedDescription);
                _isRecording = false;
            }
        }];
    }
    else {
        RPScreenRecorder* recoder = [RPScreenRecorder sharedRecorder];
        [recoder startRecordingWithHandler:^(NSError * _Nullable error) {
            if(error) {
                NSLog(@"Recod Start Error : %@", error.localizedDescription);
                _isRecording = false;
            }
            else {
            }
        }];
    }
}

- (IBAction) stopRecoding {
    if (!_isRecording)
        return;
    
    if (@available(iOS 11, *)) {
        [self.screenRecorder stopCaptureWithHandler:^(NSError * _Nullable error) {
            if (!error) {
                [self.assetVideoInput markAsFinished];
                [self.assetAudioInput markAsFinished];
                
                __block PHObjectPlaceholder *placeholder;
                
                NSLog(@"Recording stopped successfully. Cleaning up...");
                [self.assetWriter finishWritingWithCompletionHandler:^{
                    _isRecording = false;
                    [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
                        PHAssetChangeRequest* createAssetRequest = [PHAssetChangeRequest creationRequestForAssetFromVideoAtFileURL:self.assetWriter.outputURL];
                        placeholder = [createAssetRequest placeholderForCreatedAsset];
                    } completionHandler:^(BOOL success, NSError *error1) {
                        if (success)
                        {
                            NSLog(@"didFinishRecordingToOutputFileAtURL - success for ios9");
                        }
                        else
                        {
                            NSLog(@"%@", error1);
                        }
                        self.assetVideoInput = nil;
                        self.assetAudioInput = nil;
                        self.assetWriter = nil;
                    }];
                }];
            }
            else
            {
                NSLog(@"Recode Stop Error : %@", error.localizedDescription);
                self.assetVideoInput = nil;
                self.assetAudioInput = nil;
                self.assetWriter = nil;
            }
        }];
    }
    else {
        [self.screenRecorder stopRecordingWithHandler:^(RPPreviewViewController * _Nullable previewViewController, NSError * _Nullable error) {
            if(error)
            {
                // 녹화 중지 에러 발생시
                NSLog(@"Recode Stop Error : %@", error.localizedDescription);
            }
            else
            {
                // 현재까지 녹화된 정보를 지우고 싶을때 아래의 주석을 해제
                //[recoder discardRecordingWithHandler:^{
                //
                //}];
                dispatch_async(dispatch_get_main_queue(), ^{
                    previewViewController.previewControllerDelegate = self;
                    CSAppDelegate *appDelegate = (CSAppDelegate*)[[UIApplication sharedApplication] delegate];
                    
                    [appDelegate.viewController presentViewController:previewViewController animated:YES completion:nil];
                });
                
            }
        }];
        _isRecording = false;
    }
    [self.screenRecorder setMicrophoneEnabled:false];
    
}

- (void) previewControllerDidFinish:(RPPreviewViewController *)previewController
{
    [previewController dismissViewControllerAnimated:YES completion:nil];
}


- (void)removeTempFilePath:(NSString*)filePath
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:filePath]) {
        NSError* error;
        if ([fileManager removeItemAtPath:filePath error:&error] == NO) {
            NSLog(@"Could not delete old recording:%@", [error localizedDescription]);
        }
    }
}

- (void)cleanup
{
    self.avAdaptor = nil;
    self.videoWriterInput = nil;
    self.videoWriter = nil;
    self.firstTimeStamp = 0;
    self.outputBufferPoolAuxAttributes = nil;
    CGColorSpaceRelease(_rgbColorSpace);
    CVPixelBufferPoolRelease(_outputBufferPool);
}

@end

static CSScreenRecordImpl* __sharedImpl = nil;

void CSScreenRecord::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSScreenRecordImpl alloc] init];
        
        [[CSAppDelegate sharedDelegate] addSubDelegate: __sharedImpl];
    }
}

void CSScreenRecord::finalize() {
    if (__sharedImpl) {
        [[CSAppDelegate sharedDelegate] removeSubDelegate: __sharedImpl];
        
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

bool CSScreenRecord::isRecording() {
    return [__sharedImpl isRecording];
}

bool CSScreenRecord::canQualitySelect() {
    if (@available(iOS 11, *)) {
        return true;
    }
    return false;
}

bool CSScreenRecord::canMicSelect() {
    if (@available(iOS 11, *)) {
        return true;
    }
    return false;
}

void CSScreenRecord::recordStart(int displayWidth, int displayHeight, bool micEnabled) {
    if (!__sharedImpl.isRecording) {
        [__sharedImpl startRecording:displayWidth displayHeight:displayHeight micEnabled:micEnabled];
        NSLog(@"Start recording");
    }
}

void CSScreenRecord::recordEnd() {
    if (__sharedImpl.isRecording) {
        [__sharedImpl stopRecoding];
    }
}

#endif
