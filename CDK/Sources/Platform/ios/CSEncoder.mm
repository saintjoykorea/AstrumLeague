//
//  CSEncoder.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSEncoder.h"

#import <Foundation/Foundation.h>

CSString* CSEncoder::createStringWithBytes(const void* bytes, uint length, CSStringEncoding encoding) {
	NSStringEncoding nsencoding;

	switch (encoding) {
		case CSStringEncodingUTF8:
			return new CSString((const char*)bytes, length);
		case CSStringEncodingUTF16:
			nsencoding = NSUTF16StringEncoding;
			break;
		case CSStringEncodingUTF16LE:
			nsencoding = NSUTF16LittleEndianStringEncoding;
			break;
		case CSStringEncodingUTF16BE:
			nsencoding = NSUTF16BigEndianStringEncoding;
			break;
	}

	NSString* nsstr = [[NSString alloc] initWithBytes:bytes length:length encoding:nsencoding];
	CSString* str = new CSString(nsstr.UTF8String);
	[nsstr release];

	return str;
}

void* CSEncoder::createRawWithString(const char* str, uint& length, CSStringEncoding encoding) {
	NSStringEncoding nsencoding;

	switch (encoding) {
		case CSStringEncodingUTF8:
			{
				length = strlen(str);
                if (!length) {
                    return NULL;
                }
				void* buf = fmalloc(length);
                memcpy(buf, str, length);
				return buf;
			}
		case CSStringEncodingUTF16:
			nsencoding = NSUTF16StringEncoding;
			break;
		case CSStringEncodingUTF16LE:
			nsencoding = NSUTF16LittleEndianStringEncoding;
			break;
		case CSStringEncodingUTF16BE:
			nsencoding = NSUTF16BigEndianStringEncoding;
			break;
	}
	NSString* nsstr = [[NSString alloc] initWithBytes:str length:strlen(str) encoding:NSUTF8StringEncoding];
    NSData* nsdata = [nsstr dataUsingEncoding:nsencoding];

	if (!nsdata) {
		return NULL;
	}

	[nsstr release];

	length = nsdata.length;
	void* data = fmalloc(length);
    memcpy(data, nsdata.bytes, length);

	return data;
}

#endif
