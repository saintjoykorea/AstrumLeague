//
//  CSAppDelegate.h
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#if defined(CDK_IOS) && defined(CDK_IMPL)

#import "CSIOSView.h"

@interface CSAppDelegate : NSObject<UIApplicationDelegate> {
	UIWindow* _window;
	UIViewController* _viewController;
    CSIOSView* _view;
    EAGLContext* _eaglContext;
	CADisplayLink* _displayLink;
    NSMutableArray* _delegates;
	BOOL _paused;
    BOOL _alive;
}
@property (nonatomic, readonly) UIViewController* viewController;
@property (nonatomic, readonly) CSIOSView* view;
@property (nonatomic, readonly) EAGLContext* eaglContext;

+(CSAppDelegate*)sharedDelegate;
-(void)addSubDelegate:(NSObject<UIApplicationDelegate>*)delegate;
-(void)removeSubDelegate:(NSObject<UIApplicationDelegate>*)delegate;
//-(UIRectEdge)preferredScreenEdgesDeferringSystemGestures;

@end

#endif
