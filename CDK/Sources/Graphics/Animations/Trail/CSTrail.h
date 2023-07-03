//
//  CSTrail.h
//  CDK
//
//  Created by 김찬 on 2015. 11. 25..
//  Copyright © 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSTrail__
#define __CDK__CSTrail__

#include "CSAnimationSubstance.h"

#include "CSMovement.h"

class CSBuffer;

class CSTrail : public CSAnimationSubstance {
private:
    CSBlendMode _blendMode;
    const CSArray<ushort, 2>* _imageIndices;
    CSMaterial _material;
    bool _localSpace;
	bool _billboard;
    bool _repeat;
    float _repeatScroll;
    float _minDistance;
    float _pointLife;
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
    
    struct Point {
        CSVector3 point;
        float trailProgress;
        float animationProgress;
        int link;
        
        Point(const CSVector3& point, float trailProgress, float animationProgress, int link);
    };
    CSArray<Point>* _points;
    float _progress;
    bool _emitting;
    int _link;
public:
    CSTrail();
    CSTrail(const CSTrail* other, bool capture);
    CSTrail(CSBuffer* buffer);
private:
    ~CSTrail();
public:
    static inline CSTrail* trail() {
        return autorelease(new CSTrail());
    }
    static inline CSTrail* trailWithTrail(const CSTrail* other, bool capture) {
        return autorelease(new CSTrail(other, capture));
    }
    static inline CSTrail* trailWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSTrail(buffer));
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
    inline float repeatScroll() const {
        return _repeatScroll;
    }
    inline void setRepeatScroll(float repeatScroll) {
        _repeatScroll = repeatScroll;
    }
    inline bool localSpace() const {
        return _localSpace;
    }
    inline void setLocalSpace(bool localSpace) {
        _localSpace = localSpace;
    }
    inline float minDistance() const {
        return _minDistance;
    }
    inline void setMinDistance(float minDistance) {
        _minDistance = minDistance;
    }
    inline float pointLife() const {
        return _pointLife;
    }
    inline void setPointLife(float pointLife) {
        _pointLife = pointLife;
    }
    inline float width() const {
        return _width;
    }
    inline void setWidth(float width) {
        _width = width;
    }
    inline float imageInterval() const {
        return _imageInterval;
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
    inline void setImageIndices(const CSArray<ushort, 2>* indices) {
        retain(_imageIndices, indices);
    }
private:
    void addPoint(const CSVector3& p, float trailProgress, float animationProgress, float distance);
public:
    inline void addPoint(const CSVector3& p) {
        addPoint(p, _progress, -1, _minDistance);
    }
    
    inline CSAnimationSubstanceType type() const override {
        return CSAnimationSubstanceTypeTrail;
    }
    float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
private:
    enum BreakEntryType {
        BreakEntryTypeNone,
        BreakEntryTypeRewind,
        BreakEntryTypeReverse
    };
    struct BreakEntry {
        float delta;
        BreakEntryType type;
    };
    void addBreak(float progress, float delta, float duration, const CSAnimationLoop& loop, CSArray<BreakEntry>*& breaks);
public:
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) override;
    void draw(CSGraphics* graphics, bool shadow) override;
    void preload(CSResourceDelegate* delegate) const override;
    
    inline CSAnimationSubstance* copy(bool capture) const override {
        return new CSTrail(this, capture);
    }
};

#endif /* __CDK__CSTrail__ */
