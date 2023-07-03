//
//  UnitData.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 18..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#include "UnitData.h"

#include "Asset.h"

MotionData::MotionData(CSBuffer* buffer) :
    animation(buffer),
    rateAbility(buffer->readBoolean() ? buffer->readShort() : -1),
    rewind(buffer->readBoolean())
{
}

VoiceData::VoiceData(CSBuffer* buffer) {
	for (int i = 0; i < VoiceCount; i++) {
		sounds[i] = retain(buffer->readArray<SoundIndex>());
	}
}

VoiceData::~VoiceData() {
	for (int i = 0; i < VoiceCount; i++) {
		release(sounds[i]);
	}
}

SkinData::SkinData(CSBuffer* buffer) :
    smallIcon(buffer->readBoolean() ? ImageIndex(buffer) : ImageIndex::None),
    largeIcon(buffer->readBoolean() ? ImageIndex(buffer) : ImageIndex::None),
    illustration(buffer->readBoolean() ? AnimationIndex(buffer) : ImageIndex::None),
	commandIcon(buffer->readBoolean() ? ImageIndex(buffer) : ImageIndex::None),
    spawnAnimation(buffer->readBoolean() ? AnimationIndex(buffer) : AnimationIndex::None),
    name(CSObject::retain(buffer->readLocaleString())),
    grade(buffer->readByte()),
    previewUrl(CSObject::retain(buffer->readLocaleString())),
    ability(buffer),
    motions{}
{
    int motionCount = buffer->readLength();
    for (int i = 0; i < motionCount; i++) {
        int motion = buffer->readByte();
        CSAssert(!motions[motion], "invalid data");
        motions[motion] = new MotionData(buffer);
    }
    int effectCount = buffer->readLength();
    if (effectCount) {
        CSDictionary<AnimationIndex, AnimationIndex>* effects = new CSDictionary<AnimationIndex, AnimationIndex>(effectCount);
        for (int i = 0; i < effectCount; i++) {
            AnimationIndex from(buffer);
            AnimationIndex to(buffer);
            effects->setObject(from, to);
        }
        this->effects = effects;
    }
    else {
        this->effects = NULL;
    }
	int voiceCount = buffer->readLength();
	if (voiceCount) {
		CSDictionary<CSString, VoiceData>* voices = new CSDictionary<CSString, VoiceData>();
		for (int i = 0; i < voiceCount; i++) {
			VoiceData* voice = new VoiceData(buffer);
			int localeCount = buffer->readLength();
			for (int j = 0; j < localeCount; j++) {
				voices->setObject(buffer->readString(), voice);
			}
			voice->release();
		}
		this->voices = voices;
	}
	else {
		this->voices = NULL;
	}

    preloadAnimations = CSObject::retain(buffer->readArray<AnimationIndex>());
}

SkinData::~SkinData() {
    name->release();
    for (int i = 0; i < MotionCount; i++) {
        if (motions[i]) delete motions[i];
    }
	CSObject::release(effects);
	CSObject::release(previewUrl);
	CSObject::release(voices);
	CSObject::release(preloadAnimations);
}

const AnimationIndex& SkinData::effectIndex(const AnimationIndex& index) const {
    if (!effects) {
        return index;
    }
    const AnimationIndex* rtn = effects->tryGetObjectForKey(index);
    
    return rtn ? *rtn : index;
}

const VoiceData* SkinData::voice() const {
	const VoiceData* voice = NULL;

	if (voices) {
#ifdef UseVoiceLocale
		const CSString* locale = new CSString(UseVoiceLocale);
#else
		const CSString* locale = CSLocaleString::locale();
#endif
		voice = voices->objectForKey(locale);

		if (!voice) voice = voices->objectForKey(CSLocaleDefault);
	}

	return voice;
}

UnitData::UnitData(CSBuffer* buffer) :
    name(CSObject::retain(buffer->readLocaleString())),
    description(CSObject::retain(buffer->readLocaleString())),
    grade(buffer->readByte()),
	category(buffer->readByte()),
	type((ObjectType)buffer->readByte()),
	visible(buffer->readBoolean()),
	attribute(buffer->readByte()),
	attackType(buffer->readByte()),
	armorType(buffer->readByte()),
	subordinated(buffer->readBoolean()),
	population(buffer->readByte()),
	addition(buffer->readByte()),
    build(buffer->readBoolean()),
	weight(buffer->readByte()),
	resources{ (ushort)buffer->readShort(), (ushort)buffer->readShort() },
	progress(buffer->readShort()),
	order((resources[0] + resources[1] * 2) / (addition + 1)),
    collider(buffer->readFixed()),
    radius(buffer->readFloat()),
	body(buffer->readFloat()),
	top(buffer->readFloat()),
	z(buffer->readFloat()),
	minimap(buffer->readBoolean() ? AnimationIndex(buffer) : AnimationIndex::None),
	ability(buffer),
    children(CSObject::retain(buffer->readArray<UnitIndex>())),
    spellTrigger(CSObject::retain(buffer->readArray<byte, 2>())),
    actionTriggers(CSObject::retain(buffer->readArray<byte, 3>()))
{
    CSDictionary<CommandIndex, CommandUnitData>* commands = new CSDictionary<CommandIndex, CommandUnitData>();
    int commandCount = buffer->readLength();
    for (int i = 0; i < commandCount; i++) {
        CommandIndex key(buffer);
        CSAssert(!commands->containsKey(key), "invalid data");
        new (&commands->setObject(key)) CommandUnitData(buffer);
    }
    this->commands = commands;
    
    skins = CSObject::retain(buffer->readArray<SkinData>());
}

UnitData::~UnitData() {
    CSObject::release(name);
    CSObject::release(description);
    CSObject::release(children);
    CSObject::release(spellTrigger);
    CSObject::release(actionTriggers);
    commands->release();
    CSObject::release(skins);
}

const AnimationIndex& UnitData::effectIndex(int skin, const AnimationIndex& index) const {
    return skins ? skins->objectAtIndex(skin < skins->count() ? skin : 0).effectIndex(index) : index;
}
