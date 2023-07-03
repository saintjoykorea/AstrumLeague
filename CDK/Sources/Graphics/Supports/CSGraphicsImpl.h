//
//  CSGraphicsImpl.h
//  CDK
//
//  Created by 김찬 on 12. 8. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSGraphicsImpl__
#define __CDK__CSGraphicsImpl__

#include "CSRenderer.h"
#include "CSCharacters.h"
#include "CSFillShader.h"
#include "CSShockwaveShader.h"
#include "CSLensShader.h"
#include "CSSwirlShader.h"
#include "CSPixellateShader.h"
#include "CSKuwaharaShader.h"
#include "CSBlurShader.h"

class CSGraphicsImpl {
private:
    CSRenderer* _renderer;
    CSCharacters* _characters;
    CSFillShader* _fillShader;
    CSShockwaveShader* _shockwaveShader;
    CSLensShader* _lensShader;
    CSSwirlShader* _swirlShader;
    CSPixellateShader* _pixellateShader;
    CSKuwaharaShader* _kuwaharaShader;
    CSBlurShader* _blurShader;
    const CSFont* _defaultFont;
    const CSRootImage* _particleImage;
    
    static CSGraphicsImpl* __impl;
private:
    CSGraphicsImpl();
    ~CSGraphicsImpl();
public:

    static inline CSGraphicsImpl* sharedImpl() {
        CSAssert(__impl, "CSGraphicsImpl is NULL");
        return __impl;
    }
    
    static void initialize();
    static void finalize();
    
    void reload();
    void purge();
    
    inline CSRenderer* renderer() {
        return _renderer;
    }
    
    inline CSCharacters* characters() {
        return _characters;
    }
    
    inline CSFillShader* fillShader() {
        return _fillShader;
    }
    
    CSShockwaveShader* shockwaveShader();
    CSLensShader* lensShader();
    CSSwirlShader* swirlShader();
    CSPixellateShader* pixellateShader();
    CSKuwaharaShader* kuwaharaShader();
    CSBlurShader* blurShader();
    
    inline const CSFont* defaultFont() const {
        return _defaultFont;
    }
    
    inline void setDefaultFont(const CSFont* font) {
        CSObject::retain(_defaultFont, font);
    }
    
    const CSRootImage* particleImage();
};

#endif /* defined(__CDK__CSGraphicsImpl__) */

#endif

