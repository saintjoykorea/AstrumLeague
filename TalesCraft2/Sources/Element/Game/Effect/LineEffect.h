//
//  LineEffect.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef LineEffect_h
#define LineEffect_h

#include "Effect.h"

class LineEffect : public Effect {
private:
    const Object* _src;
    ImageIndex _image;
    CSColor _color;
    EffectDisplay _display;
    float _progress;

    LineEffect(const Object* src, const Object* dest, const ImageIndex& image, const CSColor& color, EffectDisplay display, int tag = 0);
    ~LineEffect();
public:
    static LineEffect* effect(const Object* src, const Object* dest, const ImageIndex& image, const CSColor& color, EffectDisplay display, int tag = 0);
    
	inline EffectType type() const override {
		return EffectTypeLine;
	}

    inline const Object* source() const  override{
        return _src;
    }
    
    void dispose() override;
    void update() override;
    void draw(CSGraphics* graphics, ObjectLayer layer, bool cover) override;
};

#endif /* LineEffect_h */
