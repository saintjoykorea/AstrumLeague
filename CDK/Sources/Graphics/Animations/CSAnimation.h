//
//  CSAnimation.h
//  CDK
//
//  Created by 김찬 on 13. 11. 19..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimation__
#define __CDK__CSAnimation__

#include "CSMovement.h"

#include "CSVector3.h"

#include "CSSet.h"

#include "CSAnimationParam.h"
#include "CSAnimationSubstance.h"
#include "CSAnimationDerivation.h"

#define CSAnimationVisibleKeyAll   0xffffffff

class CSBuffer;

class CSAnimationDerivationMulti;
class CSAnimationDerivationLinked;
class CSAnimationDerivationEmission;
class CSAnimationDerivationRandom;

typedef CSDelegate<void, const CSAnimation*, const ushort*, int, float, bool> CSAnimationSoundDelegate;

typedef CSDelegate0<void, const CSAnimation*, const ushort*, int, float, bool> CSAnimationSoundDelegate0;

template <typename Param0>
using CSAnimationSoundDelegate1 = CSDelegate1<void, Param0, const CSAnimation*, const ushort*, int, float, bool>;
template <typename Param0, typename Param1>
using CSAnimationSoundDelegate2 = CSDelegate2<void, Param0, Param1, const CSAnimation*, const ushort*, int, float, bool>;
template <typename Param0, typename Param1, typename Param2>
using CSAnimationSoundDelegate3 = CSDelegate3<void, Param0, Param1, Param2, const CSAnimation*, const ushort*, int, float, bool>;

class CSAnimation : public CSObject {
private:
    uint _key;
    uint _subkey;
    byte _weight;
    bool _reverse;
    bool _facing;
    bool _globals[2];
    const CSMovement* _radial;
    const CSMovement* _tangential;
    float _tangentialAngle;
    float _tangentialAngleVar;
    const CSMovement* _x;
    const CSMovement* _y;
    const CSMovement* _z;
    float _positionDegree;
    bool _positionUsingSpeed;
    CSAnimationLoop _positionLoop;
    bool _billboardOffset;
    const CSMovement* _rotationX;
    const CSMovement* _rotationY;
    const CSMovement* _rotationZ;
    float _rotationDuration;
    CSAnimationLoop _rotationLoop;
    bool _rotationFixedComponent;
    const CSMovement* _scaleX;
    const CSMovement* _scaleY;
    const CSMovement* _scaleZ;
    float _scaleDuration;
    CSAnimationLoop _scaleLoop;
    bool _scaleFixedComponent;
    bool _rotationFirst;
    bool _usingLight;
    bool _usingDepth;
    CSDepthMode _depthMode;
    float _depthBias;
    bool _usingStencil;
    bool _usingAlphaTest;
    const CSMovement* _alphaTest;
    float _alphaTestDuration;
    CSAnimationLoop _alphaTestLoop;
    float _duration;
    float _latency;
    bool _derivationFinish;
    bool _closing;
    bool _visible;

    const CSArray<ushort>* _soundIndices;
    CSAnimationSoundDelegate* _soundDelegate;
    float _soundVolume;
    float _soundLoop;
    float _soundLatency;
    bool _soundEssential;
    bool _soundEnabled;
    bool _localeVisible;
    mutable bool _localeCurrentContains;
    mutable uint _localeCurrentMark;
    CSSet<CSString>* _locales;

    CSAnimation* _parent;
    CSAnimationSubstance* _substance;
    CSAnimationDerivation* _derivation;
    CSMatrix* _transform;
    CSVector3 _clientPositions[2];
    float _clientScale;
    CSQuaternion _clientRotation;
    CSQuaternion* _facingRotation;
    CSMatrix* _facingCapture;
    int64 _positionRandom;
    int64 _rotationRandom;
    int64 _scaleRandom;
    int64 _alphaTestRandom;
    int64 _derivationRandom;
    uint _soundCounter;
    float _progress;
public:
    inline CSAnimation() {
    
    }
    CSAnimation(const CSAnimation* other, bool capture);
    
    CSAnimation(CSBuffer* buffer);
private:
    ~CSAnimation();
public:
    static CSAnimation* animation() {
        return autorelease(new CSAnimation());
    }
    static inline CSAnimation* animationWithAnimation(const CSAnimation* other, bool capture) {
        return autorelease(new CSAnimation(other, capture));
    }
    static CSAnimation* animationWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSAnimation(buffer));
    }
private:
    void attach(CSAnimation* parent);
public:
    inline CSAnimation* parent() {
        return _parent;
    }
    inline const CSAnimation* parent() const {
        return _parent;
    }
    
    inline void setSound(const CSArray<ushort>* indices, float volume, float loop, float latency, bool essential) {
        retain(_soundIndices, indices);
        _soundVolume = volume;
        _soundLoop = loop;
        _soundLatency = latency;
        _soundEssential = essential;
    }
    inline const CSArray<ushort>* soundIndices() const {
        return _soundIndices;
    }
    inline float soundVolume() const {
        return _soundVolume;
    }
    inline float soundLoop() const {
        return _soundLoop;
    }
    inline float soundLatency() const {
        return _soundLatency;
    }
    inline bool soundEssential() const {
        return _soundEssential;
    }
    inline void setSoundDelegate(CSAnimationSoundDelegate* delegate) {
        retain(_soundDelegate, delegate);
    }
    inline void setSoundEnabled(bool enabled) {
        _soundEnabled = enabled;
    }
    inline bool soundEnabled() const {
        return _soundEnabled;
    }
    
    inline void setLocaleVisible(bool visible) {
        _localeVisible = visible;
    }
    inline bool localeVisible() const {
        return _localeVisible;
    }
    bool checkLocale() const;
    void addLocale(const CSString* locale);
    void removeLocale(const CSString* locale);

    void setSubstance(CSAnimationSubstance* substance);
    inline CSAnimationSubstance* substance() {
        return _substance;
    }
    inline const CSAnimationSubstance* substance() const {
        return _substance;
    }
    
    void setDerivation(CSAnimationDerivation* derivation);
    inline CSAnimationDerivation* derivation() {
        return _derivation;
    }
    inline const CSAnimationDerivation* derivation() const {
        return _derivation;
    }
    inline void setDerivationFinish(bool derivationFinish) {
        _derivationFinish = derivationFinish;
    }
    inline bool derivationFinish() const {
        return _derivationFinish;
    }
    
    void setTransform(const CSMatrix* transform);
    inline const CSMatrix* transform() const {
        return _transform;
    }
    
    inline void setKey(uint key) {
        _key = key;
    }
    inline uint key() const {
        return _key;
    }
    
    void makeSubkey();
    
    inline uint subkey() const {
        return _subkey;
    }
    
    inline void setWeight(int weight) {
        _weight = weight;
    }
    inline int weight() const {
        return _weight;
    }
    
    inline void setReverse(bool reverse) {
        _reverse = reverse;
    }
    inline bool reverse() const {
        return _reverse;
    }
    inline void setFacing(bool facing) {
        _facing = facing;
    }
    inline bool facing() const {
        return _facing;
    }
    inline void setGlobal(bool global, bool next) {
        _globals[next] = global;
    }
    inline bool global(bool next) const {
        return _globals[next];
    }
    inline void setRadial(const CSMovement* radial) {
        retain(_radial, radial);
    }
    inline const CSMovement* radial() const {
        return _radial;
    }
    inline void setTangential(const CSMovement* tangential) {
        retain(_tangential, tangential);
    }
    inline const CSMovement* tangential() const {
        return _tangential;
    }
    inline void setX(const CSMovement* x) {
        retain(_x, x);
    }
    inline const CSMovement* x() const {
        return _x;
    }
    inline void setY(const CSMovement* y) {
        retain(_y, y);
    }
    inline const CSMovement* y() const {
        return _y;
    }
    inline void setZ(const CSMovement* z) {
        retain(_z, z);
    }
    inline const CSMovement* z() const {
        return _z;
    }
    inline void setDeltaPosition(float degree, bool usingSpeed, const CSAnimationLoop& loop) {
        _positionDegree = degree;
        _positionUsingSpeed = usingSpeed;
        _positionLoop = loop;
    }
    inline bool positionUsingSpeed() const {
        return _positionUsingSpeed;
    }
    inline float positionDegree() const {
        return _positionDegree;
    }
    inline const CSAnimationLoop& positionLoop() const {
        return _positionLoop;
    }
    
    inline void setRotationX(const CSMovement* x) {
        retain(_rotationX, x);
    }
    inline const CSMovement* rotationX() const {
        return _rotationX;
    }
    inline void setRotationY(const CSMovement* y) {
        retain(_rotationY, y);
    }
    inline const CSMovement* rotationY() const {
        return _rotationY;
    }
    inline void setRotationZ(const CSMovement* z) {
        retain(_rotationZ, z);
    }
    inline const CSMovement* rotationZ() const {
        return _rotationZ;
    }
    inline void setDeltaRotation(float duration, const CSAnimationLoop& loop) {
        _rotationDuration = duration;
        _rotationLoop = loop;
    }
    inline float rotationDuration() const {
        return _rotationDuration;
    }
    inline const CSAnimationLoop& rotationLoop() const {
        return _rotationLoop;
    }
    
    inline void setRotationFixedComponent(bool rotationFixedComponent) {
        _rotationFixedComponent = rotationFixedComponent;
    }
    inline bool rotationFixedComponent() const {
        return _rotationFixedComponent;
    }
    
    inline void setScaleX(const CSMovement* x) {
        retain(_scaleX, x);
    }
    inline const CSMovement* scaleX() const {
        return _scaleX;
    }
    void setScaleY(const CSMovement* y) {
        retain(_scaleY, y);
    }
    inline const CSMovement* scaleY() const {
        return _scaleY;
    }
    void setScaleZ(const CSMovement* z) {
        retain(_scaleZ, z);
    }
    inline const CSMovement* scaleZ() const {
        return _scaleZ;
    }
    inline void setDeltaScale(float duration, const CSAnimationLoop& loop) {
        _scaleDuration = duration;
        _scaleLoop = loop;
    }
    inline float scaleDuration() const {
        return _scaleDuration;
    }
    inline const CSAnimationLoop& scaleLoop() const {
        return _scaleLoop;
    }
    
    inline void setScaleFixedComponent(bool scaleFixedComponent) {
        _scaleFixedComponent = scaleFixedComponent;
    }
    inline bool scaleFixedComponent() const {
        return _scaleFixedComponent;
    }
    
    inline void setRotationFirst(bool rotationFirst) {
        _rotationFirst = rotationFirst;
    }
    inline bool rotationFirst() const {
        return _rotationFirst;
    }
    inline void setUsingLight(bool usingLight) {
        _usingLight = usingLight;
    }
    inline bool usingLight() const {
        return _usingLight;
    }
    inline void setDepth(CSDepthMode depthMode, float depthBias) {
        _usingDepth = true;
        _depthMode = depthMode;
        _depthBias = depthBias;
    }
    inline void removeDepth() {
        _usingDepth = false;
    }
    inline bool usingDepth() const {
        return _usingDepth;
    }
    inline CSDepthMode depthMode() const {
        return _depthMode;
    }
    inline float depthBias() const {
        return _depthBias;
    }
    inline void setStencil(bool stencil) {
        _usingStencil = stencil;
    }
    inline bool usingStencil() const {
        return _usingStencil;
    }
    inline void setAlphaTest(const CSMovement* alpha) {
        _usingAlphaTest = true;
        retain(_alphaTest, alpha);
        _alphaTestDuration = 0;
    }
    inline void setAlphaTest(const CSMovement* alpha, float duration, const CSAnimationLoop& loop) {
        _usingAlphaTest = true;
        retain(_alphaTest, alpha);
        _alphaTestDuration = duration;
        _alphaTestLoop = loop;
    }
    inline void removeAlphaTest() {
        _usingAlphaTest = false;
    }
    inline const CSMovement* alphaTest() const {
        return _alphaTest;
    }
    inline float alphaTestDuration() const {
        return _alphaTestDuration;
    }
    inline const CSAnimationLoop& alphaTestLoop() const {
        return _alphaTestLoop;
    }
    inline bool usingAlphaTest() const {
        return _usingAlphaTest;
    }
    
    inline void setClientPosition(const CSVector3& position, bool next) {
        _clientPositions[next] = position;
    }
    inline void setClientPosition(const CSVector3& position) {
        _clientPositions[0] = _clientPositions[1] = position;
    }
    inline const CSVector3& clientPosition(bool next) const {
        return _clientPositions[next];
    }
    inline void setClientScale(float scale) {
        _clientScale = scale;
    }
    inline float clientScale() const {
        return _clientScale;
    }
    inline void setClientRotation(const CSQuaternion& rotation) {
        _clientRotation = rotation;
    }
    inline const CSQuaternion& clientRotation() const {
        return _clientRotation;
    }
public:
    bool capture(float progress, const CSCamera* camera, CSMatrix& capture, bool facing = true) const;
public:
    void rewind();
    
    inline float progress() const {
        return _progress;
    }
    
    inline void setDuration(float duration) {
        _duration = duration;
    }
public:
    float positionDuration(const CSCamera* camera) const;
    
    float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax) const;

	bool isFinite(bool finite = false) const;
    
    inline void setLatency(float latency) {
        _progress += _latency - latency;
        _latency = latency;
    }
    inline float latency() const {
        return _latency;
    }
    
    inline void setClosing(bool closing) {
        _closing = closing;
    }
    inline bool closing() const {
        return _closing;
    }
    
    float remaining(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax) const;
private:
    void updateFacing(const CSCamera* camera);
public:
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true);
	bool isVisible(uint visibleKey) const;
    void draw(CSGraphics* graphics, uint visibleKey, bool shadow);
    void preload(CSResourceDelegate* delegate = NULL) const;
    
    inline CSObject* copy() const override {
        return new CSAnimation(this, true);
    }
    
    friend class CSAnimationDerivation;
    friend class CSAnimationDerivationMulti;
    friend class CSAnimationDerivationLinked;
    friend class CSAnimationDerivationEmission;
    friend class CSAnimationDerivationRandom;
    
};

#endif /* defined(__CDK__CSAnimation__) */
