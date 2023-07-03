//
//  CSParticle.h
//  CDK
//
//  Created by 김찬 on 14. 3. 24..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSParticle__
#define __CDK__CSParticle__

#include "CSAnimationSubstance.h"

#include "CSMovement.h"

#include "CSParticleShape.h"

enum CSParticleView {
    CSParticleViewNone,
    CSParticleViewBillboard,
    CSParticleViewHorizontalBillboard,
    CSParticleViewVerticalBillboard,
    CSParticleViewStretchBillboard
};

class CSBuffer;

class CSParticle : public CSAnimationSubstance {
private:
    const CSParticleShape* _shape;
    bool _shapeShell;
    float _life;
    float _lifeVar;
    float _emissionRate;
    uint _emissionMax;
    float _duration;
    float _latency;
    bool _prewarm;
    bool _descent;
    bool _finishAfterEmitting;
    bool _clearWithParent;
    CSBlendMode _blendMode;
    const CSArray<ushort, 2>* _imageIndices;
    byte _imageSequence;
    CSMaterial _material;
    const CSMovement* _colorRed;
    const CSMovement* _colorGreen;
    const CSMovement* _colorBlue;
    const CSMovement* _colorAlpha;
    bool _colorFixedComponent;
    const CSMovement* _radial;
    const CSMovement* _x;
    const CSMovement* _y;
    const CSMovement* _z;
    const CSMovement* _billboardX;
    const CSMovement* _billboardY;
    const CSMovement* _billboardZ;
    const CSMovement* _rotationX;
    const CSMovement* _rotationY;
    const CSMovement* _rotationZ;
    bool _rotationFixedComponent;
    const CSMovement* _scaleX;
    const CSMovement* _scaleY;
    const CSMovement* _scaleZ;
    bool _scaleFixedComponent;
    CSParticleView _view;
    float _stretchRate;
    bool _localSpace;
    
    struct Instance {
        CSMatrix* transform;
        ushort imageIndex;
        CSVector3 firstDir;
        CSVector3 lastDir;
        CSVector3 pos;
        CSVector3 billboardDir;
        CSVector3 billboard;
        CSColor color;
        CSColor colorRandom;
        float radialRandom;
        CSVector3 positionRandom;
        CSVector3 billboardRandom;
        CSVector3 rotation;
        CSVector3 rotationRandom;
        CSVector3 scale;
        CSVector3 scaleRandom;
        float life;
        float progress;
        
        Instance(const Instance& other);
        ~Instance();
        
        Instance& operator =(const Instance& other);
    };
    CSArray<Instance>* _instances;
    float _progress;
    float _counter;
    float _remaining;
    bool _emitting;
public:
    inline CSParticle() {
    
    }
    CSParticle(const CSParticle* other, bool capture);
    CSParticle(CSBuffer* buffer);
private:
    ~CSParticle();
public:
    static inline CSParticle* particle() {
        return autorelease(new CSParticle());
    }
    static inline CSParticle* particleWithParticle(const CSParticle* other, bool capture) {
        return autorelease(new CSParticle(other, capture));
    }
    static inline CSParticle* particleWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSParticle(buffer));
    }
    
    void setShape(const CSParticleShape* shape);
    inline const CSParticleShape* shape() const {
        return _shape;
    }
    
    inline void setShapeShell(bool shapeShell) {
        _shapeShell = shapeShell;
    }
    inline bool shapeShell() const {
        return _shapeShell;
    }
    inline void setLife(float life, float lifeVar) {
        _life = life;
    }
    inline float life() const {
        return _life;
    }
    inline float lifeVar() const {
        return _lifeVar;
    }
    inline void setEmissionRate(float emissionRate) {
        _emissionRate = emissionRate;
    }
    inline float emissionRate() const {
        return _emissionRate;
    }
    inline void setEmissionMax(uint emissionMax) {
        _emissionMax = emissionMax;
    }
    inline uint emissionMax() const {
        return _emissionMax;
    }
    inline void setPrewarm(bool prewarm) {
        _prewarm = prewarm;
    }
    inline bool prewarm() const {
        return _prewarm;
    }
    inline void setDescent(bool descent) {
        _descent = descent;
    }
    inline bool descent() const {
        return _descent;
    }
    inline void setFinishAfterEmitting(bool finish) {
        _finishAfterEmitting = finish;
    }
    inline bool finishAfterEmitting() const {
        return _finishAfterEmitting;
    }
    inline void setClearWithParent(bool clear) {
        _clearWithParent = clear;
    }
    inline bool clearWithParent() const {
        return _clearWithParent;
    }
    inline void setBlendMode(CSBlendMode blendMode) {
        _blendMode = blendMode;
    }
    inline CSBlendMode blendMode() const {
        return _blendMode;
    }
    inline void setImageIndices(const CSArray<ushort, 2>* indices) {
        retain(_imageIndices, indices);
    }
    
    inline void setImageSequence(uint seq) {
        _imageSequence = seq;
    }
    inline uint imageSequence() const {
        return _imageSequence;
    }
    inline void setMaterial(const CSMaterial& material) {
        _material = material;
    }
    inline const CSMaterial& material() const {
        return _material;
    }
    void setColor(const CSMovement* red, const CSMovement* green, const CSMovement* blue, const CSMovement* alpha, bool fixedComponent);
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
    inline void setRadial(const CSMovement* radial) {
        retain(_radial, radial);
    }
    inline const CSMovement* radial() const {
        return _radial;
    }
    void setPosition(const CSMovement* x, const CSMovement* y, const CSMovement* z);
    inline const CSMovement* x() const {
        return _x;
    }
    inline const CSMovement* y() const {
        return _y;
    }
    inline const CSMovement* z() const {
        return _z;
    }
    void setBillboard(const CSMovement* billboardX, const CSMovement* billboardY, const CSMovement* billboardZ);
    inline const CSMovement* billboardX() const {
        return _billboardX;
    }
    inline const CSMovement* billboardY() const {
        return _billboardY;
    }
    inline const CSMovement* billboardZ() const {
        return _billboardZ;
    }
    void setRotation(const CSMovement* rotationX, const CSMovement* rotationY, const CSMovement* rotationZ, bool fixedComponent);
    inline const CSMovement* rotationX() const {
        return _rotationX;
    }
    inline const CSMovement* rotationY() const {
        return _rotationY;
    }
    inline const CSMovement* rotationZ() const {
        return _rotationZ;
    }
    inline bool rotationFixedComponent() const {
        return _rotationFixedComponent;
    }
    void setScale(const CSMovement* scaleX, const CSMovement* scaleY, const CSMovement* scaleZ, bool fixedComponent);
    inline const CSMovement* scaleX() const {
        return _scaleX;
    }
    inline const CSMovement* scaleY() const {
        return _scaleY;
    }
    inline const CSMovement* scaleZ() const {
        return _scaleZ;
    }
    inline bool scaleFixedComponent() const {
        return _scaleFixedComponent;
    }
    inline void setView(CSParticleView view) {
        _view = view;
    }
    inline CSParticleView view() const {
        return _view;
    }
    inline void setStretchRate(float stretchRate) {
        _stretchRate = stretchRate;
    }
    inline float stretchRate() const {
        return _stretchRate;
    }
    inline void setLocalSpace(bool localSpace) {
        _localSpace = localSpace;
    }
    inline bool localSpace() const {
        return _localSpace;
    }
    
    inline CSAnimationSubstanceType type() const override {
        return CSAnimationSubstanceTypeParticle;
    }
    
    float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const override;
	bool isFinite(bool finite) const override;
    void rewind() override;
private:
    void addInstance(const CSMatrix* transform, float delta);
    bool updateInstance(Instance& p, float delta);
    void updateInstances(const CSCamera* camera);
public:
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) override;
    void draw(CSGraphics* graphics, bool shadow) override;
    void preload(CSResourceDelegate* delegate = NULL) const override;
    
    inline CSAnimationSubstance* copy(bool capture) const override {
        return new CSParticle(this, capture);
    }
};

#endif /* defined(__CDK__CSParticle__) */
