//
//  UnitData.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 18..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef UnitData_h
#define UnitData_h

#include "ObjectType.h"
#include "AbilityData.h"
#include "CommandData.h"

enum Motion {
    MotionStop,
    MotionDestroy,
    MotionMove,
    MotionAction_0,
    MotionAction_1,
    MotionAction_2,
    MotionAction_3,
    MotionAction_4,
    MotionAction_5,
    MotionAction_6,
    MotionAction_7,
    MotionAction_8,
    MotionCount
};

struct MotionData {
    AnimationIndex animation;
    short rateAbility;
    bool rewind;
    
    MotionData(CSBuffer* buffer);
    
    inline MotionData(const MotionData&) {
        CSAssert(false, "invalid operation");
    }
    inline MotionData& operator=(const MotionData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

enum Voice {
	VoiceSpawn,
	VoiceSelect,
	VoiceCommand,
	VoiceDestroy,
	VoiceCount
};

class VoiceData : public CSObject {
public:
	const CSArray<SoundIndex>* sounds[VoiceCount];

	VoiceData(CSBuffer* buffer);
private:
	~VoiceData();
};

struct SkinData {
    ImageIndex smallIcon;
    ImageIndex largeIcon;
    AnimationIndex illustration;
	ImageIndex commandIcon;
    AnimationIndex spawnAnimation;
	const CSLocaleString* name;
    byte grade;
    const CSLocaleString* previewUrl;
    AbilityUnitData ability;
    const MotionData* motions[MotionCount];
    const CSDictionary<AnimationIndex, AnimationIndex>* effects;
	const CSDictionary<CSString, VoiceData>* voices;
    const CSArray<AnimationIndex>* preloadAnimations;
    
    SkinData(CSBuffer* buffer);
    ~SkinData();
    
    inline SkinData(const SkinData& other) : ability(other.ability) {
        CSAssert(false, "invalid operation");
    }
    inline SkinData& operator=(const SkinData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    const AnimationIndex& effectIndex(const AnimationIndex& index) const;

	const VoiceData* voice() const;
};

struct UnitData {
    const CSLocaleString* name;
    const CSLocaleString* description;
    byte grade;
    sbyte category;
	ObjectType type;
	bool visible;
	byte attribute;
	sbyte attackType;
	sbyte armorType;
	bool subordinated;
	byte weight;
	byte population;
	byte addition;
    bool build;
	ushort resources[2];
	ushort progress;
	ushort order;
	fixed collider;
	float radius;
	float body;
	float top;
	float z;
	AnimationIndex minimap;
    AbilityUnitData ability;
    const CSArray<UnitIndex>* children;
    const CSArray<byte, 2>* spellTrigger;
    const CSArray<byte, 3>* actionTriggers;
    const CSDictionary<CommandIndex, CommandUnitData>* commands;
    const CSArray<SkinData>* skins;
    
    UnitData(CSBuffer* buffer);
    ~UnitData();
    
    inline UnitData(const UnitData& other) : ability(other.ability) {
        CSAssert(false, "invalid operation");
    }
    inline UnitData& operator=(const UnitData&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
    
    inline const SkinData* skin(int index) const {
        return skins ? &skins->objectAtIndex(index < skins->count() ? index : 0) : NULL;
    }
    
    inline const SkinData* baseSkin() const {
        return skins ? &skins->objectAtIndex(0) : NULL;
    }

    const AnimationIndex& effectIndex(int skin, const AnimationIndex& index) const;
};

#endif /* UnitData_h */
