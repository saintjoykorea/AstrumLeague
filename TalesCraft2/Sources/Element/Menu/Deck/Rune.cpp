//
//  Rune.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2017. 1. 18..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Rune.h"

#include "ForceBase.h"

Rune::Rune(const ForceBase* force, const RuneIndex& index, int level) :
	force(force),
    index(index),
    level(level)
{
}

Rune::Rune(CSBuffer* buffer, const ForceBase* force) :
	force(force),
    index(buffer),
    level(buffer->readShort())
{
    
}

Rune::Rune(const CSJSONObject* json, const ForceBase* force) :
	force(force),
    index(json->numberForKey("runeId")),
    level(json->numberForKey("level"))
{
}

void Rune::writeTo(CSBuffer* buffer) const {
    index.writeTo(buffer);
    buffer->writeShort(level);
}

const CSArray<AbilityDescription>* Rune::abilityValues(const UnitBase* unit) const {
	CSArray<AbilityDescription>* values = CSArray<AbilityDescription>::array();
	for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
		const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);

		if (data.display && (data.display->view & AbilityDisplayViewRune)) {
			fixed initial = abilityDisplayInitialValue(data.display->display);
			
			fixed value = ability(i, unit, initial, NULL);

			if (value != initial) {
				const CSString* valueStr = CSString::stringWithFormat("%s %s",
					value > initial ? "#color(00ff00ff) +" : "#color(ff0000ff)",
					(const char*)*abilityDisplayString(data.display->display, value));

				CSString* nameStr = CSString::string();
				if (this->data().prefix->value()) {
					nameStr->append(*this->data().prefix->value());
					nameStr->append(" ");
				}
				nameStr->append(*data.display->name->value());

				new (&values->addObject()) AbilityDescription(data.display->icon, nameStr, valueStr);
			}
			foreach(const AbilityTargetData*, subdata, this->data().ability.targets(i)) {
				fixed targetValue = ability(i, unit, initial, subdata->target);

				if (targetValue != value) {
					CSString* nameStr = CSString::string();
					if (this->data().prefix->value()) {
						nameStr->append(*this->data().prefix->value());
						nameStr->append(" ");
					}
					nameStr->append(*subdata->name);

					const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*nameStr, (const char*)*data.display->name);

					const CSString* valueStr = CSString::stringWithFormat("%s %s",
						targetValue > initial ? "#color(00ff00ff) +" : "#color(ff0000ff)",
						(const char*)*abilityDisplayString(data.display->display, targetValue));

					new (&values->addObject()) AbilityDescription(data.display->icon, name, valueStr);
				}
			}
		}
	}
	return values;
}

const CSArray<AbilityDescription>* Rune::abilityLevelupValues(bool differenceOnly, const UnitBase* unit) const {
	CSArray<AbilityDescription>* values = CSArray<AbilityDescription>::array();
	for (int i = 0; i < Asset::sharedAsset()->abilities->count(); i++) {
		const AbilityData& data = Asset::sharedAsset()->abilities->objectAtIndex(i);

		if (data.display && (data.display->view & AbilityDisplayViewRune)) {
			fixed initial = abilityDisplayInitialValue(data.display->display);

			fixed value0 = ability(i, unit, initial, NULL);
			const_cast<Rune*>(this)->level++;
			fixed value1 = ability(i, unit, initial, NULL);
			const_cast<Rune*>(this)->level--;

			if ((value0 != initial || value1 != initial) && (!differenceOnly || value0 != value1)) {
				new (&values->addObject()) AbilityDescription(data.display->icon, data.display->name->value(), abilityDisplayString(data.display->display, value0, value1));
			}
			foreach(const AbilityTargetData*, subdata, this->data().ability.targets(i)) {
				fixed targetValue0 = ability(i, unit, initial, subdata->target);
				const_cast<Rune*>(this)->level++;
				fixed targetValue1 = ability(i, unit, initial, subdata->target);
				const_cast<Rune*>(this)->level--;

				if ((targetValue0 != value0 || targetValue1 != value1) && (!differenceOnly || targetValue0 != targetValue1)) {
					const CSString* name = CSString::stringWithFormat("%s %s", (const char*)*subdata->name, (const char*)*data.display->name);

					new (&values->addObject()) AbilityDescription(data.display->icon, name, abilityDisplayString(data.display->display, targetValue0, targetValue1));
				}
			}
		}
	}
	return values;
}

const CSArray<AbilityDescription>* Rune::abilityDescriptions(const UnitBase* unit) const {
	CSArray<AbilityDescription>* descriptions = CSArray<AbilityDescription>::array();

	foreach(const AbilityDescriptionData&, subdata, data().ability.descriptions) {
		new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForRune(this, unit));
	}
	return descriptions;
}

const CSArray<AbilityDescription>* Rune::abilityLevelupDescriptions(const UnitBase* unit) const {
	CSArray<AbilityDescription>* descriptions = CSArray<AbilityDescription>::array();

	foreach(const AbilityDescriptionData&, subdata, data().ability.descriptions) {
		new (&descriptions->addObject()) AbilityDescription(ImageIndex::None, subdata.name->value(), subdata.stringForRuneLevelup(this, unit));
	}
	return descriptions;
}