//
//  CSCharacters.h
//  CDK
//
//  Created by 김찬 on 12. 8. 9..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSCharacters__
#define __CDK__CSCharacters__

#include "CSDictionary.h"
#include "CSSet.h"
#include "CSRootImage.h"
#include "CSFont.h"

#include <unicode/ubidi.h>

#define CSMaxComposedCharLength		9

class CSCharacters {
private:
    struct CharKey {
    protected:
        uchar _cc[CSMaxComposedCharLength + 1];
        uint _hash;
    public:
        CharKey(const uchar* cc, uint cclen);
        CharKey(const CharKey& other);

        inline operator uint() const {
            return _hash;
        }
        bool operator ==(const CharKey& other) const;
        CharKey& operator =(const CharKey& other);
    };
    struct FontCharKey : public CharKey {
    private:
        const CSString* _name;
        CSFontStyle _style;
        float _size;
    public:
        FontCharKey(const uchar* cc, uint cclen, const CSFont* font);
        FontCharKey(const FontCharKey& other);
        ~FontCharKey();
        
        bool operator ==(const FontCharKey& other) const;
        FontCharKey& operator =(const FontCharKey& other);
    };
    struct FontImage {
        const CSImage* image;
        float offy;

        FontImage(const CSImage* image, float offy);
        ~FontImage();

        bool operator ==(const FontImage& other) const;
        FontImage& operator =(const FontImage& other);
    };
    
    CSDictionary<FontCharKey, FontImage>* _images;
    CSDictionary<FontCharKey, CSSize>* _sizes;
    CSSet<FontCharKey>* _vacants;
    CSDictionary<CharKey, UBiDiDirection>* _directions;
    struct Texture {
        CSArray<CSRootImage>* roots;
        uint x;
        uint y;
        uint h;
        
        Texture();
        ~Texture();
        
        void clear();
    };
    Texture _textures[2];
public:
    CSCharacters();
    ~CSCharacters();
public:
    void clear();
    const CSSize& size(const uchar* cc, uint cclen, const CSFont* font);
    UBiDiDirection direction(const uchar* cc, uint cclen);
private:
    bool addRoot(bool color);
public:
    const CSImage* image(const uchar* cc, uint cclen, const CSFont* font, float& offy);
};

#endif /* defined(__CDK__CSCharacters__) */

#endif
