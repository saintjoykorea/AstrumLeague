//
//  CSCharacters.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 9..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSCharacters.h"

#include "CSOpenGL.h"
#include "CSSubImage.h"
#include "CSRaster.h"

#include "icu_compat.h"

#define CSCharactersTextureSize     2048
#define CSCharacterPadding          1

CSCharacters::CharKey::CharKey(const uchar* cc, uint cclen) : _hash(0) {
    u_strncpy(_cc, cc, cclen);
    _cc[cclen] = 0;
    for (int i = 0; i < cclen; i++) _hash += _cc[i];
}

CSCharacters::CharKey::CharKey(const CharKey& other) : _hash(other._hash) {
    u_strcpy(_cc, other._cc);
}

bool CSCharacters::CharKey::operator ==(const CharKey& other) const {
    return u_strcmp(_cc, other._cc) == 0;
}

typename CSCharacters::CharKey& CSCharacters::CharKey::operator =(const CharKey& other) {
    u_strcpy(_cc, other._cc);
    return *this;
}

CSCharacters::FontCharKey::FontCharKey(const uchar* cc, uint cclen, const CSFont* font) :
    CharKey(cc, cclen),
    _name(CSObject::retain(font->name())),
    _style(font->style()),
    _size(font->size())
{
    _hash += (_name ? _name->hash() : 0) + _style + (uint)_size;
}

CSCharacters::FontCharKey::FontCharKey(const FontCharKey& other) : 
    CharKey(other),
    _name(CSObject::retain(other._name)),
    _style(other._style),
    _size(other._size)
{
    
}

CSCharacters::FontCharKey::~FontCharKey() {
    CSObject::release(_name);
}

bool CSCharacters::FontCharKey::operator ==(const FontCharKey& other) const {
    return CSString::isEqual(_name, other._name) && _style == other._style && _size == other._size && CharKey::operator==(other);
}

typename CSCharacters::FontCharKey& CSCharacters::FontCharKey::operator =(const FontCharKey& other) {
    CSObject::retain(_name, other._name);
    _style = other._style;
    _size = other._size;
    CharKey::operator=(other);
    return *this;
}

CSCharacters::FontImage::FontImage(const CSImage* image, float offy) : image(CSObject::retain(image)), offy(offy) {

}

CSCharacters::FontImage::~FontImage() {
    image->release();
}

//============================================================================================================================================================
CSCharacters::Texture::Texture() : roots(new CSArray<CSRootImage>()), x(0), y(0), h(0) {
    
}

CSCharacters::Texture::~Texture() {
    roots->release();
}

void CSCharacters::Texture::clear() {
    roots->removeAllObjects();
    x = 0;
    y = 0;
    h = 0;
}

CSCharacters::CSCharacters() {
    _images = new CSDictionary<FontCharKey, FontImage>(2048);
    _sizes = new CSDictionary<FontCharKey, CSSize>(2048);
    _vacants = new CSSet<FontCharKey>();
    _directions = new CSDictionary<CharKey, UBiDiDirection>(2048);

    //=====================================================================
    //TODO:RTL언어에서 숫자표기시 NEUTRAL로 처리되어 표시순서가 바뀌는 경우 발생
    static_assert(sizeof(uchar) == sizeof(char16_t), "invalid uchar size");

    _directions->setObject(CharKey((const uchar*)u"0", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"1", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"2", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"3", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"4", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"5", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"6", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"7", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"8", 1), UBIDI_LTR);
    _directions->setObject(CharKey((const uchar*)u"9", 1), UBIDI_LTR);
    //=====================================================================
}

CSCharacters::~CSCharacters() {
    _images->release();
    _sizes->release();
    _vacants->release();
    _directions->release();
}

void CSCharacters::clear() {
    _images->removeAllObjects();
    _textures[0].clear();
    _textures[1].clear();
}

const CSSize& CSCharacters::size(const uchar* cc, uint cclen, const CSFont* font) {
	if (cclen > CSMaxComposedCharLength) return CSSize::Zero;

    FontCharKey key(cc, cclen, font);
    
    const CSSize* size = _sizes->tryGetObjectForKey(key);
    
    return size ? *size : (_sizes->setObject(key) = CSRaster::characterSize(cc, cclen, font));
}

UBiDiDirection CSCharacters::direction(const uchar* cc, uint cclen) {
    if (cclen > CSMaxComposedCharLength) return UBIDI_NEUTRAL;

    CharKey key(cc, cclen);

    UBiDiDirection* dir = _directions->tryGetObjectForKey(key);

    return dir ? *dir : (_directions->setObject(key) = ucompat_getBaseDirection(cc, cclen));
}

bool CSCharacters::addRoot(bool color) {
    void* emptyRaster = calloc(CSCharactersTextureSize * CSCharactersTextureSize, color ? 4 : 1);
    if (!emptyRaster) return false;
    CSRootImage* root = CSRootImage::createWithRaw(emptyRaster, color ? CSImageFormatRawRGBA8888 : CSImageFormatRawA8, CSCharactersTextureSize, CSCharactersTextureSize);
    free(emptyRaster);
    if (!root) return false;
    _textures[color].roots->addObject(root);
    root->release();
    
    CSLog("image added(%s):%d / %d", color ? "color" : "grayscale", _textures[0].roots->count(), _textures[1].roots->count());
    
    return true;
}

const CSImage* CSCharacters::image(const uchar* cc, uint cclen, const CSFont* font, float& offy) {
	if (cclen > CSMaxComposedCharLength) return NULL;

    FontCharKey key(cc, cclen, font);

    if (_vacants->containsObject(key)) return NULL;

    FontImage* e = _images->tryGetObjectForKey(key);
    
    if (e) {
        offy = e->offy;
        return e->image;
    }

    if (!u_isgraph(CSString::toCodePoint(cc))) {
        _vacants->addObject(key);
        return NULL;
    }
    uint w = 0;
    uint h = 0;
    void* data = CSRaster::createRasterFromCharacter(cc, cclen, font, w, h, offy);        //premultiplied alpha image
    if (!data) {
        if (w == 0 || h == 0) _vacants->addObject(key);
        return NULL;
    }
        
    bool color = false;
    {
        uint len = w * h;
        for (uint i = 0; i < len; i++) {
            byte* components = &((byte*)data)[i * 4];
            byte a = components[3];
            if (components[0] != a || components[1] != a || components[2] != a) {
                color = true;
                break;
            }
        }
            
        //premultiplied 지만 반투명 글자가 없다고 생각하고 무시
            
        if (!color) {
            byte* temp = (byte*)fmalloc(len);
            if (!temp) {
                free(data);
                return NULL;
            }
            for (uint i = 0; i < len; i++) {
                temp[i] = ((byte*)data)[i * 4 + 3];
            }
            free(data);
            data = temp;
        }
    }
        
    Texture& t = _textures[color];
        
    if (t.x + w > CSCharactersTextureSize) {
        t.y += t.h + CSCharacterPadding;
        t.x = 0;
        t.h = 0;
    }
    if (t.y + h > CSCharactersTextureSize) {
        if (!addRoot(color)) {
            free(data);
            return NULL;
        }
        t.x = 0;
        t.y = 0;
        t.h = 0;
    }
    else if (!t.roots->count()) {
        if (!addRoot(color)) {
            free(data);
            return NULL;
        }
    }
        
    CSRootImage* root = t.roots->lastObject();
        
    glActiveTextureCS(GL_TEXTURE0);
    glPixelStoreiCS(GL_UNPACK_ALIGNMENT, 1);
    glBindTextureCS(GL_TEXTURE_2D, root->texture0());
    glTexSubImage2DCS(GL_TEXTURE_2D, 0, t.x, t.y, w, h, color ? GL_RGBA : GL_RED, GL_UNSIGNED_BYTE, data);
        
    free(data);

    const CSImage* image = CSSubImage::image(root, CSRect(t.x, t.y, w, h));
        
    new (&_images->setObject(key)) FontImage(image, offy);
        
    if (t.h < h) {
        t.h = h;
    }
        
    t.x += w + CSCharacterPadding;

    return image;
}

