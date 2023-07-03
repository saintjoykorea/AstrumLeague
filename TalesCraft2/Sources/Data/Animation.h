//
//  Animation.h
//  TalesCraft2
//
//  Created by ChangSun on 2015. 11. 13..
//  Copyright © 2015년 brgames. All rights reserved.
//

#ifndef Animation_h
#define Animation_h

#include "AssetIndex.h"

#include "Sound.h"

enum AnimationKey : uint {
    AnimationKeyPosition        = (1 << 0),
    AnimationKeyScaleRotation   = (1 << 1),
    AnimationKeyDirection       = (1 << 2),
    AnimationKeyLayerBottom     = (1 << 14),
    AnimationKeyLayerMiddle     = (1 << 15),
    AnimationKeyLayerTop        = (1 << 16),
    AnimationKeyPositionBody    = (1 << 17),
    AnimationKeyPositionHead    = (1 << 18)
};

enum SpriteOp {
    SpriteOpUnitColorOff = 0,
    SpriteOpUnitColorOn,
	SpriteOpUnitColorMinimapOn,
    SpriteOpTitle = 10,
    SpriteOpFadeMessage,
    SpriteOpBattleButtonIn,
    SpriteOpBattleButtonOut,
    SpriteOpBattleButtonTextPrev,
    SpriteOpBattleButtonTextNext,
    SpriteOpBattleModeNotice,
    SpriteOpDivisionRanking,
	SpriteOpTrophyRewardButton,
    SpriteOpMenuTabIcon = 20,
    SpriteOpMenuTabName,
    SpriteOpMenuBattleMode,
    SpriteOpCard = 30,
    SpriteOpCardReference,
    SpriteOpCardOpenText,
    SpriteOpSkin,
	SpriteOpCardFrame_0,
	SpriteOpCardFrame_1,
	SpriteOpCardFrame_2,
    SpriteOpAttainAchievementComplete = 40,
    SpriteOpAttainQuestComplete,
    SpriteOpMinigameQuestComplete,
    SpriteOpTutorialFeatureOpen,
	SpriteOpAchievementIcon,
	SpriteOpMinigameQuestCompleteIcon,
	SpriteOpMinigameRouletteItem,
    SpriteOpRewardLevelPrev = 50,
    SpriteOpRewardLevelNext,
    SpriteOpRewardDivisionPrev,
    SpriteOpRewardDivisionNext,
    SpriteOpRewardClanMark,
    SpriteOpRewardClanMemberGradePrev,         //TODO:not used
    SpriteOpRewardClanMemberGradeNext,         //TODO:not used
    SpriteOpRewardClanMemberGradeNextForward,         //TODO:not used
    SpriteOpRewardContentOpen,
    SpriteOpRewardSeasonScorePrev,
    SpriteOpRewardSeasonScoreNext,
    SpriteOpRewardItem,
    SpriteOpRewardItemDescriptionSmall,
	SpriteOpRewardItemDescriptionLarge,
    SpriteOpRewardBoxCount,
	SpriteOpRewardAdGameResult,
    SpriteOpDecorationName = 70,
    SpriteOpMatchMap = 80,
    SpriteOpMatchMapName,
    SpriteOpMatchAlliancePlayer,
    SpriteOpMatchEnemyPlayer,
	SpriteOpMatchPlayer,	
	SpriteOpMatchRank,
    SpriteOpCampaignTitle = 90,
    SpriteOpCampaignDescription
};

class Animation : public CSObject {
private:
    AnimationIndex _index;
    CSAnimation* _animation;
    bool _alive;
public:
    Animation(const AnimationIndex& index, SoundControl soundControl = SoundControlEffect, bool preload = true);
    Animation(const AnimationGroupIndex& group, int index, SoundControl soundControl = SoundControlEffect, bool preload = true);
    Animation(const AnimationIndex& index, CSAnimationSoundDelegate* soundDelegate, bool preload = true);
    Animation(const AnimationGroupIndex& group, int index, CSAnimationSoundDelegate* soundDelegate, bool preload = true);
    Animation(const Animation* animation, bool preload = true);
private:
    void init(CSAnimation *animation, CSAnimationSoundDelegate* soundDelegate);
    
    ~Animation();
public:
    static inline Animation* animation(const AnimationIndex& index, SoundControl soundControl = SoundControlEffect, bool preload = true) {
        return autorelease(new Animation(index, soundControl, preload));
    }
    static inline Animation* animation(const AnimationGroupIndex& group, int index, SoundControl soundControl = SoundControlEffect, bool preload = true) {
        return autorelease(new Animation(group, index, soundControl, preload));
    }
    static inline Animation* animation(const AnimationIndex& index, CSAnimationSoundDelegate* soundDelegate, bool preload = true) {
        return autorelease(new Animation(index, soundDelegate, preload));
    }
    static inline Animation* animation(const AnimationGroupIndex& group, int index, CSAnimationSoundDelegate* soundDelegate, bool preload = true) {
        return autorelease(new Animation(group, index, soundDelegate, preload));
    }
    static inline Animation* animationWithAnimation(const Animation* animation, bool preload = true) {
        return autorelease(new Animation(animation, preload));
    }
    
    inline const AnimationIndex& index() const {
        return _index;
    }
    
private:
    void setClientPositionImpl(CSAnimation* animation, const CSVector3& position, bool next, uint key);
    void setClientScaleImpl(CSAnimation* animation, float scale, uint key);
    void setClientRotationImpl(CSAnimation* animation, const CSQuaternion& rotation, uint key);
    void setSpriteDelegateImpl(CSAnimation* animation, CSSpriteDelegate* delegate);
    void setSoundEnabledImpl(CSAnimation* animation, bool enabled);
public:
    inline void setClientPosition(const CSVector3& position, bool next, uint key = AnimationKeyPosition) {
        setClientPositionImpl(_animation, position, next, key);
    }
    inline void setClientPosition(const CSVector3& position, uint key = AnimationKeyPosition) {
        setClientPositionImpl(_animation, position, false, key);
        setClientPositionImpl(_animation, position, true, key);
    }
    inline void setClientScale(float scale, uint key = AnimationKeyScaleRotation) {
        setClientScaleImpl(_animation, scale, key);
    }
    inline void setClientRotation(const CSQuaternion& rotation, uint key = AnimationKeyScaleRotation) {
        setClientRotationImpl(_animation, rotation, key);
    }
    inline void setSpriteDelegate(CSSpriteDelegate* delegate) {
        setSpriteDelegateImpl(_animation, delegate);
    }
	inline void setSpriteDelegate0(std::function<void(CSGraphics*, const byte*)> func) {
		setSpriteDelegateImpl(_animation, CSSpriteDelegate0::delegate(func));
	}
	template <typename Param0>
	inline void setSpriteDelegate1(std::function<void(CSGraphics*, const byte*, non_deduced_t<Param0>)> func, Param0 param0) {
		CSSpriteDelegate1<Param0>* del = CSSpriteDelegate1<Param0>::delegate(func);
		del->setParam0(param0);
		setSpriteDelegateImpl(_animation, del);
	}
	template <typename Param0, typename Param1>
	inline void setSpriteDelegate2(std::function<void(CSGraphics*, const byte*, non_deduced_t<Param0>, non_deduced_t<Param1>)> func, Param0 param0, Param1 param1) {
		CSSpriteDelegate2<Param0, Param1>* del = CSSpriteDelegate2<Param0, Param1>::delegate(func);
		del->setParam0(param0);
		del->setParam1(param1);
		setSpriteDelegateImpl(_animation, del);
	}
	template <typename Param0, typename Param1, typename Param2>
	inline void setSpriteDelegate3(std::function<void(CSGraphics*, const byte*, non_deduced_t<Param0>, non_deduced_t<Param1>, non_deduced_t<Param2>)> func, Param0 param0, Param1 param1, Param2 param2) {
		CSSpriteDelegate3<Param0, Param1, Param2>* del = CSSpriteDelegate3<Param0, Param1, Param2>::delegate(func);
		del->setParam0(param0);
		del->setParam1(param1);
		del->setParam2(param2);
		setSpriteDelegateImpl(_animation, del);
	}
	template <class Object>
	inline void setSpriteDelegate0(Object* object, void(Object::*method)(CSGraphics*, const byte*)) {
		setSpriteDelegateImpl(_animation, CSSpriteDelegate0::delegate(object, method));
	}
	template <class Object, typename Param0>
	inline void setSpriteDelegate1(Object* object, void(Object::*method)(CSGraphics*, const byte*, non_deduced_t<Param0>),  Param0 param0) {
		CSSpriteDelegate1<Param0>* del = CSSpriteDelegate1<Param0>::delegate(object, method);
		del->setParam0(param0);
		setSpriteDelegateImpl(_animation, del);
	}
	template <class Object, typename Param0, typename Param1>
	inline void setSpriteDelegate2(Object* object, void(Object::*method)(CSGraphics*, const byte*, non_deduced_t<Param0>, non_deduced_t<Param1>), Param0 param0, Param1 param1) {
		CSSpriteDelegate2<Param0, Param1>* del = CSSpriteDelegate2<Param0, Param1>::delegate(object, method);
		del->setParam0(param0);
		del->setParam1(param1);
		setSpriteDelegateImpl(_animation, del);
	}
	template <class Object, typename Param0, typename Param1, typename Param2>
	inline void setSpriteDelegate3(Object* object, void(Object::*method)(CSGraphics*, const byte*, non_deduced_t<Param0>, non_deduced_t<Param1>, non_deduced_t<Param2>), Param0 param0, Param1 param1, Param2 param2) {
		CSSpriteDelegate3<Param0, Param1, Param2>* del = CSSpriteDelegate3<Param0, Param1, Param2>::delegate(object, method);
		del->setParam0(param0);
		del->setParam1(param1);
		del->setParam2(param2);
		setSpriteDelegateImpl(_animation, del);
	}

    inline void setSoundEnabled(bool enabled) {
        setSoundEnabledImpl(_animation, enabled);
    }
    
private:
    bool captureImpl(CSAnimation* animation, uint key, const CSCamera* camera, CSMatrix& capture) const;
public:
    bool capture(uint key, const CSCamera* camera, CSMatrix& capture) const;
private:
    void elementsImpl(uint key, CSAnimation* animation, CSArray<CSAnimation, 1, false>* elements);
public:
    const CSArray<CSAnimation, 1, false>* elements(uint key);
    
    inline const CSArray<CSAnimation>* elements(uint key) const {
        return reinterpret_cast<const CSArray<CSAnimation>*>(const_cast<Animation*>(this)->elements(key));
    }

    inline CSAnimation* root() {
        return _animation;
    }
    inline const CSAnimation* root() const {
        return _animation;
    }
    
    inline float duration(CSAnimationDuration type = CSAnimationDurationMax) const {
        return _animation->duration(NULL, type);
    }
    inline float remaining(CSAnimationDuration type = CSAnimationDurationMax) const {
        return _animation->remaining(NULL, type);
    }
    inline float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax) const {
        return _animation->duration(camera, type);
    }
    inline float remaining(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax) const {
        return _animation->remaining(camera, type);
    }
    inline float progress() const {
        return _animation->progress();
    }
	inline bool isFinite() const {
		return _animation->isFinite();
	}
    inline void rewind() {
        _alive = true;
        _animation->rewind();
    }
    inline void stop() {
        _alive = false;
    }
    inline void setLatency(float value) {
        _animation->setLatency(value);
    }
    inline bool update(float delta, const CSCamera* camera = NULL) {
        return _animation->update(delta, camera, _alive) != CSAnimationStateStopped;
    }
	void drawLayered(CSGraphics* graphics, uint visibleKey, bool shadow = false);
    void drawLayered(CSGraphics* graphics, const CSVector3& pos, uint visibleKey, bool shadow = false);
    void drawLayeredScaled(CSGraphics* graphics, const CSVector3& pos, float scale, uint visibleKey, bool shadow = false);
    
    inline void draw(CSGraphics* graphics) {
        drawLayered(graphics, CSAnimationVisibleKeyAll, false);
    }
    inline void draw(CSGraphics* graphics, const CSVector3& pos) {
        drawLayered(graphics, pos, CSAnimationVisibleKeyAll, false);
    }
    inline void drawScaled(CSGraphics* graphics, const CSVector3& pos, float scale) {
        drawLayeredScaled(graphics, pos, scale, CSAnimationVisibleKeyAll, false);
    }
    
    void preload(CSResourceDelegate* delegate = NULL) const;
    
    inline CSObject* copy() const override {
        return new Animation(this, false);
    }
    bool captureForObject(uint key, CSMatrix& capture) const;
    float durationForObject() const;
    float remainingForObject() const;
    bool updateForObject(float delta);
    
    //map object support
#ifdef CS_ASSERT_DEBUG
private:
    bool checkForLayered(const CSAnimation* animation) const ;
public:
    void checkForLayered() const;
#else
    inline void checkForLayered() const {
        
    }
#endif
};


#endif /* Animation_h */
