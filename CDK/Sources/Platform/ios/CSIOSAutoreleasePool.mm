//
//  CSIOSAutoreleasePool.m
//  CDK
//
//  Created by Kim Chan on 2017. 1. 9..
//  Copyright © 2017년 brgames. All rights reserved.
//
//#import "stdafx.h"

#define CDK_IMPL

#import "CSIOSAutoreleasePool.h"

#import <Foundation/Foundation.h>

void* CSIOSAutoreleasePool::create() {
    return [[NSAutoreleasePool alloc] init];
}

void CSIOSAutoreleasePool::destroy(void* pool) {
    [(NSAutoreleasePool*)pool release];
}

void* CSIOSAutoreleasePool::drain(void* pool) {
    [(NSAutoreleasePool*)pool release];
    return [[NSAutoreleasePool alloc] init];
}
