//
//  CSChain.cpp
//  CDK
//
//  Created by 김찬 on 2015. 11. 25..
//  Copyright © 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSChain.h"

#include "CSBuffer.h"

#include "CSRootImage.h"

#include "CSAnimation.h"

#include "CSOpenGL.h"

#include "CSDiagnostics.h"

CSChain::CSChain() {
    
}

CSChain::CSChain(const CSChain* other, bool capture) :
    CSAnimationSubstance(other),
    _blendMode(other->_blendMode),
    _imageIndices(retain(other->_imageIndices)),
    _material(other->_material),
	_billboard(other->_billboard),
    _repeat(other->_repeat),
    _repeatScroll(other->_repeatScroll),
    _width(other->_width),
    _imageInterval(other->_imageInterval),
    _imageLoop(other->_imageLoop),
    _colorRed(retain(other->_colorRed)),
    _colorGreen(retain(other->_colorGreen)),
    _colorBlue(retain(other->_colorBlue)),
    _colorAlpha(retain(other->_colorAlpha)),
    _colorFixedComponent(other->_colorFixedComponent),
    _colorDuration(other->_colorDuration),
    _colorLoop(other->_colorLoop)
{
    if (capture) {
        _colorRandom = other->_colorRandom;
        _progress = other->_progress;
    }
    else {
        if (_colorFixedComponent) {
            _colorRandom.r = _colorRandom.g = _colorRandom.b = _colorRandom.a = randFloat(0.0f, 1.0f);
        }
        else {
            _colorRandom.r = randFloat(0.0f, 1.0f);
            _colorRandom.g = randFloat(0.0f, 1.0f);
            _colorRandom.b = randFloat(0.0f, 1.0f);
            _colorRandom.a = randFloat(0.0f, 1.0f);
        }
    }
}

CSChain::CSChain(CSBuffer* buffer) {
    _blendMode = (CSBlendMode)buffer->readByte();
    _imageIndices = retain(buffer->readArray<ushort, 2>());
    _material = CSMaterial(buffer);
	_billboard = buffer->readBoolean();
    _repeat = buffer->readBoolean();
    _repeatScroll = buffer->readFloat();
    _width = buffer->readFloat();
    _imageInterval = buffer->readFloat();
    new (&_imageLoop) CSAnimationLoop(buffer);
    _colorRed = CSMovement::createWithBuffer(buffer);
    _colorGreen = CSMovement::createWithBuffer(buffer);
    _colorBlue = CSMovement::createWithBuffer(buffer);
    _colorAlpha = CSMovement::createWithBuffer(buffer);
    _colorFixedComponent = buffer->readBoolean();
    _colorDuration = buffer->readFloat();
    new (&_colorLoop) CSAnimationLoop(buffer);
    
    if (_colorFixedComponent) {
        _colorRandom.r = _colorRandom.g = _colorRandom.b = _colorRandom.a = randFloat(0.0f, 1.0f);
    }
    else {
        _colorRandom.r = randFloat(0.0f, 1.0f);
        _colorRandom.g = randFloat(0.0f, 1.0f);
        _colorRandom.b = randFloat(0.0f, 1.0f);
        _colorRandom.a = randFloat(0.0f, 1.0f);
    }
}

CSChain::~CSChain() {
    release(_imageIndices);
    release(_colorRed);
    release(_colorGreen);
    release(_colorBlue);
    release(_colorAlpha);
}

void CSChain::setColor(const CSMovement* red, const CSMovement* green, const CSMovement* blue, const CSMovement* alpha, bool fixedComponent, float duration, const CSAnimationLoop& loop) {
    retain(_colorRed, red);
    retain(_colorGreen, green);
    retain(_colorBlue, blue);
    retain(_colorAlpha, alpha);
    _colorDuration = duration;
    _colorFixedComponent = fixedComponent;
    _colorLoop = loop;
}

float CSChain::duration(const CSCamera *camera, CSAnimationDuration type, float duration) const {
	duration = 0.0f;
	if (_colorDuration) {
        duration = _colorLoop.getDuration(_colorDuration);
    }
    if (_imageInterval && _imageIndices && _imageIndices->count()) {
        float d = _imageLoop.getDuration(_imageInterval * _imageIndices->count());
        if (duration < d) {
            duration = d;
        }
    }
    return duration;
}

bool CSChain::isFinite(bool finite) const {
	if ((_colorDuration && _colorLoop.finish) || (_imageInterval && _imageIndices && _imageIndices->count() && _imageLoop.finish)) {
		finite = true;
	}
	return finite;
}

void CSChain::rewind() {
    _progress = 0.0f;
    
    if (_colorFixedComponent) {
        _colorRandom.r = _colorRandom.g = _colorRandom.b = _colorRandom.a = randFloat(0.0f, 1.0f);
    }
    else {
        _colorRandom.r = randFloat(0.0f, 1.0f);
        _colorRandom.g = randFloat(0.0f, 1.0f);
        _colorRandom.b = randFloat(0.0f, 1.0f);
        _colorRandom.a = randFloat(0.0f, 1.0f);
    }
}

CSAnimationState CSChain::update(float delta, const CSCamera* camera, bool alive) {
    _progress += delta;
    
    if (_parent) {
        _visible = false;
        
        CSMatrix capture;
        
        if (_parent->capture(_parent->progress() - _progress, camera, capture)) {
            _startPoint = capture.translationVector();
            
            if (_parent->capture(_parent->progress(), camera, capture)) {
                _endPoint = capture.translationVector();
                
                _visible = true;
            }
        }
    }
    else {
        _visible = true;
    }

    bool remaining = false;
    
    if (_colorDuration) {
        _colorLoop.getState(_progress / _colorDuration, remaining, alive);
    }
    if (_imageInterval && _imageIndices && _imageIndices->count()) {
        _imageLoop.getState(_progress / (_imageInterval * _imageIndices->count()), remaining, alive);
    }
    
    if (alive) return CSAnimationStateAlive;
    if (remaining) return CSAnimationStateFinishing;
    return CSAnimationStateNone;
}

void CSChain::draw(CSGraphics* graphics, bool shadow) {
    if (!shadow && _visible && _startPoint != _endPoint && _resourceDelegate && _imageIndices && _imageIndices->count()) {
        int imageIndex = _imageInterval ? _imageLoop.getProgress(_progress / (_imageInterval * _imageIndices->count()), NULL, NULL) * (_imageIndices->count() - 1) : 0;
        
        const CSArray<ushort>* imageIndices = _imageIndices->objectAtIndex(imageIndex);
        
        const CSImage* image = static_cast<const CSImage*>((*_resourceDelegate)(CSResourceTypeImage, imageIndices->pointer(), imageIndices->count()));
        
        if (image) {
            const CSRect& frame = image->frame();
            
            if (frame.size.width >= 2) {
                const CSSize& textureSize = image->textureSize();
                
                float h = (_width ? _width : frame.size.height) * 0.5f;
                
                graphics->push();
                graphics->setCullMode(CSCullNone);
                graphics->setBlendMode(_blendMode);
                graphics->setStrokeWidth(0);
                graphics->setMaterial(_material);
                
                float colorProgress = _colorDuration ? _colorLoop.getProgress(_progress / _colorDuration, NULL, NULL) : 1.0f;
                
                graphics->setColor(CSColor(_colorRed ? CSMath::clamp(_colorRed->value(colorProgress, _colorRandom.r), 0.0f, 1.0f) : 1.0f,
                                           _colorGreen ? CSMath::clamp(_colorGreen->value(colorProgress, _colorRandom.g), 0.0f, 1.0f) : 1.0f,
                                           _colorBlue ? CSMath::clamp(_colorBlue->value(colorProgress, _colorRandom.b), 0.0f, 1.0f) : 1.0f,
                                           _colorAlpha ? CSMath::clamp(_colorAlpha->value(colorProgress, _colorRandom.a), 0.0f, 1.0f) : 1.0f));
                
				CSVector3 pf = _billboard ? -graphics->camera().forward() : CSVector3::UnitZ;
                
                float tv = frame.top() / textureSize.height;
                float bv = frame.bottom() / textureSize.height;
                
                graphics->begin();
                graphics->addIndex(0);
                graphics->addIndex(2);
                graphics->addIndex(1);
                graphics->addIndex(2);
                graphics->addIndex(3);
                graphics->addIndex(1);
                
                CSVector3 pr = CSVector3::normalize(_endPoint - _startPoint);
                CSVector3 pu = CSVector3::normalize(CSVector3::cross(pf, pr));
                
                float su, eu;
                if (_repeat) {
                    float d = _repeatScroll * _progress;
                    su = d / textureSize.width;
                    eu = (d + _endPoint.distance(_startPoint)) / textureSize.width;
                }
                else {
                    su = (frame.left() + 1) / textureSize.width;
                    eu = (frame.right() - 1) / textureSize.width;
                }
                graphics->addVertex(CSVertex(_startPoint + pu * h, CSVector2(su, tv), pf));
                graphics->addVertex(CSVertex(_startPoint - pu * h, CSVector2(su, bv), pf));
                graphics->addVertex(CSVertex(_endPoint + pu * h, CSVector2(eu, tv), pf));
                graphics->addVertex(CSVertex(_endPoint - pu * h, CSVector2(eu, bv), pf));
                graphics->end(image->root(), GL_TRIANGLES);

                graphics->pop();
            }
        }
    }
}

void CSChain::preload(CSResourceDelegate* delegate) const {
    if (!delegate) delegate = _resourceDelegate;
    if (delegate) {
        foreach(const CSArray<ushort>*, imageIndices, _imageIndices) {
            (*delegate)(CSResourceTypeImage, imageIndices->pointer(), imageIndices->count());
        }
    }
}
