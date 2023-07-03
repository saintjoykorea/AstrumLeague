//
//  CSFont.h
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSFont__
#define __CDK__CSFont__

#include "CSString.h"

enum CSFontStyle {
    CSFontStyleNormal,
    CSFontStyleBold,
    CSFontStyleItalic,
    CSFontStyleMedium = 4,         //extension from here
    CSFontStyleSemiBold
};

class CSFont : public CSObject {
private:
    void* _handle;
    const CSString* _name;
    float _size;
    CSFontStyle _style;
public:
    CSFont(float size, CSFontStyle style);
    CSFont(const char* name, float size, CSFontStyle style);
private:
    ~CSFont();
public:
	static void load(const char* name, CSFontStyle style, const char* path);

    static CSFont* systemFont(float size, CSFontStyle style);
    static CSFont* font(const char* name, float size, CSFontStyle style);
    
    inline static CSFont* systemFontWithSize(float size) {
        return systemFont(size, CSFontStyleNormal);
    }
    inline static CSFont* fontWithSize(const char* name, float size) {
        return font(name, size, CSFontStyleNormal);
    }
    
    inline void* handle() const {
        return _handle;
    }
    
    inline const CSString* name() const {
        return _name;
    }
    
    inline float size() const {
        return _size;
    }
    
    inline CSFontStyle style() const {
        return _style;
    }
    
    CSFont* derivedFont(float size, CSFontStyle style) const;
    
    inline CSFont* derivedFontWithSize(float size) const {
        return derivedFont(size, _style);
    }
    inline CSFont* derivedFontWithStyle(CSFontStyle style) const {
        return derivedFont(_size, style);
    }
    uint hash() const override;
    bool isEqualToFont(const CSFont* font) const;
    bool isEqual(const CSObject* object) const override;
};

#endif /* defined(__CDK__CSFont__) */
