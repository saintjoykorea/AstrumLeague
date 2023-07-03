//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSVideoViewBridge.h"

#import "CSView.h"

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>


@interface CSVideoViewHandle:NSObject  {
    AVPlayerViewController* _avVideoController;
    CSVideoView *_videoView;
}

@property (nonatomic, readonly) AVPlayerViewController* avVideoController;
@property (nonatomic, readonly) CSVideoView* videoView;

@end

@implementation CSVideoViewHandle

@synthesize  avVideoController = _avVideoController , videoView = _videoView;

//비디오 초기화
-(id)initWithViedoView:(CSVideoView*)videoView {
    if (self = [super init]) {
        _avVideoController = [[AVPlayerViewController alloc] init];
        _avVideoController.showsPlaybackControls = NO;
        //[_avVideoController.view setFrame:CGRectMake (0, 0, 200, 200)]; //self.view.bounds;
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(videoPlayBackDidFinish:)
                                                     name:AVPlayerItemDidPlayToEndTimeNotification
                                                   object:[_avVideoController.player currentItem]];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(replayVideo)
                                                      name:AVPlayerItemDidPlayToEndTimeNotification
                                                    object:[_avVideoController.player currentItem]];
        
        _avVideoController.videoGravity = AVLayerVideoGravityResizeAspectFill;
        _videoView = videoView;
        
    }
    return self;
}

-(void)dealloc {
    [_avVideoController release];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

/*
-(void)playMovie:(NSURL*) urlpath{
    AVPlayer* avplayer  = [AVPlayer playerWithURL:urlpath];
    _avVideoController.player = avplayer;
    
    if(!_avVideoController.player)
        [_avVideoController.player play];
}
*/

- (void)videoPlayBackDidFinish:(NSNotification *)notification {
    _videoView->onPlayFinish();
    //_videoView->onURLFinishLoad();
    
    
    //[[NSNotificationCenter defaultCenter]removeObserver:self name:AVPlayerItemDidPlayToEndTimeNotification object:nil];
    //[_avVideoController.view removeFromSuperview];
    //_avVideoController = nil;
}

- (void)replayVideo {
    if(!_avVideoController.player)
        [_avVideoController.player play];
}

/*
 -(NSURL *)localMovieURL:(NSString *) filename
 {
 NSURL *theMovieURL = nil;
 NSBundle *bundle = [NSBundle mainBundle];
 if (bundle)
 {
 NSString *moviePath = [bundle pathForResource:filename ofType:@"MP4"];
 if (moviePath)
 {
 theMovieURL = [NSURL fileURLWithPath:moviePath];
 }
 }
 return theMovieURL;
 }
 */

@end

//외부 링크 브릿지용

void* CSVideoViewBridge::createHandle(CSVideoView* videoView) {
    return [[CSVideoViewHandle alloc] initWithViedoView:videoView];
    return nil;
}

void CSVideoViewBridge::destroyHandle(void* handle) {
    [(CSVideoViewHandle*)handle release];
}

void CSVideoViewBridge::addToView(void* handle) {
    [(UIView*) ((CSVideoViewHandle*) handle).videoView->view()->handle() addSubview:[[(CSVideoViewHandle*)handle avVideoController] view]];
}

void CSVideoViewBridge::removeFromView(void* handle) {
    [[[(CSVideoViewHandle*)handle avVideoController] view] removeFromSuperview];
}

void CSVideoViewBridge::setFrame(void* handle, const CSRect& frame) {
    CGRect cgframe = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    [[[(CSVideoViewHandle*)handle avVideoController] view] setFrame:cgframe];
}

void CSVideoViewBridge::playStart(void* handle, const char* path) {
    AVPlayer* avplayer  = [AVPlayer playerWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:path]]];
    [(CSVideoViewHandle*)handle avVideoController].player = avplayer;
    [[[(CSVideoViewHandle*)handle avVideoController] player] play];
}

#endif

