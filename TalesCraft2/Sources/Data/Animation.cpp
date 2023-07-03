//
//  Animation.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2015. 11. 13..
//  Copyright © 2015년 brgames. All rights reserved.
//
#include "Animation.h"

#include "Asset.h"

#include "Map.h"

static void animationSoundCallback(const CSAnimation* animation, const ushort* indices, int indexCount, float volume, bool essential, SoundControl soundControl) {
    CSAssert(indexCount == 3, "invalid data");
    SoundIndex index(indices[0], indices[1], indices[2]);
    
    Sound::play(soundControl, index, essential ? SoundPlayEffectEssential : SoundPlayEffect, volume);
}

Animation::Animation(const AnimationIndex& index, SoundControl soundControl, bool preload) : _index(index), _alive(true) {
    _animation = new CSAnimation(Asset::sharedAsset()->animation(index), false);

	CSAnimationSoundDelegate1<SoundControl>* delegate = CSAnimationSoundDelegate1<SoundControl>::delegate(&animationSoundCallback);
	delegate->setParam0(soundControl);
    
    init(_animation, delegate);
    
    if (preload) this->preload();
}

Animation::Animation(const AnimationGroupIndex& group, int index, SoundControl soundControl, bool preload) : _index(AnimationIndex(group, index)), _alive(true) {
    _animation = new CSAnimation(Asset::sharedAsset()->animation(group, index), false);
    
	CSAnimationSoundDelegate1<SoundControl>* delegate = CSAnimationSoundDelegate1<SoundControl>::delegate(&animationSoundCallback);
	delegate->setParam0(soundControl);

    init(_animation, delegate);
    
    if (preload) this->preload();
}

Animation::Animation(const AnimationIndex& index, CSAnimationSoundDelegate* soundDelegate, bool preload) : _index(index), _alive(true) {
    _animation = new CSAnimation(Asset::sharedAsset()->animation(index), false);
    
    init(_animation, soundDelegate);
    
    if (preload) this->preload();
}

Animation::Animation(const AnimationGroupIndex& group, int index, CSAnimationSoundDelegate* soundDelegate, bool preload) : _index(AnimationIndex(group, index)), _alive(true) {
    _animation = new CSAnimation(Asset::sharedAsset()->animation(group, index), false);
    
    init(_animation, soundDelegate);
    
    if (preload) this->preload();
}

Animation::Animation(const Animation* animation, bool preload) : _index(animation->_index), _animation(new CSAnimation(animation->_animation, true)), _alive(true) {
    if (preload) this->preload();
}

Animation::~Animation() {
    _animation->release();
}

void Animation::init(CSAnimation *animation, CSAnimationSoundDelegate* soundDelegate) {
    if (animation->soundIndices()) {
        animation->setSoundDelegate(soundDelegate);
    }
    CSAnimationSubstance* substance = animation->substance();
    if (substance) substance->setResourceDelegate(Asset::sharedAsset()->resourceDelegate);
    CSAnimationDerivation* derivation = animation->derivation();
    if (derivation) {
        foreach(CSAnimation*, child, derivation->animations()) {
            init(child, soundDelegate);
        }
    }
}

void Animation::setSpriteDelegateImpl(CSAnimation* animation, CSSpriteDelegate* delegate) {
    CSAnimationSubstance* substance = animation->substance();
    if (substance) {
        switch (substance->type()) {
            case CSAnimationSubstanceTypeSprite:
                static_cast<CSSprite*>(substance)->setDelegate(delegate);
                break;
                
        }
    }
    CSAnimationDerivation* derivation = animation->derivation();
    if (derivation) {
        foreach(CSAnimation*, child, derivation->animations()) {
            setSpriteDelegateImpl(child, delegate);
        }
    }
}

void Animation::setSoundEnabledImpl(CSAnimation* animation, bool enabled) {
    animation->setSoundEnabled(enabled);

    CSAnimationDerivation* derivation = animation->derivation();
    if (derivation) {
        foreach(CSAnimation*, child, derivation->animations()) {
            setSoundEnabledImpl(child, enabled);
        }
    }
}

void Animation::setClientPositionImpl(CSAnimation* animation, const CSVector3& position, bool next, uint key) {
    if ((animation->key() & key) == key) {
        animation->setClientPosition(position, next);
    }
    if ((animation->subkey() & key) == key) {
        CSAnimationDerivation* derivation = animation->derivation();
        if (derivation) {
            foreach(CSAnimation*, child, derivation->animations()) {
                setClientPositionImpl(child, position, next, key);
            }
        }
    }
}

void Animation::setClientScaleImpl(CSAnimation* animation, float scale, uint key) {
    if ((animation->key() & key) == key) {
        animation->setClientScale(scale);
    }
    if ((animation->subkey() & key) == key) {
        CSAnimationDerivation* derivation = animation->derivation();
        if (derivation) {
            foreach(CSAnimation*, child, derivation->animations()) {
                setClientScaleImpl(child, scale, key);
            }
        }
    }
}

void Animation::setClientRotationImpl(CSAnimation* animation, const CSQuaternion& rotation, uint key) {
    if ((animation->key() & key) == key) {
        animation->setClientRotation(rotation);
    }
    if ((animation->subkey() & key) == key) {
        CSAnimationDerivation* derivation = animation->derivation();
        if (derivation) {
            foreach(CSAnimation*, child, derivation->animations()) {
                setClientRotationImpl(child, rotation, key);
            }
        }
    }
}

bool Animation::captureImpl(CSAnimation* animation, uint key, const CSCamera* camera, CSMatrix &capture) const {
    if ((animation->key() & key) == key) {
        return animation->capture(animation->progress(), camera, capture);
    }
    if ((animation->subkey() & key) == key) {
        CSAnimationDerivation* derivation = animation->derivation();
        if (derivation) {
            foreach(CSAnimation*, child, derivation->animations()) {
                if (captureImpl(child, key, camera, capture)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Animation::capture(uint key, const CSCamera* camera, CSMatrix& capture) const {
    return captureImpl(_animation, key, camera, capture);
}

void Animation::elementsImpl(uint key, CSAnimation* animation, CSArray<CSAnimation, 1, false>* elements) {
    if ((animation->key() & key) == key) {
        elements->addObject(animation);
    }
    if ((animation->subkey() & key) == key) {
        CSAnimationDerivation* derivation = animation->derivation();
        if (derivation) {
            foreach(CSAnimation*, child, derivation->animations()) {
                elementsImpl(key, child, elements);
            }
        }
    }
}

const CSArray<CSAnimation, 1, false>* Animation::elements(uint key) {
    CSArray<CSAnimation, 1, false>* elements = CSArray<CSAnimation, 1, false>::array();
	elementsImpl(key, _animation, elements);
    return elements;
}

bool Animation::captureForObject(uint key, CSMatrix& capture) const {
    bool rtn;
    
    const Camera* camera = Map::sharedMap()->camera();
    synchronized(camera, GameLockMap) {
        rtn = this->capture(key, &camera->capture(), capture);
    }
    return rtn;
}

float Animation::durationForObject() const {
    float duration;
    
    const Camera* camera = Map::sharedMap()->camera();
    synchronized(camera, GameLockMap) {
        duration = _animation->duration(&camera->capture());
    }
    return duration;
}

float Animation::remainingForObject() const {
    float remaining;
    
    const Camera* camera = Map::sharedMap()->camera();
    synchronized(camera, GameLockMap) {
        remaining = _animation->remaining(&camera->capture());
    }
    return remaining;
}

bool Animation::updateForObject(float delta) {
    bool rtn;
    
    const Camera* camera = Map::sharedMap()->camera();
    synchronized(camera, GameLockMap) {
        rtn = update(delta, &camera->capture());
    }
    return rtn;
}

void Animation::drawLayered(CSGraphics* graphics, uint visibleKey, bool shadow) {
	_animation->draw(graphics, visibleKey, shadow);
}

void Animation::drawLayered(CSGraphics* graphics, const CSVector3& pos, uint visibleKey, bool shadow) {
    graphics->pushTransform();
    graphics->translate(pos);
	drawLayered(graphics, visibleKey, shadow);
    graphics->popTransform();
}

void Animation::drawLayeredScaled(CSGraphics* graphics, const CSVector3& pos, float scale, uint visibleKey, bool shadow) {
    graphics->pushTransform();
    graphics->translate(pos);
    graphics->scale(scale);
	drawLayered(graphics, visibleKey, shadow);
    graphics->popTransform();
}

void Animation::preload(CSResourceDelegate* delegate) const {
    CSAssert(CSThread::mainThread()->isActive(), "invalid operation");
    
    _animation->preload(delegate);
}

#ifdef CS_ASSERT_DEBUG
bool Animation::checkForLayered(const CSAnimation* animation) const {
    int layerCount = 0;
    if (animation->key() & AnimationKeyLayerBottom) {
        layerCount++;
    }
    if (animation->key() & AnimationKeyLayerMiddle) {
        layerCount++;
    }
    if (animation->key() & AnimationKeyLayerTop) {
        layerCount++;
    }
    if (layerCount == 1) {
        return true;
    }
    if (animation->substance()) {
        return false;
    }
    if (animation->derivation()) {
        foreach(const CSAnimation*, child, animation->derivation()->animations()) {
            if (!checkForLayered(child)) {
                return false;
            }
        }
    }
    return true;
}

void Animation::checkForLayered() const {
    CSAssert(checkForLayered(_animation), "invalid object animation:%d, %d, %d", _index.indices[0], _index.indices[1], _index.indices[2]);
}
#endif



