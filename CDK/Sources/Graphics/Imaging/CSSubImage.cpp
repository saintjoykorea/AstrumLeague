//
//  CSSubImage.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 6..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSSubImage.h"

#include "CSBuffer.h"

#include "CSLocaleString.h"

CSSubImage::CSSubImage(const CSImage* parent, const CSRect& frame) {
    CSAssert(parent, "parent is null");
    
    _parent = retain(parent);
    
    _frame = frame;
    _frame.origin += parent->frame().origin;
    _frame.intersect(parent->frame());
}

CSSubImage::CSSubImage(CSBuffer* buffer, const CSImage* parent) {
    CSAssert(parent, "parent is null");
    
    _parent = retain(parent);
    
	_frame.origin.x = buffer->readShort();
	_frame.origin.y = buffer->readShort();
	_frame.size.width = buffer->readShort();
	_frame.size.height = buffer->readShort();
    
    int localeCount = buffer->readLength();
    if (localeCount) {
        _localeFrames = new CSDictionary<CSString, CSRect>();
        for (int i = 0; i < localeCount; i++) {
            CSArray<CSString>* locales = buffer->readArrayFunc<CSString>(&CSString::stringWithBuffer);
			CSRect frame;
			frame.origin.x = buffer->readShort();
			frame.origin.y = buffer->readShort();
			frame.size.width = buffer->readShort();
			frame.size.height = buffer->readShort();
            
            foreach(const CSString*, locale, locales) {
                _localeFrames->setObject(locale, frame);
            }
        }
    }
}

CSSubImage::~CSSubImage() {
    _parent->release();
    
    release(_localeFrames);
}

CSSubImage* CSSubImage::imageWithBuffer(CSBuffer* buffer, const CSImage* parent) {
    return autorelease(new CSSubImage(buffer, parent));
}

const CSRect& CSSubImage::frame() const {
    if (_localeFrames) {
        const CSString* locale = CSLocaleString::locale();
        if (locale) {
            const CSRect* frame = _localeFrames->tryGetObjectForKey(locale);
            if (frame) return *frame;
        }
    }
    return _frame;
}
