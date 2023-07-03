//
//  Map+ui.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Map.h"

#include "Sound.h"

#include "Account.h"

#include "PVPGateway.h"

#include "Destination.h"

#include "LightEffect.h"
#include "StrokeEffect.h"
#include "LineEffect.h"

#define MaxSpeechBubbles    3

bool Map::select(uint mask, bool insight, bool excludeNumbering, bool commit) {
    AssertOnMainThread();

    if (_force && isActionAllowed(ActionRestrictionSelection)) {
        bool first = true;

        CSMatrix viewProjection;
        if (insight) {
            synchronized(_camera, GameLockMap) {
                viewProjection = _camera->capture().viewProjection();
            }
        }
        
        synchronized(_selectionLock, GameLockMap) {
            synchronized(_phaseLock, GameLockMap) {
                foreach(Unit*, unit, _units) {
                    if (unit->compareType(mask) && unit->isLocated() && controlStateSelection(unit) == MapControlStateEnabled && unit->force() == _force) {
						if (insight) {
                            CSVector3 p = unit->worldPoint(ObjectLayerMiddle);
                            float x, y, w;
                            x = (p.x * viewProjection.m11) + (p.y * viewProjection.m21) + (p.z * viewProjection.m31) + viewProjection.m41;
                            y = (p.x * viewProjection.m12) + (p.y * viewProjection.m22) + (p.z * viewProjection.m32) + viewProjection.m42;
                            w = 1.0f / ((p.x * viewProjection.m14) + (p.y * viewProjection.m24) + (p.z * viewProjection.m34) + viewProjection.m44);
                            x *= w;
                            y *= w;

                            if (x < -1 || x > 1 || y < -1 || y > 1) {
                                continue;
                            }
                        }
						if (excludeNumbering) {
							bool exclude = false;
							for (int i = 0; i < MaxNumberings && !exclude; i++) {
								foreach(const CSArray<Unit>*, numbering, _selection.numberings[i]) {
									if (numbering->containsObject(unit)) {
										exclude = true;
										break;
									}
								}
							}
							if (exclude) {
								continue;
							}
						}
                        if (commit) {
                            if (first) {
                                _selection.mask = insight ? 0 : mask;
                                _selection.following = false;
                                _selection.groupedSelections->removeAllObjects();
                                _selection.expandedSelections->removeAllObjects();
                                _selection.selections = _selection.expandedSelections;
                                CSObject::release(_selection.target);
                                first = false;
                            }
                        }
                        else {
                            return true;
                        }
                        bool flag = true;

                        int i;
                        
                        int order = unit->data().order;
                        
                        for (i = 0; i < _selection.groupedSelections->count(); i++) {
                            CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(i);
                            
                            const Unit* selection = selections->objectAtIndex(0);
                            
                            if (selection->index() == unit->index()) {
                                selections->addObject(unit);
                                flag = false;
                                break;
                            }
                            else if (selection->data().order < order) {
                                break;
                            }
                        }
                        if (flag) {
                            CSArray<Unit>* selections = new CSArray<Unit>();
                            selections->addObject(unit);
                            _selection.groupedSelections->insertObject(i, selections);
                            selections->release();
                        }
                        _selection.expandedSelections->addObject(unit);
                    }
                }
            }
            if (commit) {
				voice(VoiceSelect);
                passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
                passAction(ActionAllSelect, _selection.expandedSelections, NULL, NULL);
            }
        }
        return !first;
    }
    return false;
}

bool Map::dragCheck(const CSVector2 &vp) {
    AssertOnMainThread();
    
    if (controlState(MapControlDragSelect) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        foreach(Unit*, unit, targetsImpl(vp, DragCheckRange)) {
            if (unit->compareType(ObjectMaskWeapon) && controlStateSelection(unit) == MapControlStateEnabled) {
                if (!_selection.dragging) {
                    if (_playing == PlayingParticipate) {
                        if (unit->force() != _force) {
                            continue;
                        }
                    }
                    else {
                        observe(unit->force());
                    }
                    followScroll(false);
                    _selection.dragging = true;
                    setLightState(MapLightStateOff);
                }
                return true;
            }
        }
    }
    return false;
}

void Map::drag(const CSVector2 &vp) {
    AssertOnMainThread();
    
    CSAssert(_selection.dragging, "invalid state");
    
    if (_force) {
        foreach(Unit*, unit, targetsImpl(vp, DragRange)) {
            if (unit->compareType(ObjectMaskWeapon) && controlStateSelection(unit) == MapControlStateEnabled && unit->force() == _force && !_selection.dragTargets->containsObjectIdenticalTo(unit)) {
                _selection.dragEffects->addObject(LightEffect::effect(unit, EffectDisplayInfinite));
                _selection.dragEffects->addObject(StrokeEffect::effect(unit, EffectDisplayInfinite));
                _selection.dragTargets->addObject(unit);
            }
        }
    }
}

bool Map::dragEnd(bool merge) {
    AssertOnMainThread();
    
    CSAssert(_selection.dragging, "invalid state");
    
    _selection.dragging = false;
    
    setLightState(MapLightStateOn);
    
    if (!_selection.dragTargets->count()) {
        return false;
    }
    synchronized(_selectionLock, GameLockMap) {
        if (merge) {
            foreach(const Unit*, unit, _selection.expandedSelections) {
                if (!unit->compareType(ObjectMaskWeapon)) {
                    merge = false;
                    break;
                }
            }
        }
        if (!merge) {
            _selection.groupedSelections->removeAllObjects();
            _selection.expandedSelections->removeAllObjects();
            _selection.selections = _selection.expandedSelections;
        }
        _selection.mask = 0;
        
        foreach(Unit*, unit, _selection.dragTargets) {
            int i;
            
            bool isNew = true;
            
            int order = unit->data().order;
            
            for (i = 0; i < _selection.groupedSelections->count(); i++) {
                CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(i);
                
                const Unit* selection = selections->objectAtIndex(0);
                
                if (selection->index() == unit->index()) {
                    if (!selections->containsObjectIdenticalTo(unit)) {
                        selections->addObject(unit);
                    }
                    isNew = false;
                    break;
                }
                else if (selection->data().order < order) {
                    break;
                }
            }
            if (isNew) {
                CSArray<Unit>* selections = new CSArray<Unit>();
                selections->addObject(unit);
                _selection.groupedSelections->insertObject(i, selections);
                selections->release();
            }
            _selection.expandedSelections->addObject(unit);
            StrokeEffect::effect(unit, EffectDisplayVanish);
        }
        CSObject::release(_selection.target);
        
		voice(VoiceSelect);
        passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
        passAction(ActionDragSelect, _selection.expandedSelections, NULL, NULL);
    }
    
    foreach(Effect*, effect, _selection.dragEffects) {
        effect->dispose();
    }
    _selection.dragEffects->removeAllObjects();
    _selection.dragTargets->removeAllObjects();
    return true;
}

void Map::dragCancel() {
    AssertOnMainThread();
    
    CSAssert(_selection.dragging, "invalid state");
    
    foreach(Effect*, effect, _selection.dragEffects) {
        effect->dispose();
    }
    _selection.dragEffects->removeAllObjects();
    _selection.dragTargets->removeAllObjects();
    _selection.dragging = false;
    
    setLightState(MapLightStateOn);
}

void Map::focusSelection(int index, bool swap) {
    AssertOnMainThread();
    
    if (controlState(MapControlFocusSelection) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        synchronized(_selectionLock, GameLockMap) {
            if (index < _selection.groupedSelections->count()) {
                CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(index);
                
                if (_selection.selections == selections && swap) {
                    _selection.selections = _selection.expandedSelections;
                }
                else {
                    _selection.selections = selections;
                    foreach(Unit*, unit, selections) {
                        StrokeEffect::effect(unit, EffectDisplayVanish);
                    }
                    passAction(ActionFocusSelect, selections, NULL, NULL);
                }
                CSObject::release(_selection.target);
            }
        }
    }
}

void Map::cancelSelectionImpl() {
    _selection.mask = 0;
    _selection.following = false;
    _selection.groupedSelections->removeAllObjects();
    _selection.expandedSelections->removeAllObjects();
    _selection.selections = _selection.expandedSelections;
    CSObject::release(_selection.target);
}

void Map::cancelSelection() {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelSelection) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        synchronized(_selectionLock, GameLockMap) {
            cancelSelectionImpl();
        }
    }
}

void Map::cancelSelection(int index) {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelSelection) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        synchronized(_selectionLock, GameLockMap) {
            if (index < _selection.groupedSelections->count()) {
                passAction(ActionCancelSelect, _selection.groupedSelections->objectAtIndex(index), NULL, NULL);
                
                if (_selection.selections == _selection.groupedSelections->objectAtIndex(index)) {
                    _selection.selections = _selection.expandedSelections;
                }
                foreach(const Unit*, unit, _selection.groupedSelections->objectAtIndex(index)) {
                    _selection.expandedSelections->removeObjectIdenticalTo(unit);
                }
                _selection.groupedSelections->removeObjectAtIndex(index);
                CSObject::release(_selection.target);
                _selection.following = false;
            }
        }
    }
}
bool Map::selectNavigation(int index) {
    AssertOnMainThread();
    
    if (controlState(MapControlNavigation) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        synchronized(_selectionLock, GameLockMap) {
            if (index < _selection.navigations->count()) {
                Navigation* navigation = _selection.navigations->objectAtIndex(index);
                
                if (navigation->registeredUnits()->count()) {
                    CSArray<Unit>* units = CSArray<Unit>::arrayWithCapacity(navigation->registeredUnits()->count());
                    foreach(Unit*, u, navigation->registeredUnits()) {
                        if (u->isLocated() && controlStateSelection(u) == MapControlStateEnabled) {
                            units->addObject(u);
                        }
                    }
                    if (units->count()) {
                        cancelSelectionImpl();

                        _selection.groupedSelections->addObject(units);
                        _selection.expandedSelections->addObjectsFromArray(units);
                        
						voice(VoiceSelect);
                        passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
                        passAction(ActionSelectNavigation, _selection.expandedSelections, NULL, NULL);
                        
                        return true;
                    }
                }
                else if (navigation->preparedUnits()->count()) {
                    CSVector2 p;
                    synchronized(_camera, GameLockMap) {
						p = (CSVector2)_camera->capture().target() / TileSize;
                    }
                    Unit* unit = NULL;
                    float distance2 = FloatMax;
                    foreach(Unit*, u, navigation->preparedUnits()) {
                        if (u->isLocated() && controlStateSelection(u)) {
                            float d2 = p.distanceSquared(u->point());
                            
                            if (d2 < distance2) {
                                unit = u;
                                distance2 = d2;
                            }
                        }
                    }
                    if (unit) {
                        cancelSelectionImpl();

                        _selection.groupedSelections->addObject(CSArray<Unit>::arrayWithObjects(&unit, 1));
                        _selection.expandedSelections->addObject(unit);
                        
						voice(VoiceSelect);
                        passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
                        passAction(ActionSelectNavigation, _selection.expandedSelections, NULL, NULL);
                        
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
void Map::resetNavigation(Unit *unit, Navigating state) {
    const Force* playerForce = _force;
    
    if (playerForce && playerForce == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            int i;
            for (i = 0; i < _selection.navigations->count(); i++) {
                Navigation* navigation = _selection.navigations->objectAtIndex(i);
                if (navigation->index == unit->index()) {
                    switch (state) {
                        case NavigatingNone:
                            navigation->registeredUnits()->removeObjectIdenticalTo(unit);
                            navigation->preparedUnits()->removeObjectIdenticalTo(unit);
                            
                            if (!navigation->registeredUnits()->count() && !navigation->preparedUnits()->count()) {
                                _selection.navigations->removeObjectAtIndex(i);
                            }
                            break;
                        case NavigatingPrepared:
                            navigation->registeredUnits()->removeObjectIdenticalTo(unit);
                            if (!navigation->preparedUnits()->containsObjectIdenticalTo(unit)) {
                                navigation->preparedUnits()->addObject(unit);
                            }
                            break;
                        case NavigatingRegistered:
                            if (!navigation->registeredUnits()->containsObjectIdenticalTo(unit)) {
                                navigation->registeredUnits()->addObject(unit);
                            }
                            navigation->preparedUnits()->removeObjectIdenticalTo(unit);
                            break;
                    }
                    return;
                }
                else if ((int)navigation->index > (int)unit->index()) {
                    break;
                }
            }
            if (state != NavigatingNone) {
                Navigation* navigation = new Navigation(unit->index());
                _selection.navigations->insertObject(i, navigation);
                switch (state) {
                    case NavigatingPrepared:
                        navigation->preparedUnits()->addObject(unit);
                        break;
                    case NavigatingRegistered:
                        navigation->registeredUnits()->addObject(unit);
                        break;
                }
                navigation->release();
            }
        }
    }
}
bool Map::selectNumbering(int index) {
    CSAssert(index >= 0 && index < MaxNumberings, "invalid operation");
    
    if (controlState(MapControlNumbering) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        synchronized(_selectionLock, GameLockMap) {
            if (_selection.numberings[index]->count()) {
                cancelSelectionImpl();
                foreach(CSArray<Unit>*, numbering, _selection.numberings[index]) {
                    CSArray<Unit>* selections = new CSArray<Unit>(numbering->count());
                    foreach(Unit*, unit, numbering) {
                        if (unit->isLocated() && controlStateSelection(unit) == MapControlStateEnabled) {
                            selections->addObject(unit);
                            _selection.expandedSelections->addObject(unit);
                            StrokeEffect::effect(unit, EffectDisplayVanish);
                        }
                    }
					if (selections->count()) {
						_selection.groupedSelections->addObject(selections);
					}
                    selections->release();
                }
				voice(VoiceSelect);
                passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
                passAction(ActionSelectNumbering, _selection.expandedSelections, NULL, NULL);
                
                return true;
            }
        }
    }
    return false;
}
void Map::setNumbering(int index, bool merge) {
    CSAssert(index >= 0 && index < MaxNumberings, "invalid operation");
    
    if (controlState(MapControlNumbering) == MapControlStateEnabled) {
        synchronized(_selectionLock, GameLockMap) {
            if (_selection.selections->count()) {
                if (merge && _selection.numberings[index]->count()) {
                    bool isWeapon0 = false;
                    bool isWeapon1 = false;
                    foreach(const Unit*, unit, _selection.numberings[index]->objectAtIndex(0)) {
                        if (unit->compareType(ObjectMaskWeapon)) {
                            isWeapon0 = true;
                            break;
                        }
                    }
                    foreach(const Unit*, unit, _selection.selections) {
                        if (unit->compareType(ObjectMaskWeapon)) {
                            isWeapon1 = true;
                            break;
                        }
                    }
                    if (isWeapon0 != isWeapon1) {
                        merge = false;
                    }
                }
                
                if (!merge) _selection.numberings[index]->removeAllObjects();
                
                foreach(Unit*, unit, _selection.selections) {
                    int order = unit->data().order;
                    bool isNew = true;
                    int i;
                    for (i = 0; i < _selection.numberings[index]->count(); i++) {
                        CSArray<Unit>* numbering = _selection.numberings[index]->objectAtIndex(i);
                        
                        const Unit* numberingUnit = numbering->objectAtIndex(0);
                        
                        if (numberingUnit->index() == unit->index()) {
                            if (!numbering->containsObjectIdenticalTo(unit)) {
                                numbering->addObject(unit);
                            }
                            isNew = false;
                            break;
                        }
                        else if (numberingUnit->data().order < order) {
                            break;
                        }
                    }
                    if (isNew) {
                        CSArray<Unit>* numbering = new CSArray<Unit>();
                        numbering->addObject(unit);
                        _selection.numberings[index]->insertObject(i, numbering);
                        numbering->release();
                    }
                }
            }
            passAction(ActionSetNumbering, _selection.selections, NULL, NULL);
        }
        if (merge) {
            selectNumbering(index);
        }
    }
}
void Map::cancelNumbering(int index) {
    CSAssert(index >= 0 && index < MaxNumberings, "invalid operation");
    
    if (controlState(MapControlCancelNumbering) == MapControlStateEnabled) {
        synchronized(_selectionLock, GameLockMap) {
            _selection.numberings[index]->removeAllObjects();
        }
        passAction(ActionCancelNumbering, NULL, NULL, NULL);
    }
}
void Map::removeNumbering(const Unit* unit) {
    synchronized(_selectionLock, GameLockMap) {
        for (int i = 0; i < MaxNumberings; i++) {
            for (int j = 0; j < _selection.numberings[i]->count(); j++) {
                CSArray<Unit>* numbering = _selection.numberings[i]->objectAtIndex(j);
                if (numbering->removeObjectIdenticalTo(unit)) {
                    if (numbering->count() == 0) {
                        _selection.numberings[i]->removeObjectAtIndex(j);
                    }
                    break;
                }
            }
        }
    }
}
void Map::resetNumbering(Unit* unit) {
    synchronized(_selectionLock, GameLockMap) {
        for (int i = 0; i < MaxNumberings; i++) {
            bool exists = false;
            for (int j = 0; j < _selection.numberings[i]->count(); j++) {
                CSArray<Unit>* numbering = _selection.numberings[i]->objectAtIndex(j);
                if (numbering->removeObjectIdenticalTo(unit)) {
                    if (numbering->count() == 0) {
                        _selection.numberings[i]->removeObjectAtIndex(j);
                    }
                    exists = true;
                    break;
                }
            }
            if (exists) {
                int order = unit->data().order;
                bool isNew = true;
                int j;
                for (j = 0; j < _selection.numberings[i]->count(); j++) {
                    CSArray<Unit>* numbering = _selection.numberings[i]->objectAtIndex(j);
                    
                    const Unit* numberingUnit = numbering->objectAtIndex(0);
                    
                    if (numberingUnit->index() == unit->index()) {
                        numbering->addObject(unit);
                        isNew = false;
                        break;
                    }
                    else if (numberingUnit->data().order < order) {
                        break;
                    }
                }
                if (isNew) {
                    CSArray<Unit>* numbering = new CSArray<Unit>();
                    numbering->addObject(unit);
                    _selection.numberings[i]->insertObject(j, numbering);
                    numbering->release();
                }
            }
        }
    }
}

void Map::addSelectionImpl(Unit *unit) {
    if (!_selection.expandedSelections->containsObjectIdenticalTo(unit)) {
        bool flag = true;
        
        int i;
        
        int order = unit->data().order;
        
        for (i = 0; i < _selection.groupedSelections->count(); i++) {
            CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(i);
            
            const Unit* selection = selections->objectAtIndex(0);
            
            if (selection->index() == unit->index()) {
                selections->addObject(unit);
                flag = false;
                break;
            }
            else if (selection->data().order < order) {
                break;
            }
        }
        if (flag) {
            CSArray<Unit>* selections = new CSArray<Unit>();
            selections->addObject(unit);
            _selection.groupedSelections->insertObject(i, selections);
            selections->release();
        }
        _selection.expandedSelections->addObject(unit);
    }
}
void Map::addSelection(Unit *unit) {
    const Force* playerForce = _force;
    
    if (controlStateSelection(unit) == MapControlStateEnabled && playerForce && playerForce == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            addSelectionImpl(unit);
        }
    }
}
void Map::autoSelection(Unit *unit) {
    synchronized(_selectionLock, GameLockMap) {
        if (_selection.mask && unit->compareType(_selection.mask) && controlStateSelection(unit) == MapControlStateEnabled) {
            const Force* playerForce = _force;
            if (playerForce && playerForce == unit->force()) {
                addSelectionImpl(unit);
            }
        }
    }
}
bool Map::removeSelection(const Object *obj) {
    bool rtn = false;
    
    synchronized(_selectionLock, GameLockMap) {
        if (obj == _selection.target) {
            CSObject::release(_selection.target);
            rtn = true;
        }
        if (obj->compareType(ObjectMaskUnit)) {
            const Unit* unit = static_cast<const Unit*>(obj);
            
            for (int i = 0; i < _selection.groupedSelections->count(); i++) {
                CSArray<Unit>* selections = _selection.groupedSelections->objectAtIndex(i);
                if (selections->objectAtIndex(0)->index() == unit->index()) {
                    selections->removeObjectIdenticalTo(unit);
                    if (selections->count() == 0) {
                        if (selections == _selection.selections) {
                            _selection.selections = _selection.expandedSelections;
                        }
                        _selection.groupedSelections->removeObjectAtIndex(i);
                    }
                    rtn = true;
                    break;
                }
            }
            _selection.expandedSelections->removeObjectIdenticalTo(unit);
            
            if (!_selection.expandedSelections->count() && isSingle()) {
                _mode.context.single.actionRestriction &= ~ActionRestrictionSelection;
            }
        }
    }
    return rtn;
}
void Map::cancelTarget() {
    synchronized(_selectionLock, GameLockMap) {
        CSObject::release(_selection.target);
    }
}
void Map::updateSelection() {
    synchronized(_selectionLock, GameLockMap) {
        if (_selection.following && _selection.selections->count() && controlState(MapControlScroll) == MapControlStateEnabled) {
            CSVector2 p = convertMapToFlatWorldSpace(_selection.selections->objectAtIndex(0)->point());
            synchronized(_camera, GameLockMap) {
                if (controlState(MapControlHorizontalScroll) != MapControlStateEnabled) {
                    p.x = _camera->capture().target().x;
                }
                if (controlState(MapControlVerticalScroll) != MapControlStateEnabled) {
                    p.y = _camera->capture().target().y;
                }
                _camera->setScroll(p);
            }
        }
        if (_selection.target && !_selection.target->isVisible(vision())) {
            _selection.target->release();
            _selection.target = NULL;
        }
        int i = 0;
        while (i < _selection.clickEffects->count()) {
            Effect* effect = _selection.clickEffects->objectAtIndex(i);
            if (effect->isVisible()) i++;
            else _selection.clickEffects->removeObjectAtIndex(i);
        }
    }
}

void Map::healthRate(float& playerRate, float& enemyRate) const {
    const Force* force = _force;

    if (force) {
        fixed playerHealth = fixed::Zero;
        fixed playerMaxHealth = fixed::Zero;
        fixed enemyHealth = fixed::Zero;
        fixed enemyMaxHealth = fixed::Zero;

        synchronized(_phaseLock, GameLockMap) {
            foreach(const Unit*, unit, _units) {
                if (unit->force() && unit->compareType(ObjectMaskBase)) {
                    if (unit->force() == force) {
                        playerHealth += unit->health();
                        playerMaxHealth += unit->maxHealth();
                    }
                    else if (unit->force()->alliance() != force->alliance()) {
                        enemyHealth += unit->health();
                        enemyMaxHealth += unit->maxHealth();
                    }
                }
            }
        }
        playerRate = playerHealth ? (float)(playerHealth / playerMaxHealth) : 0;
        enemyRate = enemyHealth ? (float)(enemyHealth / enemyMaxHealth) : 0;
    }
    else {
        playerRate = 0;
        enemyRate = 0;
    }
}

const CSArray<Unit>* Map::commandUnits(const CommandIndex& index, const Object* target) const {
    CSArray<Unit>* units = NULL;
    
	int selectionFlag = 0;
	synchronized(_selectionLock, GameLockMap) {
		units = CSArray<Unit>::arrayWithArray(_selection.selections, false);		//커맨드 트리거동작이 크므로 동기화시간 절약을 위해 카피본을 사용한다. 
		if (!units->count()) return units;
		if (_selection.selections == _selection.expandedSelections && _selection.groupedSelections->count() > 1) selectionFlag |= CommandSelectionFlagMultiGroup;
		if (_selection.mask) selectionFlag |= CommandSelectionFlagAll;
	}

	if (units->count() >= 2 && target) {
		bool swap;

		do {
			swap = false;

			fixed d0 = target->point().distanceSquared(units->objectAtIndex(0)->point());
			for (int i = 1; i < units->count(); i++) {
				fixed d1 = target->point().distanceSquared(units->objectAtIndex(i)->point());

				if (d0 > d1) {
					Unit* unit = units->objectAtIndex(i - 1);
					unit->retain();
					units->removeObjectAtIndex(i - 1);
					units->insertObject(i, unit);
					unit->release();

					swap = true;
				}
				else {
					d0 = d1;
				}
			}
		} while (swap);
	}

    fixed estimation = fixed::Zero;
            
	int i = 0;

	while (i < units->count()) {
		Unit* unit = units->objectAtIndex(i);

		CommandReturn rtn = unit->command(index, const_cast<Object*>(target), estimation, selectionFlag, 0, false);

		bool all;

		switch (rtn.state) {
			case CommandStateTarget:
				all = rtn.parameter.target.all;
				break;
			case CommandStateRange:
				all = rtn.parameter.range.all;
				break;
			case CommandStateAngle:
				all = rtn.parameter.angle.all;
				break;
			case CommandStateLine:
				all = rtn.parameter.line.all;
				break;
			case CommandStateProduce:
			case CommandStateTrain:
			case CommandStateBuild:
				all = true;
				break;
			case CommandStateEnabled:
				all = rtn.parameter.enabled.all;
				break;
			default:
				units->removeObjectAtIndex(i);
				continue;
		}
		if (all) i++;
		else {
			unit->retain();
			units->removeAllObjects();
			units->addObject(unit);
			unit->release();
			break;
		}
    }
	return units;
}

CommandReturn Map::commandImpl(CSArray<Unit>* units, const CommandIndex& index, Object *target, const FPoint* point, int selectionFlag, CommandCommit commitType) {
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    
    if (units->count()) {
        CSArray<Object*>* targets = NULL;
        
		int commit = commitType != CommandCommitNone;
		bool retry = commitType >= CommandCommitRun;
		bool stack = commitType == CommandCommitStack;

        if (commit) {
            if (units->count() >= 2) {
                const FPoint* origin;
                
				if (target) origin = &target->point();
                else if (point) origin = point;
				else origin = NULL;

				if (origin) {
					bool swap;

					do {
						swap = false;

						fixed d0 = origin->distanceSquared(units->objectAtIndex(0)->point());
						for (int i = 1; i < units->count(); i++) {
							fixed d1 = origin->distanceSquared(units->objectAtIndex(i)->point());

							if (d0 > d1) {
								Unit* unit = units->objectAtIndex(i - 1);
								unit->retain();
								units->removeObjectAtIndex(i - 1);
								units->insertObject(i, unit);
								unit->release();

								swap = true;
							}
							else {
								d0 = d1;
							}
						}
					} while (swap);
				}

				if (!target && point) {
					targets = CSArray<Object*>::arrayWithCapacity(units->count());

					FPoint cp(fixed::Zero, fixed::Zero);
					fixed massiveCollider = fixed::Zero;
					foreach(const Unit*, unit, units) {
						cp += unit->point();
						fixed collider = unit->collider();
						massiveCollider += collider * collider;
					}
					cp /= units->count();
					massiveCollider = CSMath::sqrt(massiveCollider);
					
					foreach(const Unit*, unit, units) {
						FPoint diff = unit->point() - cp;
						if (massiveCollider < diff.length()) {
							diff.normalize();
							diff *= massiveCollider;
						}
						FPoint dp = *point + diff;
						if (locatablePosition(unit->type(), dp, unit->collider(), NULL, MapLocationEnabled) == MapLocationUnabled) {
							dp = *point;
						}
						Destination* eachTarget = Destination::destination(unit->force());
						eachTarget->locate(dp);
						targets->addObject(eachTarget);
					}
				}
            }
            else if (!target && point) {
                target = Destination::destination(units->objectAtIndex(0)->force());
                target->locate(*point);
            }
        }

        for (; ; ) {
            fixed estimation = fixed::Zero;
            
            for (int i = 0; i < units->count(); i++) {
                Unit* unit = units->objectAtIndex(i);
                
                CommandReturn crtn = unit->command(index, targets ? targets->objectAtIndex(i) : target, estimation, selectionFlag, commit, stack);
                
                if (crtn.state == CommandStateEnabled) {
                    rtn = crtn;
                    
                    if (!crtn.parameter.enabled.all) {
                        return rtn;
                    }
                }
                else if (crtn.state == rtn.state) {
                    switch (crtn.state) {
                        case CommandStateCooltime:
                            if (crtn.parameter.cooltime.remaining < rtn.parameter.cooltime.remaining) {
                                rtn = crtn;
                            }
                            break;
                        case CommandStateRetry:
                            if (crtn.parameter.retry.remaining < rtn.parameter.retry.remaining) {
                                rtn = crtn;
                            }
                            break;
                    }
                }
                else if (crtn.state > rtn.state) {
                    rtn = crtn;
                }
            }
            if (retry && rtn.state == CommandStateRetry) {
                commit = rtn.parameter.retry.remaining + 1;
                rtn.state = CommandStateEmpty;
            }
            else {
                break;
            }
        }
    }
    return rtn;
}

CommandReturn Map::command(const CommandIndex& index, const Object *target, CommandCommit commit) {
    AssertOnMainThread();
    
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    
    if (!isActionAllowed(ActionRestrictionCommand)) {
        return rtn;
    }

    CommandReturn srtn;
    srtn.state = CommandStateEnabled;
    
    const ActionTarget* stateTarget;
    switch (controlStateCommand(index, &stateTarget)) {
        case MapControlStateHidden:
            srtn.state = CommandStateEmpty;
            commit = CommandCommitNone;
            break;
        case MapControlStateUnabled:
            srtn.state = CommandStateUnabled;
            srtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandDeactivated);
			srtn.parameter.unabled.announce = AnnounceNone;
            commit = CommandCommitNone;
            break;
        case MapControlStateEnabled:
            if (commit && stateTarget && (!target || !stateTarget->accept(target, target->point()))) {
                srtn.state = CommandStateUnabled;
                srtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
				srtn.parameter.unabled.announce = AnnounceNone;
                commit = CommandCommitNone;
            }
            break;
    }
    CSArray<Unit>* sources = NULL;

    int selectionFlag = 0;

    synchronized(_selectionLock, GameLockMap) {
        sources = new CSArray<Unit>(_selection.selections, false);

        if (_selection.selections == _selection.expandedSelections && _selection.groupedSelections->count() > 1) selectionFlag |= CommandSelectionFlagMultiGroup;
        if (_selection.mask) selectionFlag |= CommandSelectionFlagAll;
    }
    
    if (_mode.type == GameModeTypeOnline) {
        rtn = commandImpl(sources, index, const_cast<Object*>(target), NULL, selectionFlag, CommandCommitNone);
        
        if (commit) {
            synchronized(_selectionLock, GameLockMap) {
                CSObject::release(_selection.target);
            }
            
            switch (rtn.state) {
                case CommandStateTarget:
                    if (rtn.parameter.target.condition) {
                        rtn.state = CommandStateEnabled;
                        rtn.parameter.enabled.all = rtn.parameter.target.all;
                        goto enabled;
                    }
                    else {
                        rtn.state = CommandStateUnabled;
                        rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
						rtn.parameter.unabled.announce = AnnounceNone;
                    }
                    break;
                case CommandStateRange:
                    if (rtn.parameter.range.condition) {
                        rtn.state = CommandStateEnabled;
                        rtn.parameter.enabled.all = rtn.parameter.range.all;
                        goto enabled;
                    }
                    else {
                        rtn.state = CommandStateUnabled;
                        rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
						rtn.parameter.unabled.announce = AnnounceNone;
                    }
                    break;
                case CommandStateAngle:
                    if (rtn.parameter.angle.condition) {
                        rtn.state = CommandStateEnabled;
                        rtn.parameter.enabled.all = rtn.parameter.angle.all;
                        goto enabled;
                    }
                    else {
                        rtn.state = CommandStateUnabled;
                        rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
						rtn.parameter.unabled.announce = AnnounceNone;
                    }
                    break;
                case CommandStateLine:
                    if (rtn.parameter.line.condition) {
                        rtn.state = CommandStateEnabled;
                        rtn.parameter.enabled.all = rtn.parameter.line.all;
                        goto enabled;
                    }
                    else {
                        rtn.state = CommandStateUnabled;
                        rtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
						rtn.parameter.unabled.announce = AnnounceNone;
                    }
                    break;
                case CommandStateProduce:
                case CommandStateTrain:
                case CommandStateBuild:
                    rtn.state = CommandStateEnabled;
                    rtn.parameter.enabled.all = false;
                case CommandStateEnabled:
                enabled:
                    if (_playing == PlayingParticipate) {
                        writeCommand(_mode.context.online.localBuffer, sources->asReadOnly(), index, target, NULL);
                    }
                    break;
            }
        }
    }
    else {
		if (_playing != PlayingParticipate) commit = CommandCommitNone;

        if (commit) {
            _updateLock.lock(GameLockMap);
        }
        rtn = commandImpl(sources, index, const_cast<Object*>(target), NULL, selectionFlag, commit);
        
        if (commit) {
            synchronized(_selectionLock, GameLockMap) {
                CSObject::release(_selection.target);
            }
            
            switch (rtn.state) {
                case CommandStateProduce:
                case CommandStateTrain:
                case CommandStateBuild:
                    rtn.state = CommandStateEnabled;
                    rtn.parameter.enabled.all = false;
                case CommandStateEnabled:
                    break;
            }
			_updateLock.unlock();
		}
        if (srtn.state != CommandStateEnabled && rtn.state != CommandStateEmpty) {
            rtn = srtn;
        }
        if (commit && rtn.state == CommandStateEnabled) {
            passAction(ActionCommand, sources, &index, target);
        }
    }
    sources->release();
    
    return rtn;
}

const Unit* Map::spawnTarget(const Spawn* src, const byte* condition) {
    CSVector3 p0;
    CSMatrix viewProjection;
    synchronized(_camera, GameLockMap) {
        const CSCamera& capture = _camera->capture();
        p0 = capture.target();
        viewProjection = capture.viewProjection();
    }
    float distance2 = FloatMax;

    Unit* target = NULL;

    synchronized(_phaseLock, GameLockMap) {
        foreach(Unit*, unit, _units) {
            if (unit->isVisible(_force) && unit->match(src, condition)) {
                CSVector3 p1 = unit->worldPoint(ObjectLayerMiddle);
                float x, y, w;
                x = (p1.x * viewProjection.m11) + (p1.y * viewProjection.m21) + (p1.z * viewProjection.m31) + viewProjection.m41;
                y = (p1.x * viewProjection.m12) + (p1.y * viewProjection.m22) + (p1.z * viewProjection.m32) + viewProjection.m42;
                w = 1.0f / ((p1.x * viewProjection.m14) + (p1.y * viewProjection.m24) + (p1.z * viewProjection.m34) + viewProjection.m44);
                x *= w;
                y *= w;

                if (x >= -1 && x <= 1 && y >= -1 && y <= 1) {
                    float d2 = p0.distanceSquared(p1);
                    if (d2 < distance2) {
                        target = unit;
                        distance2 = d2;
                    }
                }
            }
        }
    }
    return target;
}

CommandReturn Map::spawn(int slot, const Object* target, SpawnCommit commit) {
    AssertOnMainThread();
    
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    
    if (!isActionAllowed(ActionRestrictionSpawn)) {
        return rtn;
    }
    if (_force && slot < _force->spawnCount()) {
        Spawn* spawn = _force->spawn(slot);
        
        CommandReturn srtn;
        srtn.state = CommandStateEnabled;
        
        const ActionTarget* stateTarget;
        switch (controlStateSpawn(spawn->index(), &stateTarget)) {
            case MapControlStateHidden:
                srtn.state = CommandStateEmpty;
                commit = SpawnCommitNone;
                break;
            case MapControlStateUnabled:
                srtn.state = CommandStateUnabled;
                srtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandDeactivated);
				srtn.parameter.unabled.announce = AnnounceNone;
                commit = SpawnCommitNone;
                break;
            case MapControlStateEnabled:
				if (commit && stateTarget && (!target || !stateTarget->accept(target, target->point()))) {
                    srtn.state = CommandStateUnabled;
                    srtn.parameter.unabled.msg = MessageIndex(MessageSetCommand, MessageCommandInvalidTarget);
					srtn.parameter.unabled.announce = AnnounceNone;
                    commit = SpawnCommitNone;
                }
                break;
        }
        if (commit == SpawnCommitRunAuto) {
            rtn = spawn->run(const_cast<Object*>(target), false);

            switch (rtn.state) {
                case CommandStateTarget:
                    target = spawnTarget(spawn, rtn.parameter.target.condition);
                    break;
                case CommandStateRange:
                    target = spawnTarget(spawn, rtn.parameter.range.condition);
                    if (target) {
                        Destination* dest = Destination::destination(_force);
                        dest->locate(target->point());
                        target = dest;
                    }
                    break;
            }       
        }
        if (_mode.type == GameModeTypeOnline) {
            rtn = spawn->run(const_cast<Object*>(target), false);
            
            if (commit) {
                if (rtn.state == CommandStateEnabled && _playing == PlayingParticipate) {
                    writeSpawn(_mode.context.online.localBuffer, slot, target);
                }
                synchronized(_selectionLock, GameLockMap) {
                    CSObject::release(_selection.target);
                }
            }
        }
        else {
            bool exec = _playing == PlayingParticipate && commit;
            if (exec) {
                _updateLock.lock(GameLockMap);
            }
            rtn = spawn->run(const_cast<Object*>(target), exec);
            
            if (commit) {
                synchronized(_selectionLock, GameLockMap) {
                    CSObject::release(_selection.target);
                }
                if (rtn.state == CommandStateEnabled && _playing == PlayingParticipate) {
                    _force->rollSpawn(slot);
                }
            }
            if (exec) {
                _updateLock.unlock();
            }
        }
        if (srtn.state != CommandStateEnabled && rtn.state != CommandStateEmpty) {
            rtn = srtn;
        }
        if (_playing == PlayingParticipate && commit && rtn.state == CommandStateEnabled) {
            passAction(ActionSpawn, NULL, NULL, target);
        }
    }
    return rtn;
}

uint Map::unitCount(const UnitIndex& index) const {
    uint count = 0;
    
    const Force* playerForce = _force;
    
    if (playerForce) {
        synchronized_to_update_thread(_phaseLock, GameLockMap) {
            foreach(const Unit*, unit, _units) {
                if (unit->force() == playerForce && unit->index() == index) {
                    count++;
                }
            }
        }
    }
    return count;
}

uint Map::producingCount(const UnitIndex& index) const {
    uint count = 0;
    
    synchronized(_selectionLock, GameLockMap) {
        foreach(const Unit*, unit, _selection.selections) {
            count += unit->producingCount(index);
        }
    }
    return count;
}

void Map::cancelProducingCommand(const UnitIndex& index) {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelProducingCommand) == MapControlStateEnabled && isActionAllowed(ActionRestrictionCommand) && _playing == PlayingParticipate) {
        Unit* unit = NULL;
        synchronized(_selectionLock, GameLockMap) {
            CSObject::release(_selection.target);

            foreach(Unit*, otherUnit, _selection.selections) {
                if (otherUnit->producingCount(index)) {
                    unit = otherUnit;
                    unit->retain();

                    passAction(ActionCancelProducingCommand, _selection.selections, NULL, NULL);
                    
                    break;
                }
            }
        }
        if (unit) {
            if (_mode.type == GameModeTypeOnline) {
                writeCancelProducingCommand(_mode.context.online.localBuffer, _selection.selections->asReadOnly(), index);
            }
            else {
                synchronized(_updateLock, GameLockMap) {
                    unit->cancelProducingCommand(index);
                }
            }
            unit->release();
        }
    }
}

void Map::cancelBuildingCommand(Unit* unit) {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelBuildingCommand) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection) && isActionAllowed(ActionRestrictionCommand) && _playing == PlayingParticipate && _force == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            CSObject::release(_selection.target);
        }
        if (_mode.type == GameModeTypeOnline) {
            writeCancelBuildingCommand(_mode.context.online.localBuffer, unit);
        }
        else {
            synchronized(_updateLock, GameLockMap) {
                unit->cancelBuildingCommand();
            }
        }
        passAction(ActionCancelBuildingCommand, CSArray<Unit>::arrayWithObjects(&unit, 1), NULL, NULL);
    }
}

void Map::cancelProducingCommand(Unit* unit, int slot) {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelProducingCommand) == MapControlStateEnabled && isActionAllowed(ActionRestrictionCommand) && _playing == PlayingParticipate && _force == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            CSObject::release(_selection.target);
        }
        if (_mode.type == GameModeTypeOnline) {
            writeCancelProducingCommand(_mode.context.online.localBuffer, unit, slot);
        }
        else {
            synchronized(_updateLock, GameLockMap) {
                unit->cancelProducingCommand(slot);
            }
        }
        passAction(ActionCancelProducingCommand, CSArray<Unit>::arrayWithObjects(&unit, 1), NULL, NULL);
    }
}
void Map::cancelTrainingCommand(Unit* unit, int slot) {
    AssertOnMainThread();
    
    if (controlState(MapControlCancelTrainingCommand) == MapControlStateEnabled && isActionAllowed(ActionRestrictionCommand) && _playing == PlayingParticipate && _force == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            CSObject::release(_selection.target);
        }
        if (_mode.type == GameModeTypeOnline) {
            writeCancelTrainingCommand(_mode.context.online.localBuffer, unit, slot);
        }
        else {
            synchronized(_updateLock, GameLockMap) {
                unit->cancelTrainingCommand(slot);
            }
        }
        passAction(ActionCancelTrainingCommand, CSArray<Unit>::arrayWithObjects(&unit, 1), NULL, NULL);
    }
}
void Map::unloadCommand(Unit* unit, int slot) {
    AssertOnMainThread();
    
    if (controlState(MapControlUnloadCommand) == MapControlStateEnabled && isActionAllowed(ActionRestrictionCommand) && _playing == PlayingParticipate && _force == unit->force()) {
        synchronized(_selectionLock, GameLockMap) {
            CSObject::release(_selection.target);
        }
        if (_mode.type == GameModeTypeOnline) {
            writeUnloadCommand(_mode.context.online.localBuffer, unit, slot);
        }
        else {
            synchronized(_updateLock, GameLockMap) {
                unit->unloadCommand(slot);
            }
        }
        passAction(ActionUnloadCommand, CSArray<Unit>::arrayWithObjects(&unit, 1), NULL, NULL);
    }
}

bool Map::click(const CSVector2& vp, ClickCommand command, bool all, bool merge) {
    AssertOnMainThread();
    
    Object* target = this->targetImpl(vp);
    
    if (_playing == PlayingParticipate && command) {
#ifdef UseObserverPlayTest
        if (target && target->compareType(ObjectMaskUnit)) {
            Unit* unit = static_cast<Unit*>(target);
            if (unit->force() && unit->force() != _force) {
                goto select;
            }
        }
#endif
        CSArray<Unit>* selections = NULL;
        
        int selectionFlag = 0;
        synchronized(_selectionLock, GameLockMap) {
            if (!_selection.selections->count()) {
                goto select;
            }
            selections = CSArray<Unit>::arrayWithArray(_selection.selections, false);

            if (_selection.selections == _selection.expandedSelections && _selection.groupedSelections->count() > 1) selectionFlag |= CommandSelectionFlagMultiGroup;
            if (_selection.mask) selectionFlag |= CommandSelectionFlagAll;
        }
        {
            bool enforcing = command == ClickCommandMove && !target;
            
            CommandIndex index(CommandSetTarget, enforcing);
            
            const ActionTarget* stateTarget;
            if (controlStateCommand(index, &stateTarget) == MapControlStateEnabled && isActionAllowed(ActionRestrictionCommand)) {
                FPoint point;
                FPoint* pointptr;
                if (target) {
                    pointptr = NULL;
                    point = target->point();
                }
                else if (convertViewToMapSpace(vp, point, true)) {
                    pointptr = &point;
                }
                else {
                    return false;
                }
                
                if (stateTarget && !stateTarget->accept(target, point)) {
                    return false;
                }
                
                bool commanded = false;
                
                if (_mode.type == GameModeTypeOnline) {
                    CommandReturn crtn = commandImpl(selections, index, target, pointptr, selectionFlag, CommandCommitNone);
                    
                    switch(crtn.state) {
                        case CommandStateEnabled:
                            commanded = true;
                            writeCommand(_mode.context.online.localBuffer, selections->asReadOnly(), index, target, &point);
                            break;
                        case CommandStateUnabled:
							if (crtn.parameter.unabled.msg) {
								pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), Asset::sharedAsset()->string(crtn.parameter.unabled.msg)));
							}
							if (crtn.parameter.unabled.announce != AnnounceNone) {
								announce(crtn.parameter.unabled.announce);
							}
                            return false;
                    }
                }
                else {
                    synchronized(_updateLock, GameLockMap) {
                        CommandReturn crtn = commandImpl(selections, index, target, pointptr, selectionFlag, CommandCommitRun);
                        
                        switch(crtn.state) {
                            case CommandStateEnabled:
                                commanded = true;
                                break;
                            case CommandStateUnabled:
								if (crtn.parameter.unabled.msg) {
									pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconNotice), Asset::sharedAsset()->string(crtn.parameter.unabled.msg)));
								}
								if (crtn.parameter.unabled.announce != AnnounceNone) {
									announce(crtn.parameter.unabled.announce);
								}
                                return false;
                        }
                    }
                }
                if (commanded) {
                    if (controlState(MapControlClickSelect) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
                        if (target && target->compareType(ObjectMaskUnit) ? controlStateSelection(static_cast<Unit*>(target)) : controlState(MapControlSelect) == MapControlStateEnabled) {
                            synchronized(_selectionLock, GameLockMap) {
                                CSObject::retain(_selection.target, target);
                            }
                        }
                    }
                    synchronized(_selectionLock, GameLockMap) {
                        foreach(Effect*, prevEffect, _selection.clickEffects) {
                            prevEffect->dispose();
                        }
                        _selection.clickEffects->removeAllObjects();
                        
                        if (target) {
                            StrokeEffect::effect(target, EffectDisplayVanish);
                        }
                        else {
                            target = Destination::destination(Map::sharedMap()->force());
                            target->locate(point);
                            _selection.clickEffects->addObject(AnimationEffect::effect(AnimationIndex(AnimationSetGame, AnimationGameTargetCommand_0 + enforcing), target, NULL, false, true, true));
                        }
                        foreach(Unit*, unit, selections) {
                            _selection.clickEffects->addObject(LineEffect::effect(unit, target, ImageIndex::None, enforcing ? Asset::playerColor0 : Asset::enemyColor, EffectDisplayVanish));
                        }
                    }

					voice(VoiceCommand);

                    passAction(ActionCommand, selections, &index, target);
                    
                    return false;
                }
            }
        }
    }
    
select:
    
    if (controlState(MapControlClickSelect) == MapControlStateEnabled && isActionAllowed(ActionRestrictionSelection)) {
        if (!target) {
            observe(NULL);
            return false;
        }
        
        synchronized(_selectionLock, GameLockMap) {
            if (target->compareType(ObjectMaskUnit)) {
                Unit* unit = static_cast<Unit*>(target);
                
                if (controlStateSelection(unit) != MapControlStateEnabled) {
                    return false;
                }
                observe(unit->force());
                
                if (unit->force() == _force) {
                    if (merge) {
                        if (_selection.expandedSelections->count()) {
                            Unit* other = _selection.expandedSelections->objectAtIndex(0);
                            if (unit->compareType(ObjectMaskWeapon)) {
                                if (!other->compareType(ObjectMaskWeapon)) {
                                    merge = false;
                                }
                            }
                            else if (_selection.groupedSelections->count() == 1 && other->index() != unit->index()) {
                                merge = false;
                            }
                        }
                    }
                    
                    if (!merge) {
                        cancelSelectionImpl();
                    }
                    if (all) {
                        CSBoundingFrustum frustum;
                        synchronized(_camera, GameLockMap) {
                            frustum = _camera->capture().boundingFrustum();
                        }
                        synchronized(_phaseLock, GameLockMap) {
                            foreach(Unit*, otherUnit, _units) {
                                if (otherUnit->force() == _force && otherUnit->index() == unit->index() && otherUnit->isLocated()) {
                                    CSBoundingSphere bs(otherUnit->worldPoint(ObjectLayerMiddle), (float)otherUnit->collider() * TileSize);
                                    
                                    if (frustum.intersects(bs) && controlStateSelection(otherUnit) == MapControlStateEnabled) {
                                        addSelectionImpl(otherUnit);
                                        
                                        StrokeEffect::effect(otherUnit, EffectDisplayVanish);
                                    }
                                }
                            }
                        }
                    }
                    else {
                        addSelectionImpl(unit);
                        
                        StrokeEffect::effect(unit, EffectDisplayVanish);
                    }
					voice(VoiceSelect);
                    passAction(ActionSelect, _selection.expandedSelections, NULL, NULL);
                    passAction(ActionClickSelect, _selection.expandedSelections, NULL, NULL);
                    return true;
                }
                CSObject::retain(_selection.target, target);
            }
            else if (controlState(MapControlSelect) == MapControlStateEnabled) {
                CSObject::retain(_selection.target, target);
            }
        }
    }
    return false;
}

void Map::showBuildingLocations(const UnitIndex &index) {
    AssertOnMainThread();
    
    const UnitData& data = Asset::sharedAsset()->unit(index);
    
    CSArray<FPoint>* points;
    
    const Force* vision = this->vision();
    
    if (Object::compareType(data.type, ObjectMaskBase)) {
        points = new CSArray<FPoint>();
        
        foreach(const Area&, area, _areas) {
            bool enabled = true;

            synchronized(_phaseLock, GameLockMap) {
                for (const TileObject* current = tile(area.point).objects(); current; current = current->next()) {
                    if (current->object()->compareType(ObjectMaskBase) && current->object()->isVisible(vision)) {
                        enabled = false;
                        break;
                    }
                }
            }
            if (enabled) {
                points->addObject(area.point);
            }
        }
    }
    else if (Object::compareType(data.type, ObjectMaskRefinery)) {
        points = new CSArray<FPoint>();
        
        synchronized(_phaseLock, GameLockMap) {
            foreach(const Resource*, resource, _resources) {
                if (resource->kind == 1) {
                    bool enabled = true;
					for (const TileObject* current = tile(resource->point()).objects(); current; current = current->next()) {
						if (current->object()->compareType(ObjectMaskRefinery) && current->object()->isVisible(vision)) {
							enabled = false;
							break;
						}
					}
                    if (enabled) {
                        points->addObject(resource->point());
                    }
                }
            }
        }
    }
    else {
        return;
    }
    if (points->count()) {
        if (!_buildingLocations) {
            _buildingLocations = new CSArray<Frame>(points->count());
        }
        else {
            int i = 0;
            while (i < _buildingLocations->count()) {
                Frame* oldframe = _buildingLocations->objectAtIndex(i);
                
                if (oldframe->index != index) {
                    _buildingLocations->removeObjectAtIndex(i);
                }
                else {
                    int index = points->indexOfObject(oldframe->point());
                    if (index >= 0) {
                        points->removeObjectAtIndex(index);
                        i++;
                    }
                    else {
                        _buildingLocations->removeObjectAtIndex(i);
                    }
                }
            }
        }
        foreach(const FPoint&, point, points) {
            Frame* frame = Frame::frame(_force, index, CommandBuildTargetNone, false);
            frame->locate(point);
            _buildingLocations->addObject(frame);
        }
        /*
        CSVector2 origin;
        synchronized(_camera, GameLockMap) {
            origin = (CSVector2)_camera->capture().target();
        }
        CSVector2 target;
        float distance2 = FloatMax;
        foreach(const FPoint&, point, points) {
            Frame* frame = new Frame(_force, index, CommandBuildTargetNone, false);
            frame->locate(point);
            _buildingLocations->addObject(frame);
            frame->release();
            
            CSVector2 wp = (CSVector2)point * TileSize;
            float d2 = origin.distanceSquared(wp);
            if (d2 < distance2) {
                distance2 = d2;
                target = wp;
            }
        }
        _camera->setScroll(target, 0.1f);
         */
    }
    points->release();
}

void Map::hideBuildingLocations() {
    AssertOnMainThread();
    
    CSObject::release(_buildingLocations);
}
bool Map::isTeamPlaying() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.teamPlaying;
}
void Map::setChatBlocked(bool block) {
    if (_mode.type == GameModeTypeOnline) {
        _mode.context.online.chatBlocked = block;
        
        pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconChat), Asset::sharedAsset()->string(MessageSetGame, MessageGameChatBlock_0 + block)));
    }
}
bool Map::isChatBlocked() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.chatBlocked;
}
void Map::setChatToTeam(bool chatToTeam) {
    if (_mode.type == GameModeTypeOnline) {
        _mode.context.online.chatToTeam = _mode.context.online.teamPlaying && chatToTeam;
    }
}
bool Map::isChatToTeam() const {
    return _mode.type == GameModeTypeOnline && _mode.context.online.chatToTeam;
}
void Map::chat(Force* force, const User* user, const CSString* msg, bool toAlliance) {
    if (isChatBlocked() && user->userId != Account::sharedAccount()->userId) {
        return;
    }
    
    Sound::play(SoundControlEffect, SoundFxChatNotification, SoundPlayEffect);
    
    uint color = force ? (uint)force->allianceColor() : 0xffffffff;
    
    if (toAlliance) {
        msg = CSString::stringWithFormat("#color(%08X)%s%s#color()\n%s", color, (const char*)*Asset::sharedAsset()->string(MessageSetGame, MessageGameChatToTeam), (const char*)*user->name, (const char*)*msg);
    }
    else {
        msg = CSString::stringWithFormat("#color(%08X)%s#color()\n%s", color, (const char*)*user->name, (const char*)*msg);
    }
    
    Map::sharedMap()->pushMessage(Message::message(ImageIndex(ImageSetGame, ImageGameMessageIconChat), msg, true));
}

void Map::updateEmoticons(float delta) {
	for (int i = 0; i < MaxPlayers; i++) {
		if (_forces[i]) _forces[i]->updateEmoticon(delta);
	}
}

void Map::emoticon(Force* force, const EmoticonIndex& index) {
    if (isChatBlocked() && force != _force) {
        return;
    }
    if (force->showEmoticon(index)) {
        if (!force->ai()) {
            for (int i = 0; i < MaxPlayers; i++) {
                if (_forces[i] && _forces[i] != force && _forces[i]->ai()) {
                    _forces[i]->ai()->emoticonResponseIndex = index;
                }
            }
        }
    }
}

void Map::emoticon(const EmoticonIndex& index) {
    if (isPlaying()) {
        if (modeType() == GameModeTypeOnline) {
            writeEmoticon(_mode.context.online.localBuffer, index);
        }
        else {
            emoticon(_force, index);
        }
    }
}

void Map::ping(Force* force, int index, const CSVector2& point) {
    if (_force && _force->alliance() != force->alliance()) {
        return;
    }

    const PingData& data = Asset::sharedAsset()->ping(index);
    
    CSString* msg = CSObject::copy(data.message->value());
    
    if (msg) {
        msg->replace("$user", CSString::cstringWithFormat("#color(%08X)%s#color()", (uint)force->allianceColor(), (const char*)*force->player->name));
        
        pushMessage(Message::message(data.icon, msg));
        
        msg->release();
    }
    
    if (event(force, true, data.animation, data.color, point)) {
        warn(data.icon, point, true);
    }
}

void Map::ping(int index, const CSVector2& point) {
    if (isPlaying()) {
        if (modeType() == GameModeTypeOnline) {
            writePing(_mode.context.online.localBuffer, index, point);
        }
        else {
            ping(_force, index, point);
        }
        passAction(ActionPing, NULL, NULL, NULL);
    }
}

void Map::end() {
    AssertOnMainThread();
    
    switch (_state) {
        case MapStateNone:
        case MapStateAuto:
            if (_playing == PlayingParticipate) {
#ifdef UseAlwaysWin
				Sound::stop(SoundControlGameBgm);
				_running = false;
				_force->result = GameResultVictory;
				_state = MapStateVictory;
#else
				if (_mode.type == GameModeTypeOnline) {
					writeControl(_mode.context.online.localBuffer, GameControlNone);
					if (_paused || !Account::sharedAccount()->match()) {
						Sound::stop(SoundControlGameBgm);
						_running = false;
						_force->result = GameResultDefeat;
						_state = MapStateDefeat;
					}
                }
				else {
					_force->setControl(GameControlNone);
					_paused = false;
				}
#endif
            }
            else {
				Sound::stop(SoundControlGameBgm);
				_running = false;
				_state = MapStateInterrupt;
            }
            break;
    }
}

void Map::skip() {
	if (isSingle()) {
		_mode.context.single.skipCommit = true;
	}
}

bool Map::isSkipAllowed() const {
	return isSingle() && _mode.context.single.skipReady;
}

typename Map::MonitorStatus Map::monitorStatus() const {
    MonitorStatus status;
    
    status.updateLatency = _latency.updateLatecy;
    status.timeoutLatency = _latency.timeoutLatency;
    status.objectCount = _objects->count();
    status.unitCount = _units->count();
    //status.secretCount = CSSecret::sharedSecret()->valueCount();
    
    return status;
}


void Map::updateGraph() {
	_graphInterval += _latency.frameDelayFloat;
	if (_graphInterval >= MapGraphInterval) {
		_graphInterval -= MapGraphInterval;

		MapGraph graph = {};

		int playerCount = 0;
		int resourceAmount = 0;
		int armyAmount = 0;
		for (int i = 0; i < MaxPlayers; i++) {
			const Force* force = _forces[i];
			if (force && force->player->userId) {
				int resource = (int)((force->resource(0) + force->resource(1)) + (force->gathering(0) + force->gathering(1)) * MapGraphResourceGatheringWeight);
				graph.rate[i][MapGraphTypeResource] = resource;
				resourceAmount += resource;
				playerCount++;
			}
		}
		foreach(const Unit*, unit, _units) {
			if (unit->compareType(ObjectMaskWeapon)) {
				const Force* force = unit->force();
				if (force && force->player->userId) {
					const UnitData& data = unit->data();
					int army = data.resources[0] + data.resources[1];
					graph.rate[unit->force()->index][MapGraphTypeArmy] += army;
					armyAmount += army;
				}
			}
		}
		for (int i = 0; i < MaxPlayers; i++) {
			if (_forces[i] && _forces[i]->player->userId) {
				float& r = graph.rate[i][MapGraphTypeResource];
				float& a = graph.rate[i][MapGraphTypeArmy];

				if (resourceAmount) r = r * playerCount / resourceAmount - 1;
				if (armyAmount) a = a * playerCount / armyAmount - 1;
			}
		}
		_graph->addObject(graph);
	}
}