//
//  CSAppDelegate.m
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppDelegate.h"

#import "CSDictionary.h"

#import "CSThread.h"
#import "CSGraphicsImpl.h"
#import "CSAudio.h"
#import "CSApplication.h"
#import "CSDevice.h"
#import "CSSignal.h"

@interface CSRootViewController : UIViewController {
	unsigned int _supportedInterfaceOrientations;
}

-(id)init;

@end

@implementation CSRootViewController

-(id)init {
	if (self = [super init]) {
		NSArray* orientations = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"UISupportedInterfaceOrientations"];

		_supportedInterfaceOrientations = 0;

		for (NSString* orientation in orientations) {
			if ([orientation isEqualToString:@"UIInterfaceOrientationLandscapeLeft"]) {
				_supportedInterfaceOrientations |= UIInterfaceOrientationMaskLandscapeLeft;
			}
			else if ([orientation isEqualToString:@"UIInterfaceOrientationLandscapeRight"]) {
				_supportedInterfaceOrientations |= UIInterfaceOrientationMaskLandscapeRight;
			}
			else if ([orientation isEqualToString:@"UIInterfaceOrientationPortrait"]) {
				_supportedInterfaceOrientations |= UIInterfaceOrientationMaskPortrait;
			}
			else if ([orientation isEqualToString:@"UIInterfaceOrientationPortraitUpsideDown"]) {
				_supportedInterfaceOrientations |= UIInterfaceOrientationMaskPortraitUpsideDown;
			}
		}
	}
	return self;
}

-(UIRectEdge)preferredScreenEdgesDeferringSystemGestures {
    return UIRectEdgeAll;
}

- (void)viewDidAppear:(BOOL)isAnimated {
    [super viewDidAppear:isAnimated];
    if (@available(iOS 11.0, *))
    {
        [self setNeedsUpdateOfHomeIndicatorAutoHidden];
        [self setNeedsUpdateOfScreenEdgesDeferringSystemGestures];
    }
}

-(BOOL)shouldAutorotate {
	return YES;
}

-(NSUInteger)supportedInterfaceOrientations {
	return _supportedInterfaceOrientations;
}

-(BOOL)prefersStatusBarHidden {
    return YES;
}

-(BOOL)prefersHomeIndicatorAutoHidden {
    return NO;
}

@end

@implementation CSAppDelegate

@synthesize view = _view, viewController = _viewController, eaglContext = _eaglContext;

+(id)sharedDelegate {
    return [[UIApplication sharedApplication] delegate];
}

-(void)addSubDelegate:(NSObject<UIApplicationDelegate>*)delegate {
    [_delegates addObject:[NSValue valueWithNonretainedObject:delegate]];
}

-(void)removeSubDelegate:(NSObject<UIApplicationDelegate>*)delegate {
    for (int i = 0; i < _delegates.count; i++) {
        NSObject<UIApplicationDelegate>* other = [[_delegates objectAtIndex:i] nonretainedObjectValue];
        
        if (other == delegate) {
            [_delegates removeObjectAtIndex:i];
            break;
        }
    }
}

-(void)destroy {
    if (_alive) {
        CSApplication::sharedApplication()->expireGLContextIdentifier();
   
        [_displayLink release];

        _viewController.view = nil;
        [_viewController release];
        [_window release];
		[_view dispose];		//view가 즉시 삭제되지 않아서 dealloc을 통해 해제하면 안된다.
        [_view release];
        [_eaglContext release];
        [EAGLContext setCurrentContext:nil];
        
        CSApplication::sharedApplication()->OnDestroy();
        onDestroy();
        
        CSGraphicsImpl::finalize();
        CSAudio::finalize();
        CSThread::finalize();
        CSSignal::finalize();

        [_delegates release];
        
        [[NSNotificationCenter defaultCenter] removeObserver:self];
        
        _alive = NO;
    }
}

-(void)dealloc {
	[self destroy];
    
    [super dealloc];
}

-(void)forwardInvocation:(NSInvocation*)invocation {
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        
        if ([delegate respondsToSelector:invocation.selector]) {
            [invocation invokeWithTarget:delegate];
        }
    }
    [super forwardInvocation:invocation];
}

-(void)raiseMemoryWarning:(CSMemoryWarningLevel)level {
    CSApplication::sharedApplication()->OnMemoryWarning(level);
    onMemoryWarning(_view.view, level);
    if (level == CSMemoryWarningLevelCritical) {
        CSGraphicsImpl::sharedImpl()->purge();
    }
}

#ifdef CS_IOS_CUSTOM_MEMORY_WARNING
-(void)checkCustomMemoryWarning {
    CSMemoryUsage usage = CSDevice::memoryUsage();
    if (usage.freeMemory) {
        static const int64 warningMemories[] = {
            64 * 1024768,
            52 * 1024768,
            40 * 1024768
        };
        int i;
        for (i = 0; i < 3 ;i++) {
            if (usage.freeMemory > warningMemories[i]) {
                break;
            }
        }
        if (i) {
            NSLog(@"custom memory warning:%d", i - 1);
            
            [self raiseMemoryWarning:(CSMemoryWarningLevel)(i - 1)];
        }
    }
}
#endif

-(void)timeout {
	CSThread::mainThread()->execute();

#ifdef CS_IOS_CUSTOM_MEMORY_WARNING
    [self checkCustomMemoryWarning];
#endif
}

-(void)handleOpenURL:(NSURL*)url {
    NSString* urlstr = url.absoluteString;
    
    CSDictionary<CSString, CSString>* queries = NULL;
    
    NSUInteger index = [urlstr rangeOfString:@"?"].location;
    if (index != NSNotFound) {
        queries = CSDictionary<CSString, CSString>::dictionary();
        for (NSString* str in [[urlstr substringFromIndex:index + 1] componentsSeparatedByString:@"&"]) {
            NSArray* comp = [str componentsSeparatedByString:@"="];
            if (comp.count == 2) {
                CSString* key = CSString::string([[comp objectAtIndex:0] UTF8String]);
                CSString* value = CSString::string([[comp objectAtIndex:1] UTF8String]);
                
                queries->setObject(key, value);
            }
        }
    }
    const char* curlstr = urlstr.UTF8String;
    CSApplication::sharedApplication()->OnReceiveQuery(curlstr, queries);
    onReceiveQuery(_view.view, curlstr, queries);
}

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    srand(clock());
    
	CSThread::initialize();
    CSSignal::initialize();

    _eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
	if (!_eaglContext) {
		_eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	}
	[EAGLContext setCurrentContext:_eaglContext];

	CSGraphicsImpl::initialize();
    CSAudio::initialize();
    
	_window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	_viewController = [[CSRootViewController alloc] init];
	_view = [[CSIOSView alloc] initWithFrame:_window.bounds];
    
	_viewController.view = _view;
	_window.rootViewController = _viewController;
	[_window makeKeyAndVisible];
    
    _delegates = [[NSMutableArray alloc] init];
    
    onStart(_view.view);

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    
    _alive = YES;
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(application:didFinishLaunchingWithOptions:)]) {
            [delegate application:application didFinishLaunchingWithOptions:launchOptions];
        }
    }
    
	return YES;
}

-(void)applicationDidBecomeActive:(UIApplication*)application {
    NSLog(@"applicationDidBecomeActive");
    
    if (!_displayLink) {
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(timeout)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    if (_paused) {
#ifdef CS_IOS_OES_RELOAD
        CSGraphicsImpl::sharedImpl()->reload();
        [((CSIOSView*)_viewController.view) reload];
        CSApplication::sharedApplication()->OnReload();
        onReload(_view.view);
#endif
        CSAudio::autoResume();
        
        CSThread::notifyResumeApp();
        CSApplication::sharedApplication()->OnResume();
        onResume(_view.view);
        
        _paused = NO;
    }
    
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)]) {
            [delegate applicationDidBecomeActive:application];
        }
    }
}

-(void)applicationWillResignActive:(UIApplication*)application {
    NSLog(@"applicationWillResignActive");
    
	[_displayLink invalidate];
	_displayLink = nil;

	if (!_paused) {
        CSApplication::sharedApplication()->OnPause();
		onPause(_view.view);
        CSThread::notifyPauseApp();

        CSAudio::autoPause();
        
		_paused = YES;
	}
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationWillResignActive:)]) {
            [delegate applicationWillResignActive:application];
        }
    }
}

-(void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"applicationDidEnterBackground");
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)]) {
            [delegate applicationDidEnterBackground:application];
        }
    }
}

-(void)applicationWillEnterForeground:(UIApplication *)application {
    NSLog(@"applicationWillEnterForeground");
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)]) {
            [delegate applicationWillEnterForeground:application];
        }
    }
}

-(void)applicationWillTerminate:(UIApplication*)application {
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationWillTerminate:)]) {
            [delegate applicationWillTerminate:application];
        }
    }
    
	[self destroy];
}

-(void)applicationDidReceiveMemoryWarning:(UIApplication*)application {
    [self raiseMemoryWarning:CSMemoryWarningLevelLow];
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(applicationDidReceiveMemoryWarning:)]) {
            [delegate applicationDidReceiveMemoryWarning:application];
        }
    }
}

- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options {
    [self handleOpenURL:url];
    
    for (NSValue* value in _delegates) {
        NSObject<UIApplicationDelegate>* delegate = [value nonretainedObjectValue];
        if ([delegate respondsToSelector:@selector(application:openURL:options:)]) {
            [delegate application:application openURL:url options:options];
        }
    }
    
    return YES;
}

-(void)keyboardWillShow:(NSNotification*)notification {
    CGRect keyboardFrame = [[notification.userInfo valueForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    
    _view.view->setKeyboardHeight(keyboardFrame.size.height);
}

-(void)keyboardWillHide:(NSNotification*)notification {
    _view.view->setKeyboardHeight(0);
}

@end

void CSApplication::setVersion(const char* version) {
    if (version) {
        if (!_version) _version = new CSString(version);
        else _version->set(version);
    }
    else {
        CSObject::release(_version);
    }
}

void CSApplication::setResolution(CSResolution resolution) {
    [CSAppDelegate sharedDelegate].view.resolution = resolution;
}

CSResolution CSApplication::resolution() const {
    return [CSAppDelegate sharedDelegate].view.resolution;
}

void CSApplication::openURL(const char *url) {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]] options:@{} completionHandler:nil];
}

const CSString* CSApplication::clipboard() {
    NSString* str = [[UIPasteboard generalPasteboard] string];
    
    return str ? CSString::string(str.UTF8String) : NULL;
}

void CSApplication::setClipboard(const char *text) {
    [[UIPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:text]];
}

void CSApplication::shareUrl(const char* title, const char* message, const char* url) {
    NSArray* items = @[[NSString stringWithFormat:@"%s\n%s", message, url]];

    UIActivityViewController* controller = [[UIActivityViewController alloc] initWithActivityItems:items applicationActivities:nil];
    controller.excludedActivityTypes = @[];
    
    if ([controller respondsToSelector:@selector(popoverPresentationController)]) {
        controller.popoverPresentationController.sourceView = [[CSAppDelegate sharedDelegate] view];
        controller.popoverPresentationController.sourceRect = CGRectMake([[[[CSAppDelegate sharedDelegate] viewController] view] frame].size.width / 2, [[[[CSAppDelegate sharedDelegate] viewController] view] frame].size.height / 4, 0, 0);
    }

    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:controller animated:YES completion:nil];

    [controller autorelease];
}

void CSApplication::finish() {
    [[CSAppDelegate sharedDelegate] destroy];
    
    exit(0);
}

#endif
