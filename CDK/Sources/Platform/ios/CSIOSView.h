//
//  CSIOSView.h
//  CDK
//
//  Created by chan on 13. 3. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#if defined(CDK_IOS) && defined(CDK_IMPL)

#import "CSView.h"
#import "CSGraphics.h"

#import <UIKit/UIKit.h>

@interface CSIOSView : UIView {
	CSView* _view;
	CSGraphics* _graphics;
	CSResolution _resolution;
    uint _renderBuffer;
    uint _frameBuffer;
    CSGLContextIdentifier _identifier;
}

@property (nonatomic, readonly) CSView* view;
@property (nonatomic, readonly) CSGraphics* graphics;
@property (nonatomic, readwrite, assign) CSResolution resolution;

-(void)dispose;
#ifdef CS_IOS_OES_RELOAD
-(void)reload;
#endif
-(void)render;

@end

#endif
