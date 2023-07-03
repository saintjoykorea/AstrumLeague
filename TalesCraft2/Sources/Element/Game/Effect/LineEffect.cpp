//
//  LineEffect.cpp
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define GameImpl

#include "LineEffect.h"

#include "Map.h"

#define EffectDuration  1.0f

LineEffect::LineEffect(const Object* src, const Object* dest, const ImageIndex& image, const CSColor& color, EffectDisplay display, int tag) : Effect(dest, tag), _src(retain(src)), _image(image), _color(color), _display(display) {

}

LineEffect::~LineEffect() {
    release(_src);
}

LineEffect* LineEffect::effect(const Object* src, const Object* dest, const ImageIndex& image, const CSColor& color, EffectDisplay display, int tag) {
    LineEffect* effect = new LineEffect(src, dest, image, color, display, tag);
    effect->destination()->registerEffect(effect);
    effect->release();
    return effect;
}

void LineEffect::dispose() {
    synchronized(this, GameLockEffect) {
        release(_src);
    }
    Effect::dispose();
}

void LineEffect::update() {
    switch (_display) {
        case EffectDisplayBlink:
            {
                int duration = EffectDuration * Map::sharedMap()->framePerSecond();
                int frame = Map::sharedMap()->frame() % duration;
                if (frame > duration / 2) {
                    frame = duration - frame;
                }
                _progress = EffectDuration * frame / duration;
            }
            break;
        case EffectDisplayVanish:
            _progress = CSMath::min(_progress + Map::sharedMap()->frameDelayFloat(), EffectDuration);
            if (_progress == EffectDuration) {
                synchronized(this, GameLockEffect) {
                    release(_src);
                    release(_dest);
                }
            }
            break;
    }
}

void LineEffect::draw(CSGraphics* graphics, ObjectLayer layer, bool cover) {
    if (layer == ObjectLayerBottom && cover) {
        const Object* src = NULL;
        const Object* dest = NULL;
        synchronized(this, GameLockEffect) {
            src = retain(_src);
            dest = retain(_dest);
        }
        if (src && dest && src != dest) {
            CSVector3 p0 = src->worldPoint(ObjectLayerBottom);
            CSVector3 p1 = dest->worldPoint(ObjectLayerBottom);
            
            int strokeWidth = graphics->strokeWidth();
            float brightness = graphics->brightness();
            CSColor color = graphics->color();
            
            CSColor lineColor = _color;
            lineColor.a *= (float)(EffectDuration - _progress) / EffectDuration;
            
            graphics->setStrokeWidth(0);
            graphics->setBrightness(1.0f);
            graphics->setColor(lineColor);
            if (_image) {
                graphics->drawLineImage(Asset::sharedAsset()->image(_image), Map::sharedMap()->frame(), p0, p1);
            }
            else {
                graphics->drawLine(p0, p1);
            }
            graphics->setStrokeWidth(strokeWidth);
            graphics->setBrightness(brightness, false);
            graphics->setColor(color, false);
        }
        
        if (src) src->release();
        if (dest) dest->release();
    }
}
