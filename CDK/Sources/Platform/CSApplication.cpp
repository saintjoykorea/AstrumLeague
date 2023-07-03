//
//  CSApplication.cpp
//  CDK
//
//  Created by 김찬 on 14. 1. 9..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSApplication.h"

CSApplication CSApplication::__application;

CSApplication::CSApplication() : _glContextIdentifier(1), _version(NULL) {
    
}
CSApplication::~CSApplication() {
    CSObject::release(_version);
}

