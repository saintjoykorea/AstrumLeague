//
//  CSStringBreakerIOS.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 1. 14..
//  Copyright © 2019년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSString.h"

#import "CSMemory.h"

#import <Foundation/Foundation.h>

//=============================
//TODO:DEBUG
#import <unicode/ushape.h>
#import <unicode/ubidi.h>
//=============================

static NSArray* split(UBreakIteratorType type, NSString* nsstr) {
	NSStringEnumerationOptions option;
    
    switch (type) {
        case UBRK_WORD:
            option = NSStringEnumerationByWords;
            break;
        case UBRK_LINE:
            option = NSStringEnumerationByLines;
            break;
        case UBRK_SENTENCE:
            option = NSStringEnumerationBySentences;
            break;
        default:
            option = NSStringEnumerationByComposedCharacterSequences;
            break;
    }
    
    NSMutableArray* nssubstrs = [NSMutableArray arrayWithCapacity:nsstr.length];
    
    [nsstr enumerateSubstringsInRange:NSMakeRange(0, nsstr.length)
                              options:option
                           usingBlock:^(NSString * _Nullable substring, NSRange substringRange, NSRange enclosingRange, BOOL * _Nonnull stop)
     {
		 uint a = enclosingRange.location;
		 uint b = substringRange.location;
        for (int i = a; i < b; i++) [nssubstrs addObject:[nsstr substringWithRange:NSMakeRange(i, 1)]];
		 
         [nssubstrs addObject:substring];
		 
         a = substringRange.location + substringRange.length;
         b = enclosingRange.location + enclosingRange.length;
        for (int i = a; i < b; i++) [nssubstrs addObject:[nsstr substringWithRange:NSMakeRange(i, 1)]];

         *stop = NO;
     }];
    
    if (nssubstrs.count) {
        int i = 0;
        while (i < nssubstrs.count) {
            unichar c = [[nssubstrs objectAtIndex:i] characterAtIndex:0];

            switch (c) {
                case 0x0E33:
                    [nssubstrs removeObjectAtIndex:i];
                    break;
                default:
                    i++;
                    break;
            }
        }
    }
	else if (nsstr.length) {
		[nssubstrs addObject:nsstr];
	}
	return nssubstrs;
}

CSCharSequence::CSCharSequence(UBreakIteratorType type, const char* str) : _seq(NULL), _count(0) {
	NSArray* nssubstrs = split(type, [NSString stringWithUTF8String:str]);
    
    _count = nssubstrs.count;
    
    if (!_count) return;
	
	_seq = (uint*)fmalloc(_count * sizeof(uint));
	
	uint offset = 0;
	for (int i = 0; i < nssubstrs.count; i++) {
		offset += strlen([[nssubstrs objectAtIndex:i] UTF8String]);
		_seq[i] = offset;
	}
	if (type == UBRK_WORD) wordMerge(str);
}

CSCharSequence::CSCharSequence(UBreakIteratorType type, const uchar* str) : _seq(NULL), _count(0) {
	NSArray* nssubstrs = split(type, [NSString stringWithCharacters:str length:u_strlen(str)]);
    
	if (!nssubstrs.count) return;
	
    _count = nssubstrs.count;
    
    if (!_count) return;
	
	_seq = (uint*)fmalloc(_count * sizeof(uint));
	
	uint offset = 0;
	for (int i = 0; i < nssubstrs.count; i++) {
		offset += [[nssubstrs objectAtIndex:i] length];
		_seq[i] = offset;
	}
	if (type == UBRK_WORD) wordMerge(str);
}

#endif
