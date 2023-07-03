//
//  CSTrail.cpp
//  CDK
//
//  Created by 김찬 on 2015. 11. 25..
//  Copyright © 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTrail.h"

#include "CSBuffer.h"

#include "CSRootImage.h"

#include "CSAnimation.h"

#include "CSOpenGL.h"

#include "CSDiagnostics.h"

#define MaxPoints       2048

CSTrail::Point::Point(const CSVector3& point, float trailProgress, float animationProgress, int link) : point(point), trailProgress(trailProgress), animationProgress(animationProgress), link(link) {

}

CSTrail::CSTrail() : _points(new CSArray<Point>()), _emitting(true) {

}

CSTrail::CSTrail(const CSTrail* other, bool capture) :
    CSAnimationSubstance(other),
    _blendMode(other->_blendMode),
    _imageIndices(retain(other->_imageIndices)),
    _material(other->_material),
    _localSpace(other->_localSpace),
	_billboard(other->_billboard),
    _repeat(other->_repeat),
    _repeatScroll(other->_repeatScroll),
    _minDistance(other->_minDistance),
    _pointLife(other->_pointLife),
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
        _points = new CSArray<Point>(other->_points, true);
        _progress = other->_progress;
        _emitting = other->_emitting;
        _link = other->_link;
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
        _points = new CSArray<Point>();
        _emitting = true;
    }
}

CSTrail::CSTrail(CSBuffer* buffer) {
    _blendMode = (CSBlendMode)buffer->readByte();
    _imageIndices = retain(buffer->readArray<ushort, 2>());
    _material = CSMaterial(buffer);
    _localSpace = buffer->readBoolean();
	_billboard = buffer->readBoolean();
    _repeat = buffer->readBoolean();
    _repeatScroll = buffer->readFloat();
    _minDistance = buffer->readFloat();
    _pointLife = buffer->readFloat();
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
    _points = new CSArray<Point>();
    _emitting = true;
    
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

CSTrail::~CSTrail() {
    release(_imageIndices);
    release(_colorRed);
    release(_colorGreen);
    release(_colorBlue);
    release(_colorAlpha);
    _points->release();
}

void CSTrail::setColor(const CSMovement* red, const CSMovement* green, const CSMovement* blue, const CSMovement* alpha, bool fixedComponent, float duration, const CSAnimationLoop& loop) {
    retain(_colorRed, red);
    retain(_colorGreen, green);
    retain(_colorBlue, blue);
    retain(_colorAlpha, alpha);
    _colorFixedComponent = fixedComponent;
    _colorDuration = duration;
    _colorLoop = loop;
}

void CSTrail::addPoint(const CSVector3 &p, float trailProgress, float animationProgress, float distance) {
    if (_points->count() < MaxPoints) {
        if (distance && _points->count()) {
            const Point& lp = _points->lastObject();
            
            if (lp.point.distanceSquared(p) < distance * distance) {
                return;
            }
        }
        bool replace;
        if (_points->count() > 1) {
            const Point& p0 = _points->lastObject();
            const Point& p1 = _points->objectAtIndex(_points->count() - 2);
            replace = p0.link != _link && p0.link != p1.link;
        }
        else if (_points->count() == 1) {
            const Point& p0 = _points->lastObject();
            replace = p0.link != _link;
        }
        else {
            replace = false;
        }
        new (replace ? &_points->lastObject() : &_points->addObject()) Point(p, trailProgress, animationProgress, _link);
    }
}

float CSTrail::duration(const CSCamera *camera, CSAnimationDuration type, float duration) const {
    if (_colorDuration) {
        float d = _colorLoop.getDuration(_colorDuration);
        if (duration < d) {
            duration = d;
        }
    }
    if (_imageInterval && _imageIndices && _imageIndices->count()) {
        float d = _imageLoop.getDuration(_imageInterval * _imageIndices->count());
        if (duration < d) {
            duration = d;
        }
    }
    if (duration) {
        switch (type) {
            case CSAnimationDurationAvg:
                duration += _pointLife * 0.5f;
                break;
            case CSAnimationDurationMax:
                duration += _pointLife;
                break;
        }
    }
    return duration;
}

bool CSTrail::isFinite(bool finite) const {
	if ((_colorDuration && _colorLoop.finish) || (_imageInterval && _imageIndices && _imageIndices->count() && _imageLoop.finish)) {
		finite = true;
	}
	return finite;
}

void CSTrail::rewind() {
    _points->removeAllObjects();
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
    _emitting = true;
    _link = 0;
}

void CSTrail::addBreak(float progress, float delta, float duration, const CSAnimationLoop& loop, CSArray<BreakEntry>*& breaks) {
    if (duration) {
        int step0 = CSMath::ceil(progress / duration);
        int step1 = CSMath::ceil((progress - delta) / duration);
        
        if (step0 != step1 && (loop.count == 0 || step1 <= loop.count)) {
            if (!breaks) breaks = new CSArray<BreakEntry>(1);
            BreakEntry entry;
            entry.delta = progress - (int)(progress / duration) * duration;
            
            if (loop.count == 0 || step0 <= loop.count) {
                entry.type = loop.roundTrip ? BreakEntryTypeReverse : BreakEntryTypeRewind;
            }
            else {
                entry.type = BreakEntryTypeNone;
            }

            uint i = 0;
            while (i < breaks->count()) {
                if (entry.delta > breaks->objectAtIndex(i).delta) {
                    break;
                }
                i++;
            }
            breaks->insertObject(i, entry);
        }
    }
}
CSAnimationState CSTrail::update(float delta, const CSCamera* camera, bool alive) {
    _progress += delta;
    
    if (_parent) {
        CSMatrix capture;
        
        if (_localSpace) {
            foreach(Point&, p, _points) {
                if (_parent->capture(p.animationProgress, camera, capture)) {
                    p.point = capture.translationVector();
                }
            }
        }
        if (_emitting) {
            CSAnimation* current = _parent;
            
            CSArray<BreakEntry>* breaks = NULL;
            
            do {
                addBreak(current->progress(), delta, current->positionDuration(camera), current->positionLoop(), breaks);
                addBreak(current->progress(), delta, current->rotationDuration(), current->rotationLoop(), breaks);
                addBreak(current->progress(), delta, current->scaleDuration(), current->scaleLoop(), breaks);
                if (current->global(!current->reverse())) {
                    break;
                }
                current = current->parent();
            } while (current);
            
            if (breaks) {
                foreach(const BreakEntry&, entry, breaks) {
                    float animationProgress;
                    
                    if (entry.type == BreakEntryTypeRewind) {
                        animationProgress = _parent->progress() - entry.delta - 0.0001f;
                        
                        if (_parent->capture(animationProgress, camera, capture)) {
                            addPoint(capture.translationVector(), _progress - entry.delta - 0.0001f, animationProgress, 1);
                            _link++;
                        }
                    }
                    
                    animationProgress = _parent->progress() - entry.delta;
                    
                    if (_parent->capture(animationProgress, camera, capture)) {
                        addPoint(capture.translationVector(), _progress - entry.delta, animationProgress, 0);
                        
                        if (entry.type == BreakEntryTypeReverse) {
                            _link++;
                            addPoint(capture.translationVector(), _progress - entry.delta, animationProgress, 0);
                        }
                    }
                }
                breaks->release();
            }
            if (_parent->capture(_parent->progress(), camera, capture)) {
                addPoint(capture.translationVector(), _progress, _parent->progress(), _minDistance);
            }
        }
    }
    
    bool remaining = false;
    
    if (_colorDuration) {
        _colorLoop.getState(_progress / _colorDuration, remaining, alive);
    }
    if (_imageInterval && _imageIndices && _imageIndices->count()) {
        _imageLoop.getState(_progress / (_imageInterval * _imageIndices->count()), remaining, alive);
    }

    _emitting = alive || remaining;
    
    if (_pointLife) {
        while (_points->count() && _points->objectAtIndex(0).trailProgress <= _progress - _pointLife) {
            _points->removeObjectAtIndex(0);
        }
        if (_points->count()) {
            remaining = true;
        }
    }
    else {
        if (!alive) {
            _points->removeAllObjects();
        }
    }
    if (alive) return CSAnimationStateAlive;
    if (remaining) return CSAnimationStateFinishing;
    return _pointLife ? CSAnimationStateStopped : CSAnimationStateNone;
}

void CSTrail::draw(CSGraphics* graphics, bool shadow) {
    if (!shadow && _resourceDelegate && _imageIndices && _imageIndices->count() && _points->count() >= 2) {
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
                
                int s = 0;
                
                float d = _repeatScroll * _progress;
                
                while (s < _points->count()) {
                    int e = s;
                    int link = _points->objectAtIndex(s).link;
                    while (e < _points->count() - 1 && _points->objectAtIndex(e + 1).link == link) {
                        e++;
                    }
                    if (e > s) {
                        graphics->begin();
                        
                        int vi = 0;
                        for (int i = s; i < e; i++) {
                            graphics->addIndex(vi + 0);
                            graphics->addIndex(vi + 2);
                            graphics->addIndex(vi + 1);
                            graphics->addIndex(vi + 2);
                            graphics->addIndex(vi + 3);
                            graphics->addIndex(vi + 1);
                            vi += 2;
                        }
                        for (int i = s; i <= e; i++) {
                            const CSVector3& p0 = _points->objectAtIndex(i == e ? i - 1 : i).point;
                            const CSVector3& p1 = _points->objectAtIndex(i != e ? i + 1 : i).point;
                            CSVector3 pr = CSVector3::normalize(p1 - p0);
                            CSVector3 pu = CSVector3::normalize(CSVector3::cross(pf, pr));
                            
                            float u = (_repeat ? d : frame.origin.x + CSMath::clamp(frame.size.width * (i - s) / (e - s), 1.0f, frame.size.width - 1.0f)) / textureSize.width;
                            
                            const CSVector3& p = _points->objectAtIndex(i).point;
                            graphics->addVertex(CSVertex(p + pu * h, CSVector2(u, tv), pf));
                            graphics->addVertex(CSVertex(p - pu * h, CSVector2(u, bv), pf));
                            
                            if (_repeat && i < e) {
                                d += _points->objectAtIndex(i).point.distance(_points->objectAtIndex(i + 1).point);
                            }
                        }
                        graphics->end(image->root(), GL_TRIANGLES);
                    }
                    s = e + 1;
                }
                graphics->pop();
            }
        }
    }
}

void CSTrail::preload(CSResourceDelegate* delegate) const {
    if (!delegate) delegate = _resourceDelegate;
    if (delegate) {
        foreach(const CSArray<ushort>*, imageIndices, _imageIndices) {
            (*delegate)(CSResourceTypeImage, imageIndices->pointer(), imageIndices->count());
        }
    }
}
