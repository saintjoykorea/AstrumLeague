//
//  CSImage.cpp
//  CDK
//
//  Created by 김찬 on 2016. 2. 11..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSImage.h"

#include "CSBuffer.h"

#include "CSRootImage.h"
#include "CSSubImage.h"

CSImage* CSImage::rootImageWithBuffer(CSBuffer* buffer) {
    return CSRootImage::imageWithBuffer(buffer);
}
CSImage* CSImage::subImageWithBuffer(CSBuffer* buffer, const CSImage* parent) {
    return CSSubImage::imageWithBuffer(buffer, parent);
}

