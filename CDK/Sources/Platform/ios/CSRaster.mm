//
//  CSRaster.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSRaster.h"

#import "CSLog.h"

#import "CSDictionary.h"

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreText/CoreText.h>

@interface FaceEntry : NSObject {
	char* _name;
	CSFontStyle _style;
	NSString* _fontName;
}

@property (nonatomic, readonly) const char* name;
@property (nonatomic, readonly) CSFontStyle style;
@property (nonatomic, readonly) NSString* fontName;

-(id)initWithName:(const char*)name style:(CSFontStyle)style fontName:(NSString*)fontName;

@end	

@implementation FaceEntry

@synthesize name = _name;
@synthesize style = _style;
@synthesize fontName = _fontName;

-(id)initWithName:(const char*)name style:(CSFontStyle)style fontName:(NSString*)fontName {
    if (self = [super init]) {
		_name = strdup(name);
		_style = style;
        _fontName = [fontName retain];
    }
    return self;
}

-(void)dealloc {
	free(_name);

	[_fontName release];

	[super dealloc];
}

@end

static NSMutableArray* __faceEntries = [[NSMutableArray alloc] init];

void CSRaster::loadFontHandle(const char* name, CSFontStyle style, const char* path) {
	CGDataProviderRef fontDataProvider = CGDataProviderCreateWithFilename(path);
	CGFontRef customFont = CGFontCreateWithDataProvider(fontDataProvider);
	CGDataProviderRelease(fontDataProvider);
	NSString* fontName = (__bridge NSString*)CGFontCopyPostScriptName(customFont);
	CFErrorRef error;
	bool success = CTFontManagerRegisterGraphicsFont(customFont, &error);
	CGFontRelease(customFont);
	if (success) {
        for (FaceEntry* e in __faceEntries) {
            if (strcmp(e.name, name) == 0 && e.style == style && [e.fontName isEqualToString:fontName]) {
                CSLog("font already exists:%s", fontName.UTF8String);
                return;
            }
        }
		FaceEntry* e = [[FaceEntry alloc] initWithName:name style:style fontName:fontName];
		[__faceEntries addObject:e];
		[e release];
	}
	else {
		CSErrorLog("unable to load font:%s", path);
	}
}

static UIFont* systemFont(float size, CSFontStyle style) {
    switch (style) {
        case CSFontStyleBold:
            return [UIFont boldSystemFontOfSize:size];
        case CSFontStyleItalic:
            return [UIFont italicSystemFontOfSize:size];
        case CSFontStyleMedium:
        case CSFontStyleSemiBold:
            CSWarnLog("unable to find system font style:%d", style);
        default:
            return [UIFont systemFontOfSize:size];
    }
}

void* CSRaster::createSystemFontHandle(float size, CSFontStyle style) {
    return [[NSArray alloc] initWithObjects:systemFont(size, style), nil];
}

void* CSRaster::createFontHandle(const char* name, float size, CSFontStyle style) {
	NSMutableArray* uifonts = [[NSMutableArray alloc] initWithCapacity:4];
	for (FaceEntry* e in __faceEntries) {
		if (strcmp(e.name, name) == 0 && e.style == style) {
			UIFont* uifont = [UIFont fontWithName:e.fontName size:size];

			if (uifont) [uifonts addObject:uifont];
		}
	}
	return uifonts;
}

void CSRaster::destroyFontHandle(void* handle) {
	[(NSArray*)handle release];
}

static UIFont* selectFont(NSString* str, const CSFont* font) {
	int strlen = str.length;
	unichar strbuf[strlen + 1];
	[str getCharacters:strbuf range:NSMakeRange(0, strlen)];
	strbuf[strlen] = 0;

	CGGlyph glyph;
	for (UIFont* uifont in (NSArray*)font->handle()) {
		if (CTFontGetGlyphsForCharacters((__bridge CTFontRef)uifont, strbuf, &glyph, 1)) {
            //CSLog("select font:%s face:%s", str.UTF8String, uifont.fontName.UTF8String);
			return uifont;
		}
	}
    CSErrorLog("unabled to select font:%s font name:%s", str.UTF8String, (const char*)*font->name());
	return systemFont(font->size(), font->style());
}

CSSize CSRaster::characterSize(const uchar* cc, uint cclen, const CSFont* font) {
    NSString* str = [[NSString alloc] initWithCharacters:cc length:cclen];
	UIFont* uifont = selectFont(str, font);
	CGSize size = [str sizeWithAttributes:@{NSFontAttributeName: uifont}];
	[str release];
    
    size.height += font->size() - uifont.ascender;
    
    return CSSize(size.width, size.height);
}

void* CSRaster::createRasterFromCharacter(const uchar* cc, uint cclen, const CSFont* font, uint& width, uint& height, float& offy) {
    NSString* str = [[NSString alloc] initWithCharacters:cc length:cclen];
    
	UIFont* uifont = selectFont(str, font);

	void* raster = NULL;
		
	CGSize size = [str sizeWithAttributes:@{NSFontAttributeName: uifont}];

	width = (uint)ceilf(size.width);
	height = (uint)ceilf(size.height);

	if (width && height) {
		raster = calloc(width * height, 4);
		
		if (raster) {
			CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			CGContextRef context = CGBitmapContextCreate(raster, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
			CGColorSpaceRelease(colorSpace);

			CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 1.0f);
			
			CGContextTranslateCTM(context, 0.0f, height);
			CGContextScaleCTM(context, 1.0f, -1.0f);
			
			UIGraphicsPushContext(context);
			
			[str drawInRect:CGRectMake(0, 0, width, height) withFont:uifont];
			//[str drawInRect:CGRectMake(0, 0, width, height) withAttributes:@{NSFontAttributeName: (UIFont*)font->handle()}];
			
			UIGraphicsPopContext();
			
			CGContextRelease(context);
			
			offy = font->size() - uifont.ascender;
		}
	}
	
    [str release];
    return raster;
}

void* CSRaster::createRasterFromImageSource(const void* source, uint len, uint& width, uint& height) {
	UIImage* image = [[UIImage alloc] initWithData:[NSData dataWithBytesNoCopy:(void*)source length:len freeWhenDone:NO]];

	CGImageRef imageref = [image CGImage];
    
	if (!imageref) {
		width = 0;
		height = 0;
		[image release];
		return NULL;
	}
	width = CGImageGetWidth(imageref);
	height = CGImageGetHeight(imageref);
    
    void* raster = malloc(height * width * 4);
    if (!raster) {
        [image release];
        return NULL;
    }
	CGColorSpaceRef deviceColorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context;

	context = CGBitmapContextCreate(raster, width, height, 8, 4 * width, deviceColorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	CGColorSpaceRelease(deviceColorSpace);
    
    CGContextSetBlendMode(context, kCGBlendModeCopy);
	CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageref);
	CGContextRelease(context);
    
	[image release];

	return raster;
}

bool CSRaster::saveImageFromRaster(const char *path, const void *raster, uint width, uint height) {
    const char* ext = path + strlen(path) - 3;
    
    int format;
    if (strcasecmp(ext, "png") == 0) {
        format = 0;
    }
    else if (strcasecmp(ext, "jpg") == 0) {
        format = 1;
    }
    else {
        CSErrorLog("invalid format:%s", ext);
        return false;
    }

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, raster, width * height * 4, NULL);
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGImageRef imageRef = CGImageCreate(width,
                                        height,
                                        8,
                                        32,
                                        width * 4,
                                        colorSpaceRef,
                                        kCGBitmapByteOrder32Big | kCGImageAlphaLast,
                                        provider,
                                        NULL,
                                        NO,
                                        kCGRenderingIntentDefault);
    
    
    UIImage* image = [[UIImage alloc] initWithCGImage:imageRef];
    
    bool rtn = false;

    NSData* data;
    switch (format) {
        case 0:
            data = UIImagePNGRepresentation(image);
            break;
        default:
            data = UIImageJPEGRepresentation(image, 1.0f);
            break;
    }
    if ([data writeToFile:[NSString stringWithUTF8String:path] atomically:YES]) {
        rtn = true;
    }
    [image release];

    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpaceRef);
    CGImageRelease(imageRef);
    return rtn;
}

#endif
