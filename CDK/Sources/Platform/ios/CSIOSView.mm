//
//  CSIOSView.m
//  CDK
//
//  Created by chan on 13. 3. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSIOSView.h"

#import "CSOpenGL.h"

#import "CSGraphicsImpl.h"

#import "CSAppDelegate.h"

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreFoundation/CoreFoundation.h>

@implementation CSIOSView

@synthesize view = _view;
@synthesize graphics = _graphics;

-(NSString*)resolutionPath {
	NSString* dirpath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	return [dirpath stringByAppendingString:@"/resolution.xmf"];
}

-(void)loadResolution {
	NSString* path = [self resolutionPath];
	
	NSData* data = [NSData dataWithContentsOfFile:path];
	
	if (data) {
		_resolution = (CSResolution)*(const byte*)data.bytes;
	}
	else {
		_resolution = CSResolution1080;
	}
}

-(void)saveResolution {
	NSString* path = [self resolutionPath];

	if (_resolution != CSResolution1080) {
		NSData* data = [NSData dataWithBytesNoCopy:&_resolution length:1 freeWhenDone:NO];
	
		[data writeToFile:path atomically:YES];
	}
	else {
		[[NSFileManager defaultManager] removeItemAtPath:path error:nil];
	}
}

-(void)applyResolution {
    CGSize screenSize = UIScreen.mainScreen.bounds.size;
	
    CGFloat screenScale = UIScreen.mainScreen.scale;
    
	int width, height;
	switch(_resolution) {
		case CSResolution720:
			width = 720;
			height = 1280;
			break;
		case CSResolution1080:
			width = 1080;
			height = 1920;
			break;
		default:
            [self setContentScaleFactor:screenScale];
            NSLog(@"resolution scale:%d, %f", _resolution, screenScale);
            return;
	}
	if (screenSize.width > screenSize.height) {
		int temp = width;
		width = height;
		height = temp;
	}
	CGFloat scale = CSMath::clamp(CSMath::max(width / screenSize.width, height / screenSize.height), (CGFloat)1, screenScale);
	[self setContentScaleFactor:scale];
    
	NSLog(@"resolution scale:%d, %f", _resolution, scale);
}

-(CSResolution)resolution {
	return _resolution;
}

-(void)setResolution:(CSResolution)resolution {
	if (_resolution != resolution) {
		_resolution = resolution;
		
		[self applyResolution];

		[self saveResolution];
		
        [self layoutSubviews];
	}
}

-(void)setup {
	[self setOpaque:YES];

	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;

	[eaglLayer setDrawableProperties:[NSDictionary dictionaryWithObjectsAndKeys:
									  [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
									  kEAGLColorFormatRGBA8,
									  kEAGLDrawablePropertyColorFormat, nil]];

	[self setMultipleTouchEnabled:YES];
	
	[self loadResolution];
	
	[self applyResolution];
    
    glGenRenderbuffersCS(1, &_renderBuffer);
    glGenFramebuffersCS(1, &_frameBuffer);
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffer);
    glBindRenderbufferCS(GL_RENDERBUFFER, _renderBuffer);
    glFramebufferRenderbufferCS(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    
    CSGraphics::invalidate(CSGraphicsValidationTarget);
    
	_view = new CSView(self);
}

-(void)dispose {
	if (_view) {
		if (_identifier.isValid()) {
			glDeleteFramebuffersCS(1, &_frameBuffer);
			glDeleteRenderbuffersCS(1, &_renderBuffer);
		}
		CSObject::release(_graphics);
		_view->release();
		_view = NULL;
	}
}

-(id)initWithCoder:(NSCoder*)aDecoder {
	if (self = [super initWithCoder:aDecoder]) {
		[self setup];
	}

	return self;
}

-(id)initWithFrame:(CGRect)frame {
	if ((self = [super initWithFrame:frame])) {
		[self setup];
	}

	return self;
}

+(id)viewWithFrame:(CGRect)frame {
	return [[[self alloc] initWithFrame:frame] autorelease];
}

-(void)dealloc {
    [self dispose];

	[super dealloc];
}

+(Class)layerClass {
	return [CAEAGLLayer class];
}

-(void)layoutSubviews {
	[EAGLContext setCurrentContext:[[CSAppDelegate sharedDelegate] eaglContext]];
	glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
	[[EAGLContext currentContext] renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)[self layer]];

    int width, height;
    glGetRenderbufferParameterivCS(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameterivCS(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    if (!_graphics) {
        _graphics = new CSGraphics(CSRenderTarget::target(width, height, true));
    }
    else {
        _graphics->target()->resize(width, height);
    }
	
	NSLog(@"resolution:%d, %d", width, height);
	
	[self render];
}

#define CONVERT_PLATFORM_TOUCH	\
	CSPlatformTouch pts[touches.count];	\
	uint i = 0;	\
	for (UITouch* touch in touches) { \
		CGPoint point = [touch locationInView:self]; \
		pts[i].key = (uint64)touch; \
        pts[i].button = CSTouchButtonFinger; \
		pts[i].x = point.x;	\
        pts[i].y = point.y;	\
		++i; \
	}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	CONVERT_PLATFORM_TOUCH

	_view->touchesBegan(pts, i);
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	CONVERT_PLATFORM_TOUCH

	_view->touchesMoved(pts, i);
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	CONVERT_PLATFORM_TOUCH

	_view->touchesEnded(pts, i);
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event {
	CONVERT_PLATFORM_TOUCH

	_view->touchesCancelled(pts, i);
}

#ifdef CS_IOS_OES_RELOAD
-(void)reload {
    if (_graphics) _graphics->release();
	
    glDeleteFramebuffersCS(1, &_frameBuffer);
    glDeleteRenderbuffersCS(1, &_renderBuffer);
    
    glGenRenderbuffersCS(1, &_renderBuffer);
    glGenFramebuffersCS(1, &_frameBuffer);
    glBindFramebufferCS(GL_FRAMEBUFFER, _frameBuffer);
    glBindRenderbufferCS(GL_RENDERBUFFER, _renderBuffer);
    glFramebufferRenderbufferCS(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    [[EAGLContext currentContext] renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)[self layer]];
    
    int width, height;
    glGetRenderbufferParameterivCS(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameterivCS(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    _graphics = new CSGraphics(CSRenderTarget::target(width, height, true));
    
    CSGraphics::invalidate(CSGraphicsValidationTarget);
}
#endif

-(void)render {
	[EAGLContext setCurrentContext:[[CSAppDelegate sharedDelegate] eaglContext]];

	_view->draw(_graphics);
	
	_graphics->flush();
    
    _graphics->target()->blit(_frameBuffer, false);
    
	glBindRenderbufferCS(GL_RENDERBUFFER, _renderBuffer);
	[[EAGLContext currentContext] presentRenderbuffer:GL_RENDERBUFFER];
    
    CSGraphics::invalidate(CSGraphicsValidationTarget | CSGraphicsValidationStencil);
}

@end

#endif
