//
//  CSChain.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 10. 5..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSChain_h
#define CSChain_h

#include "CSAnimationSubstance.h"

#include "CSMovement.h"

class CSBuffer;

class CSChain : public CSAnimationSubstance {
private:
    CSBlendMode _blendMode;
    const CSArray<ushort, 2>* _imageIndices;
    CSMaterial _material;
	bool _billboard;
    bool _repeat;
    float _repeatScroll;
    float _width;
    float _imageInterval;
    CSAnimationLoop _imageLoop;
    const CSMovement* _colorRed;
    const CSMovement* _colorGreen;
    const CSMovement* _colorBlue;
    const CSMovement* _colorAlpha;
    bool _colorFixedComponent;
    float _colorDuration;
    CSAnimationLoop _colorLoop;
    CSColor _colorRandom;
    
    CSVector3 _startPoint;
    CSVector3 _endPoint;
    float _progress;
    bool _visible;
public:
    CSChain();
    CSChain(const CSChain* other, bool capture);
    CSChain(CSBuffer* buffer);
private:
    ~CSChain();
public:
    static inline CSChain* chain() {
        return autorelease(new CSChain());
    }
    static inline CSChain* chainWithTrail(const CSChain* other, bool capture) {
        return autorelease(new CSChain(other, capture));
    }
    static inline CSChain* chainWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSChain(buffer));
    }
    
    inline CSBlendMode blendMode() const {
        return _blendMode;
    }
    inline void setBlendMode(CSBlendMode blendMode) {
        _blendMode = blendMode;
    }
	inline bool billboard() const {
		return _billboard;
	}
	inline void setBillboard(bool billboard) {
		_billboard = billboard;
	}
    inline bool repeat() const {
        return _repeat;
    }
    inline void setRepeat(bool repeat) {
        _repeat = repeat;
    }
    inline void setRepeatScroll(float repeatScroll) {
        _repeatScroll = repeatScroll;
    }
    inline float width() const {
        return _width;
    }
    inline void setWidth(float width) {
        _width = width;
    }
    inline const CSAnimationLoop& imageLoop() const {
        return _imageLoop;
    }
    inline void setImageInterval(float interval, const CSAnimationLoop& loop) {
        _imageInterval = interval;
        _imageLoop = loop;
    }
    void setColor(const CSMovement* red, const CSMovement* green, const CSMovement* blue, const CSMovement* alpha, bool fixedComponent, float duration, const CSAnimationLoop& loop);
    inline const CSMovement* colorRed() const {
        return _colorRed;
    }
    inline const CSMovement* colorGreen() const {
        return _colorGreen;
    }
    inline const CSMovement* colorBlue() const {
        return _colorBlue;
    }
    inline const CSMovement* colorAlpha() const {
        return _colorAlpha;
    }
    inline bool colorFixedComponent() const {
        return _colorFixedComponent;
    }
    inline float colorDuration() const {
        return _colorDuration;
    }
    inline const CSAnimationLoop& colorLoop() const {
        return _colorLoop;
    }
    inline void setStartPoint(const CSVector3& startPoint) {
        _startPoint = startPoint;
    }
    inline const CSVector3& startPoint() const {
        return _startPoint;
    }
    inline void setEndPoint(const CSVector3& endPoint) {
        _endPoint = endPoint;
    }
    inline const CSVector3& endPoint() const {
        return _endPoint;
    }
    inline void setImageIndices(const CSArray<ushort, 2>* indices) {
        retain(_imageIndices, indices);
    }
    inline CSAnimationSubstanceType type() const override {
        return CSAnimationSubstanceTypeChain;
    }
    float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
public:
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) override;
    void draw(CSGraphics* graphics, bool shadow) override;
    void preload(CSResourceDelegate* delegate) const override;
    
    inline CSAnimationSubstance* copy(bool capture) const override {
        return new CSChain(this, capture);
    }
};

#endif /* CSChain_h */
