//
//  CSFont.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSFont.h"

#include "CSRaster.h"

CSFont::CSFont(float size, CSFontStyle style) :
    _handle(CSRaster::createSystemFontHandle(size, style)),
    _size(size),
    _style(style)
{

}

CSFont::CSFont(const char* name, float size, CSFontStyle style) :
    _name(new CSString(name)),
    _size(size),
    _style(style)
{
    _handle = CSRaster::createFontHandle(name, size, style);
}

CSFont::~CSFont() {
    release(_name);
    
    CSRaster::destroyFontHandle(_handle);
}

void CSFont::load(const char* name, CSFontStyle style, const char* path) {
	CSRaster::loadFontHandle(name, style, path);
}

CSFont* CSFont::systemFont(float size, CSFontStyle style) {
    return autorelease(new CSFont(size, style));
}

CSFont* CSFont::font(const char* name, float size, CSFontStyle style) {
    return autorelease(new CSFont(name, size, style));
}

CSFont* CSFont::derivedFont(float size, CSFontStyle style) const {
    return _name ? font(*_name, size, style) : systemFont(size, style);
}

uint CSFont::hash() const {
    return (_name ? _name->hash() : 0) + _size + _style;
}
bool CSFont::isEqualToFont(const CSFont* font) const {
    return CSString::isEqual(_name, font->_name) && _size == font->_size && _style == font->_style;
}
bool CSFont::isEqual(const CSObject* object) const {
    const CSFont* font = dynamic_cast<const CSFont*>(object);
    
    return font && isEqualToFont(font);
}
