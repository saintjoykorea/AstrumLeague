//
//  Unit.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define GameImpl

#include "Unit.h"

#include "Map.h"
#include "Frame.h"
#include "Destination.h"

#include "GameLayerIndex.h"

#include "MessageLayer.h"

#include "UnitAbilityLayer.h"

#define SpeechDuration                  3.5f
#define SpeechTransitionDuration        0.25f
#define DisplayingDuration              0.5f
#define DamagingDisplayDuration         0.25f
#define HealthChangeDisplayDuration     3.0f

static const fixed DamagingDuration(5);
static const fixed AggroDuration(5);
static const fixed AggroChangeDistance(fixed::Raw, 32768);

Unit::Unit(Force* force, const UnitIndex& index, int level, int skin, Unit* parent) :
	UnitBaseGame(force, index, level, skin),
	key(Map::sharedMap()->registerKey()),
	_originForce(force),
	_originIndex(index),
	_scaleBase(fixed::One)
{
    AssertOnUpdateThread();
    
    SyncLog("Sync (%d) (Create):%d, %d, %d", Map::sharedMap()->frame(), key, (uint)_index, level);
    
    _data = &Asset::sharedAsset()->unit(index);
	_type = _data->type;
	_collider = _data->collider;
	_radius = _data->radius;

    _parent = _data->subordinated ? retain(parent) : NULL;
    if (_parent) {
        synchronized(_parent) {
            if (!_parent->_children) {
                _parent->_children = new CSArray<Unit>();
            }
            _parent->_children->addObject(this);
        }
        this->skin = _parent->skin;
        this->level = _parent->level;
    }
    else {
        if (!level) this->level = force ? force->unitLevel(index) : 1;
        if (!skin && force) this->skin = force->unitSkin(index);
    }
	_originLevel = level;

    fixed health = ability(AbilityHealth);
    fixed shield = ability(AbilityShield);
    fixed maxMana = ability(AbilityMana);
    _health = _maxHealth = health;
    _shield = _maxShield = shield;
    _alive = health || shield ? AliveYes : AliveNone;
    _maxMana = maxMana;
    _mana = CSMath::min(ability(AbilityManaInitial), maxMana);
    _sight = ability(AbilitySight);
	_scale = ability(AbilityScale, fixed::One);
	_collider *= _scale;
	_radius *= (float)_scale;

    if (_force && _data->population) {
        _force->revisePopulation(_data->population);
    }
	_motion.index = AnimationIndex::None;
	for (int i = 0; i < MotionCount; i++) {
        _motions[i] = i;
    }
	if (_data->minimap) {
		_minimap = new Animation(_data->minimap, SoundControlEffect, false);
		_minimap->setSpriteDelegate0(this, &Unit::spriteElement);

		Map::sharedMap()->registerMinimapUnit(this);
	}
    CSAssert(_data->actionTriggers, "invalid data");
    _action = new Action();
    _action->trigger = _data->actionTriggers->objectAtIndex(0);
    _action->ip = 0;
    _action->loop = 0;
    _action->prev = NULL;

	_command = CommandIndex::None;

	_nextActions = new CSArray<NextAction>(1);

	if (compareType(ObjectMaskUnit)) {
        Map::sharedMap()->autoSelection(this);
        
        Map::sharedMap()->registerUnit(this);
    }
    Map::sharedMap()->applyUnitSetting(this);
    
    Map::sharedMap()->registerObject(this);
}

Unit::~Unit() {
    if (_producing) {
        release(_producing->rallyPoint);
        delete _producing;
    }
    if (_training) {
        delete _training;
    }
    if (_transporting) {
        _transporting->crews->release();
        _transporting->follows->release();
        delete _transporting;
    }
    if (_gathering) {
        release(_gathering->resource);
        release(_gathering->base);
        delete _gathering;
    }
    releaseWatchings();
	release(_watchingExclusions);
    
    clearBuffs(false);
    
    if (_escaping) {
        delete _escaping;
    }
    if (_jumping) {
        delete _jumping;
    }
    if (_originPoint) {
        delete _originPoint;
    }
    release(_parent);
    release(_children);
    if (_rangeDisplayValue) {
        delete _rangeDisplayValue;
    }
    if (_speech) {
        _speech->msg->release();
        delete _speech;
    }
    release(_motion.animation);
	release(_minimap);
    release(_path);

    clearEffects();
    
    while (_action) {
        Action* prev = _action->prev;
        delete _action;
        _action = prev;
    }
    release(_target);
    release(_originTarget);
	_nextActions->release();
    release(_aggro);
    release(_revisionTimes);
	release(_damageOrigins);
    release(_growups);
    release(_name);
    release(_abilities);
}

Unit* Unit::unit(Force* force, const UnitIndex& index, int level, int skin, Unit* parent) {
    Unit* unit = new Unit(force, index, level, skin, parent);
    unit->release();        //no autorelease, already called registerObject
    return unit;
}

void Unit::deactivate() {
    stopBuilding(StopBuildingDestroy);
    
    Map::sharedMap()->removeSquad(this);
    Map::sharedMap()->removeSelection(this);
    Map::sharedMap()->removeNumbering(this);
    if (_navigating) {
        Map::sharedMap()->resetNavigation(this, NavigatingNone);
        _navigating = NavigatingNone;
    }
    
    if (_producing) {
        if (_force && _producing->count) {
            for (int i = 0; i < _producing->count; i++) {
                const UnitIndex& index = _producing->indices[i];
                const UnitData& data = Asset::sharedAsset()->unit(index);
                
                _force->reviseResource(0, (fixed)data.resources[0]);
                _force->reviseResource(1, (fixed)data.resources[1]);
            }
            if (_producing->progress) {
                const UnitData& data = Asset::sharedAsset()->unit(_producing->indices[0]);
                _force->revisePopulation(-data.population * (data.addition + 1));
            }
        }
        synchronized(this, GameLockObject) {
            release(_producing->rallyPoint);
            delete _producing;
            _producing = NULL;
        }
    }
    if (_training) {
        if (_force) {
            for (int i = 0; i < _training->count; i++) {
                const RuneIndex& index = _training->indices[i];
                
                const RuneData& data = Asset::sharedAsset()->rune(index);
                _force->reviseResource(0, (fixed)data.resources[0]);
                _force->reviseResource(1, (fixed)data.resources[1]);
                _force->cancelTraining(index);
            }
        }
        synchronized(this, GameLockObject) {
            delete _training;
            _training = NULL;
        }
    }
    if (_transporting) {
        foreach(Unit*, crew, _transporting->crews) {
            crew->destroy();
        }
        synchronized(this, GameLockObject) {
            _transporting->crews->release();
            _transporting->follows->release();
            delete _transporting;
            _transporting = NULL;
        }
    }
    if (_gathering) {
        synchronized(this, GameLockObject) {
            if (_gathering->resource) {
                if (_gathering->occupied) {
                    _gathering->resource->occupy(false);
                }
                _gathering->resource->endGathering();
                _gathering->resource->release();
            }
            release(_gathering->base);
            delete _gathering;
            _gathering = NULL;
        }
    }
    releaseWatchings();
	release(_watchingExclusions);
	release(_damageOrigins);

	_nextActions->removeAllObjects();
}

void Unit::dispose() {
    Object::dispose();
    
    deactivate();
    
	Map::sharedMap()->unregisterMinimapUnit(this);

    Map::sharedMap()->unregisterUnit(this);
    
    clearBuffs(true);

    if (_escaping) {
        delete _escaping;
        _escaping = NULL;
    }
    if (_jumping) {
        delete _jumping;
        _jumping = NULL;
    }
    if (!_forceDisposed) {
        if (_force) {
            if (_data->population) {
                _force->revisePopulation(-_data->population);
            }
            if (_supply) {
                _force->reviseSupply(-_supply);
            }
        }
        _forceDisposed = true;
    }
    if (_originPoint) {
        delete _originPoint;
        _originPoint = NULL;
    }
    if (_parent) {
        synchronized(_parent, GameLockObject) {
            if (_parent->_children) {
                _parent->_children->removeObjectIdenticalTo(this);
                if (!_parent->_children->count()) {
                    _parent->_children->release();
                    _parent->_children = NULL;
                }
            }
        }
    }
    synchronized(this, GameLockObject) {
        //release(_parent);
        release(_children);
        
        if (_speech) {
            _speech->msg->release();
            delete _speech;
            _speech = NULL;
        }
    }
    if (_rangeDisplayValue) {
        delete _rangeDisplayValue;
        _rangeDisplayValue = NULL;
    }
    release(_path);
    _movement.dir = 0;
    
    clearEffects();
    
    while (_action) {
        Action* prev = _action->prev;
        delete _action;
        _action = prev;
    }
    release(_target);
    release(_originTarget);
	_nextActions->removeAllObjects();
	release(_aggro);
    release(_revisionTimes);
	release(_damageOrigins);
    release(_growups);
}

void Unit::destroy() {
	if (_state != UnitStateDestroy) {
		if (_alive == AliveYes) _alive = AliveNo;
		_health = fixed::Zero;
		_shield = fixed::Zero;

		if (_located) {
			Map::sharedMap()->voice(VoiceDestroy, this);
		}

		_state = UnitStateDestroy;

		deactivate();
		changeAction();
	}
}

bool Unit::setType(ObjectType type) {
    if (_type != type) {
        bool resetVision = false;
        
        if (_located) {
            if (!Map::sharedMap()->relocate(this, true, type, _collider, _collision)) {
                return false;
            }
            resetVision = compareType(ObjectMaskAir) != compareType(type, ObjectMaskAir) || compareType(ObjectMaskUnit) != compareType(type, ObjectMaskUnit);
            
            if (resetVision) {
                changeVision(false);
            }
        }
        
        release(_path);
        _movement.dir = 0;
        _pathDelay = 0;
        
        _type = type;
        
        if (resetVision) {
            _sight = ability(AbilitySight);
            _visionPoint = _point;
            changeVision(true);
        }
        if (compareType(ObjectMaskUnit)) {
            Map::sharedMap()->autoSelection(this);
            
            if (_navigating) {
                Map::sharedMap()->resetNavigation(this, _navigating);
            }
            Map::sharedMap()->registerUnit(this);
        }
        else {
            Map::sharedMap()->removeSelection(this);
            Map::sharedMap()->removeNumbering(this);
            if (_navigating) {
                Map::sharedMap()->resetNavigation(this, NavigatingNone);
            }
            Map::sharedMap()->unregisterUnit(this);
        }
        
        Map::sharedMap()->applyUnitSetting(this);
    }
    return true;
}
void Unit::setCollision(bool collision) {
    if (_collision != collision) {
        Map::sharedMap()->relocate(this, false, _type, _collider, collision);
        _collision = collision;
    }
}

CSVector3 Unit::worldPoint(ObjectLayer layer) const {
	CSVector3 wp = Map::sharedMap()->convertMapToWorldSpace(_point);
	switch (layer) {
		case ObjectLayerMiddle:
			wp.z += _z + _data->z;
			wp += Map::sharedMap()->camera()->up() * _data->body * (float)_scale;
			break;
		case ObjectLayerTop:
			wp.z += _z + _data->z;
			wp += Map::sharedMap()->camera()->up() * _data->top * (float)_scale;
			break;
	}
	return wp;
}

const CSColor& Unit::color(bool visible, bool detecting) const {
    const Force* vision = Map::sharedMap()->vision();
    
    const Force* force = _force;
    
    return (force && (_data->visible || (visible && (!isCloaking() || detecting || !vision || vision->alliance() == force->alliance())))) ? force->allianceColor() : CSColor::Transparent;
}

void Unit::setForce(Force* force, int level) {
    if (force != _force) {
        Map::sharedMap()->removeSelection(this);
        Map::sharedMap()->removeNumbering(this);
        if (_navigating) {
            Map::sharedMap()->resetNavigation(this, NavigatingNone);
        }
        
        if (_force) {
            if (_data->population) {
                _force->revisePopulation(-_data->population);
            }
            if (_supply) {
                _force->reviseSupply(-_supply);
            }
        }
        if (_producing) {
            if (_force && _producing->count) {
                for (int i = 0; i < _producing->count; i++) {
                    const UnitIndex& index = _producing->indices[i];
                    const UnitData& data = Asset::sharedAsset()->unit(index);
                    
                    _force->reviseResource(0, (fixed)data.resources[0]);
                    _force->reviseResource(1, (fixed)data.resources[1]);
                }
                if (_producing->progress) {
                    const UnitData& data = Asset::sharedAsset()->unit(_producing->indices[0]);
                    _force->revisePopulation(-data.population);
                }
            }
            synchronized(this, GameLockObject) {
                release(_producing->rallyPoint);
                delete _producing;
                _producing = NULL;
            }
        }
        if (_training) {
            if (_force) {
                for (int i = 0; i < _training->count; i++) {
                    const RuneIndex& index = _training->indices[i];
                    
                    const RuneData& data = Asset::sharedAsset()->rune(index);
                    _force->reviseResource(0, (fixed)data.resources[0]);
                    _force->reviseResource(1, (fixed)data.resources[1]);
                    _force->cancelTraining(index);
                }
            }
            synchronized(this, GameLockObject) {
                delete _training;
                _training = NULL;
            }
        }
        if (_located) {
            Map::sharedMap()->relocate(this, true, _type, _collider, _collision);
            
            changeVision(false);
        }
		_nextActions->removeAllObjects();
        changeAction();
        
        _force = force;

		this->level = level;
        
        if (_located) {
            _sight = ability(AbilitySight);
            _visionPoint = _point;
            changeVision(true);
        }
        
        if (_navigating) {
            Map::sharedMap()->resetNavigation(this, _navigating);
        }
        if (_force) {
            if (_data->population) {
                _force->revisePopulation(_data->population);
            }
            if (_supply) {
                _force->reviseSupply(_supply);
            }
        }
        if (_transporting) {
            foreach(Unit*, crew, _transporting->crews) {
                crew->setForce(_force, level);
            }
        }
        foreach(Unit*, child, _children) {
            child->setForce(_force, level);
        }
        if (_parent) {
            synchronized(_parent, GameLockObject) {
                if (_parent->_children) {
                    if (_parent->force() == _force) {
                        if (!_parent->_children->containsObjectIdenticalTo(this)) {
                            _parent->_children->addObject(this);
                        }
                    }
                    else {
                        _parent->_children->removeObjectIdenticalTo(this);
                    }
                }
            }
        }

        _dirtyui = true;
        
        if (compareType(ObjectMaskUnit)) {
            Map::sharedMap()->autoSelection(this);
        }
        
        Map::sharedMap()->applyUnitSetting(this);
    }
}

bool Unit::setIndex(const UnitIndex& index, int state) {
    if (index != _index) {
        if (!stopBuilding(StopBuildingTarget)) {
            return false;
        }
        if (_navigating) {
            Map::sharedMap()->resetNavigation(this, NavigatingNone);
        }
        
        bool selected = Map::sharedMap()->removeSelection(this);
        
        const UnitData& data = Asset::sharedAsset()->unit(index);
        
        if (_located) {
            if (!Map::sharedMap()->relocate(this, false, data.type, data.collider * _scale, _collision)) {
                return false;
            }
            changeVision(false);
        }
        if (_force) {
            _force->revisePopulation(data.population - _data->population);
        }

        _index = index;
        _data = &Asset::sharedAsset()->unit(index);
        _type = data.type;
		_collider = data.collider * _scale;
		_radius = data.radius * (float)_scale;
        _state = state;
        fixed nextMaxHealth = ability(AbilityHealth);
        fixed nextMaxShield = ability(AbilityShield);
        if (_maxHealth != nextMaxHealth) {
            _health = _maxHealth ? (_health * nextMaxHealth / _maxHealth) : nextMaxHealth;
            _maxHealth = nextMaxHealth;
        }
        if (_maxShield != nextMaxShield) {
            _shield = _maxShield ? (_shield * nextMaxShield / _maxShield) : nextMaxShield;
			_maxShield = nextMaxShield;
        }
        if (!_maxHealth && !_maxShield) {
            _alive = AliveNone;
            _healthDisplay = 0.0f;
        }
        else if (_alive == AliveNone) {
            _alive = AliveYes;
        }
        
        _maxMana = ability(AbilityMana);
        _mana = CSMath::min(ability(AbilityManaInitial), _maxMana);
        
        for (int i = 0; i < MotionCount; i++) {
            _motions[i] = i;
        }
        
        _distanceDisplay = 0.0f;
        _rangeDisplay = 0.0f;
        if (_rangeDisplayValue) {
            delete _rangeDisplayValue;
            _rangeDisplayValue = NULL;
        }
        
        synchronized(this, GameLockObject) {
			_motion.index = AnimationIndex::None;
			release(_minimap);
			if (_data->minimap) {
				_minimap = new Animation(_data->minimap, SoundControlEffect, false);
				_minimap->setSpriteDelegate0(this, &Unit::spriteElement);
			}
        }

		if (_minimap) {
			Map::sharedMap()->registerMinimapUnit(this);
		}
		else {
			Map::sharedMap()->unregisterMinimapUnit(this);
		}
        
		_nextActions->removeAllObjects();
        changeAction();
        
        if (_located) {
            _sight = ability(AbilitySight);
            _visionPoint = _point;
            changeVision(true);
        }

        if (compareType(ObjectMaskUnit)) {
            Map::sharedMap()->resetNumbering(this);
            
            if (_navigating) {
                Map::sharedMap()->resetNavigation(this, _navigating);
            }
            if (selected) {
                Map::sharedMap()->addSelection(this);
            }
            else {
                Map::sharedMap()->autoSelection(this);
            }
            Map::sharedMap()->registerUnit(this);
        }
        else{
            Map::sharedMap()->removeNumbering(this);
            
            Map::sharedMap()->unregisterUnit(this);
        }
        
        Map::sharedMap()->applyUnitSetting(this);
        
        _dirtyui = true;
    }
    return true;
}

bool Unit::isCloaking() const {
    if (_cloaking) {
        return true;
    }
    if (_buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            for (Buff* current = _buffs; current; current = current->next) {
                if (current->cloaking) {
                    return true;
                }
            }
        }
    }
    return false;
}
bool Unit::isDetecting() const {
    if (_detecting) {
        return true;
    }
    if (_buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            for (Buff* current = _buffs; current; current = current->next) {
                if (current->detecting) {
                    return true;
                }
            }
        }
    }
    return false;
}
void Unit::setDetecting(bool detecting) {
    if (detecting != _detecting) {
        if (_located) {
            changeVision(false);
        }
        _detecting = detecting;
        if (_located) {
            _visionPoint = _point;
            changeVision(true);
        }
    }
}
void Unit::setDetecting(bool detecting, Buff* buff) {
    if (detecting != buff->detecting) {
        if (_located) {
            changeVision(false);
        }
        buff->detecting = detecting;
        if (_located) {
            _visionPoint = _point;
            changeVision(true);
        }
    }
}

bool Unit::isReachable(const FPoint& point) const {
	FPoint p = _point;

	return Map::sharedMap()->hasPath(p, point);
}

int Unit::childrenCount() const {
    if (_children) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_children) {
                return _children->count();
            }
        }
    }
    return 0;
}

void Unit::locate(const FPoint& point) {
    if (!_located) {
        _visionPoint = point;
        changeVision(true);
        
        Map::sharedMap()->autoSelection(this);
    }
    Object::locate(point);
    
	_motion.relocated = true;
}
void Unit::unlocate() {
    if (_located) {
        Map::sharedMap()->removeSelection(this);
        
        changeVision(false);
    }
    Object::unlocate();
}
bool Unit::isDetected(int alliance) const {
    const Force* force = _force;
    
    return !isCloaking() || !force || force->alliance() == alliance || Map::sharedMap()->isDetected(bounds(), alliance);
}
bool Unit::isDetected(const Force* otherForce) const {
    const Force* force = _force;

    return !otherForce || !force || !isCloaking() || force->alliance() == otherForce->alliance() || Map::sharedMap()->isDetected(bounds(), otherForce->alliance());
}
bool Unit::isVisible(int alliance) const {
    return _located && (_data->visible || Map::sharedMap()->isVisible(bounds(), alliance)) && isDetected(alliance);
}
bool Unit::isVisible(const Force* otherForce) const {
    return _located && (!otherForce || _data->visible || (Map::sharedMap()->isVisible(bounds(), otherForce->alliance()) && isDetected(otherForce->alliance())));
}
bool Unit::isHiding() const {
	if (!_force) return false;

	for (int a = 0; a < MaxPlayers; a++) {
		if (a != _force->alliance()) {
			bool flag = false;
			for (int i = 0; i < MaxPlayers; i++) {
				const Force* force = Map::sharedMap()->force(i);
				if (force && force->alliance() == a) {
					flag = true;
					break;
				}
			}
			if (flag && isVisible(a)) {
				return false;
			}
		}
	}
	return true;
}

static void smoothAlpha(float& src, float dest) {
	if (src < dest) {
		src = CSMath::min(src + Map::sharedMap()->frameDelayFloat() / DisplayingDuration, dest);
	}
	else if (src > dest) {
		src = CSMath::max(src - Map::sharedMap()->frameDelayFloat() / DisplayingDuration, dest);
	}
}

ObjectDisplay Unit::display() const {
	int layer;
	if (compareType(ObjectMaskAir)) layer = 2;
	else if (compareType(ObjectMaskGround)) layer = 1;
	else layer = 0;

	if (!_located) {
		_alpha = 0;
		return ObjectDisplay(ObjectVisibleNone, layer);
	}

	bool cloaking = isCloaking();

	static const uint ObjectMaskBush = ObjectMaskUnit & ~ObjectMaskAir;

	bool faint = cloaking || (compareType(ObjectMaskBush) && Map::sharedMap()->tile(_point).attribute() == TileAttributeBush);

	float alpha = faint ? 0.75f : 1;

	if (_data->visible) {
		smoothAlpha(_alpha, alpha);
        return ObjectDisplay(ObjectVisibleNormal, layer);
    }
    const Force* vision = Map::sharedMap()->vision();
        
    const Force* force = _force;

    if (!vision || (force && vision->alliance() == force->alliance())) {
		smoothAlpha(_alpha, alpha);
    }
    else if (Map::sharedMap()->isVisible(bounds(), vision->alliance())) {
		if (cloaking && !Map::sharedMap()->isDetected(bounds(), vision->alliance())) {
			smoothAlpha(_alpha, 0);
			return ObjectDisplay(_alpha ? ObjectVisibleNormal : ObjectVisibleDistortion, layer);
        }
        else smoothAlpha(_alpha, alpha);
    }
	else if (compareType(ObjectMaskBuilding)) _alpha = 0;
	else smoothAlpha(_alpha, 0);

	return ObjectDisplay(_alpha ? ObjectVisibleNormal : ObjectVisibleNone, layer);
}

void Unit::changeVision(const FPoint& point, fixed sight, bool visible, bool lock) {
    if (_force) {
        Map::sharedMap()->changeVision(this, point, sight, isDetecting(), _force->alliance(), visible, lock);
    }
    for (Buff* current = _buffs; current; current = current->next) {
        if (current->vision && current->originForce) {
            Map::sharedMap()->changeVision(this, point, _sight, false, current->originForce->alliance(), visible, lock);
        }
    }
}

int Unit::producingCount(const UnitIndex& index) const {
    int count = 0;
    if (_producing) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_producing) {
                for (int i = 0; i < _producing->count; i++) {
                    if (_producing->indices[i] == index) {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}
bool Unit::isProducing() const {
	bool rtn = false;
	if (_producing) {
		synchronized_to_update_thread(this, GameLockObject) {
			rtn = _producing && _producing->count;
		}
	}
	return rtn;
}
bool Unit::isTransporting() const {
	bool rtn = false;
	if (_transporting) {
		synchronized_to_update_thread(this, GameLockObject) {
			rtn = _transporting && _transporting->crews->count();
		}
	}
	return rtn;
}
bool Unit::isGathering() const {
	bool rtn = false;
	if (_gathering) {
		synchronized_to_update_thread(this, GameLockObject) {
			rtn = _gathering && _gathering->resource;
		}
	}
	return rtn;
}
bool Unit::isGathering(int kind) const {
	if (_gathering) {
		synchronized_to_update_thread(this, GameLockObject) {
			if (_gathering && _gathering->resource) {
				return _gathering->resource->kind == kind;
			}
		}
	}
	return false;
}

fixed Unit::abilityElement(int index, fixed value, const UnitBase *target) const {
    assertOnUnlocked();
    
    value = UnitBase::abilityElement(index, value, target);

    struct BuffAbilityEntry {
        BuffIndex index;
        const UnitBaseGame* origin;
    };
    CSArray<BuffAbilityEntry>* buffs = NULL;

    if (_abilities || _buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_abilities) {
                foreach(const AbilityRevision&, abtr, _abilities) {
                    if (abtr.index == index && (!abtr.condition || (target && target->match(this, abtr.condition)))) {
                        if (abtr.value >= fixed::Zero) value = abtr.value;
                        value += abtr.diff;
                        break;
                    }
                }
            }
            for (const Buff* current = _buffs; current; current = current->next) {
                if (!buffs) buffs = new CSArray<BuffAbilityEntry>();
                BuffAbilityEntry& buff = buffs->addObject();
                buff.index = current->index;
                buff.origin = current->origin;
				if (buff.origin) {
					if (buff.origin->asUnit()) buff.origin->asUnit()->retain();
					else if (buff.origin->asSpawn()) buff.origin->asSpawn()->retain();
				}
            }
        }
    }
    if (buffs) {
        foreach(BuffAbilityEntry&, buff, buffs) {
            value = Asset::sharedAsset()->buff(buff.index).ability.value(index, buff.origin, NULL, value, this);
            
			if (buff.origin) {
				if (buff.origin->asUnit()) buff.origin->asUnit()->release();
                else if (buff.origin->asSpawn()) buff.origin->asSpawn()->release();
			}
        }
        buffs->release();
    }

    return value;
}
const CSArray<BuffIndex>* Unit::buffs() const {
    if (_buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            if (_buffs) {
                CSArray<BuffIndex>* buffs = CSArray<BuffIndex>::array();
                for (Buff* current = _buffs; current; current = current->next) {
                    buffs->addObject(current->index);
                }
                return buffs;
            }
        }
    }
    return NULL;
}

const CSString* Unit::name() const {
    if (_name) {
        synchronized(this, GameLockObject) {
            if (_name) {
                _name->retain();
                _name->autorelease();
                return _name;
            }
        }
    }
    return _data->name->value();
}

void Unit::rename(const CSString* name) {
    synchronized(this, GameLockObject) {
        retain(_name, name);
    }
}

void Unit::obtainExp(int exp) {
    exp += _exp;
    foreach (const Growup*, growup, _growups) {
        bool levelup = false;
        while (level < growup->maxLevel && exp >= growup->levelupExp) {
            exp -= growup->levelupExp;
            level++;
            levelup = true;
        }
        if (levelup && growup->levelupEffect) {
            AnimationEffect::effect(*growup->levelupEffect, this, NULL, false, true, false);
        }
    }
    _exp = exp;

	updateScale();
}

void Unit::addGrowup(const Growup* growup) {
    if (!_growups) {
        _growups = new CSArray<const Growup>();
    }
    else if (_growups->containsObjectIdenticalTo(growup)) {
        return;
    }
    _growups->addObject(growup);
}

void Unit::clearGrowups() {
    if (_growups) {
        _growups->removeAllObjects();
    }
}

fixed Unit::revise(int op, fixed value, fixed time, UnitBaseGame* origin) {
    if (value <= fixed::Zero) {
        return fixed::Zero;
    }

    Force* force = NULL;
	Unit* unit = NULL;
	if (origin) {
		force = origin->force();
		unit = origin->asUnit();
	}
    if (unit && !unit->compareType(ObjectMaskUnit)) {
        unit = unit->_parent && unit->_parent->compareType(ObjectMaskUnit) ? unit->_parent : NULL;
    }
    if (unit) {
        foreach(const RevisionTime&, revising, _revisionTimes) {
            if (revising.key == unit->key) {
                return fixed::Zero;
            }
        }
        if (time) {
            if (!_revisionTimes) {
                _revisionTimes = new CSArray<RevisionTime>(1);
            }
            RevisionTime& revising = _revisionTimes->addObject();
            revising.key = unit->key;
            revising.time = time;
        }
    }

    UnitLog(this, "revise:%f, %d, %d", Map::sharedMap()->playTime(), op, (int)value);

    switch (op) {
        case 0:
            if (_alive == AliveYes) {
                value = recover(value);
            }
            break;
        case 1:
            if (_alive == AliveYes) {
#ifdef UseCheatInvincible
				if (_health + _shield <= fixed::One) return fixed::Zero;
                if (value > fixed::One) value = fixed::One;
#endif
                if (_shield < value) {
                    fixed v = _shield;
                    _shield = fixed::Zero;
                    if (_health <= value - v) {
                        value = v + _health;
                        _health = fixed::Zero;
                        _alive = AliveNo;
                    }
                    else {
                        _health -= value - v;
                    }
                }
                else {
                    _shield -= value;
                }
                if (_force && force && _force->alliance() != force->alliance()) {
					if (Map::sharedMap()->event(_force, true, AnimationIndex(AnimationSetGame, AnimationGameMinimapEvent), Asset::enemyColor, _point)) {
						if (Map::sharedMap()->warn(ImageIndex(ImageSetGame, ImageGameWarning), _point, false)) {
							static const uint ObjectMaskBaseArea = ObjectMaskBuilding ^ ObjectMaskTower;

							if (compareType(ObjectMaskBaseArea)) {
								Map::sharedMap()->announce(_force, AnnouncePlayerBaseAttacked);
								Map::sharedMap()->announce(_force, true, AnnounceAllianceBaseAttacked);
							}
							else {
								Map::sharedMap()->announce(_force, AnnouncePlayerAttacked);
								Map::sharedMap()->announce(_force, true, AnnounceAllianceAttacked);
							}
						}
						if (unit && !unit->isDetected(_force->alliance())) {
							Map::sharedMap()->pushMessage(_force, Message::message(skinData()->smallIcon, Asset::sharedAsset()->string(MessageSetGame, MessageGameUndetectedAttack)));
						}
                    }
                    if (_alive == AliveNo) {
                        if (compareType(ObjectMaskBase)) {
                            const CSString* msg = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetGame, MessageGameDestroy),
                                                                             (uint)_force->allianceColor(),
                                                                             (const char*)*_force->player->name,
                                                                             (const char*)*name());
                            
                            Map::sharedMap()->pushMessage(Message::message(smallIcon(), msg));
                        }
                        
                        if (unit) {
                            unit->_kill++;
                        }
                        
                        foreach (const Growup*, growup, _growups) {
                            if (unit) {
								int exp = growup->dropExpBase + growup->dropExpUp * (level - 1);
								switch (growup->dropExpOp) {
									case DropExpOpPopulation:
										exp *= _data->population;
										break;
								}
								if (exp) unit->obtainExp(exp);
                            }
                            force->reviseResource(0, (fixed)growup->rewardResources[0]);
                            force->reviseResource(1, (fixed)growup->rewardResources[1]);

                            if (growup->dropUnits) {
                                foreach(const DropUnit&, du, growup->dropUnits) {
                                    fixed r = Map::sharedMap()->random()->nextFixed(fixed::Epsilon, fixed::One);
                                    
                                    if (r <= du.rate) {
                                        const UnitData& data = Asset::sharedAsset()->unit(du.index);
                                        FPoint p = _point;
                                        if (Map::sharedMap()->locatablePosition(this, data.type, p, data.collider, NULL, MapLocationCollided) != MapLocationUnabled) {
                                            Unit* dropUnit = Unit::unit(force, du.index, du.level, du.skin, NULL);
											dropUnit->locate(p);

											Map::sharedMap()->voice(VoiceSpawn, dropUnit);
										}
                                        break;
                                    }
                                }
                            }
                            if (growup->dropEffect) {
                                Destination* dest = Destination::destination(_force);
                                dest->locate(_point);
                                AnimationEffect::effect(*growup->dropEffect, dest, NULL, false, false, false);
                            }
                        }
                    }
                    else {
						_damaging = DamagingDuration;

						_damage += value;
						_damageCount++;
                        
                        for (Buff* current = _buffs; current; current = current->next) {
                            current->damage += value;
							current->damageCount++;

							if (unit && current->damageOrigins && !current->damageOrigins->containsObjectIdenticalTo(unit->key)) {
								current->damageOrigins->addObject(unit->key);
							}
                        }
                        if (unit) {
                            if (_aggro != unit) {
                                if (_aggro) {
                                    fixed d0 = _point.distance(unit->_point) - unit->_collider;
                                    fixed d1 = _point.distance(_aggro->_point) - _aggro->_collider;
                                    
                                    if (d0 + AggroChangeDistance >= d1) {
                                        break;
                                    }
                                    _aggro->release();
                                }
                                unit->retain();
                                _aggro = unit;
                            }
                            _aggroDuration = AggroDuration;

							if (_damageOrigins && !_damageOrigins->containsObjectIdenticalTo(unit->key)) {
								_damageOrigins->addObject(unit->key);
							}
                        }
                    }
                }
            }
            break;
        case 2:
            if (_alive == AliveYes) {
                if (_shield + value > _maxShield) {
                    value = _maxShield - _shield;
                    _shield = _maxShield;
                }
                else {
                    _shield += value;
                }
            }
            break;
        case 3:
            if (_alive == AliveYes) {
                if (_shield < value) {
                    fixed v = _shield;
                    _shield = fixed::Zero;
                }
                else {
                    _shield -= value;
                }
            }
            break;
        case 4:
            if (_alive == AliveYes) {
                if (_mana + value > _maxMana) {
                    value = _maxMana - _mana;
                    _mana = _maxMana;
                }
                else {
                    _mana += value;
                }
            }
            break;
        case 5:
            if (_alive == AliveYes) {
#ifdef UseCheatInvincible
                if (value > fixed::One) value = fixed::One;
#endif
                if (_mana >= value) {
                    _mana -= value;
                }
                else {
                    value = _mana;
                    _mana = fixed::Zero;
                }
            }
            break;
        case 6:
            if (_force) {
                _force->reviseResource(0, value);
            }
            else {
                value = fixed::Zero;
            }
            break;
        case 7:
            if (_force) {
                if (value > _force->resource(0)) {
                    value = _force->resource(0);
                }
                _force->reviseResource(0, -value);
            }
            else {
                value = fixed::Zero;
            }
            break;
        case 8:
            if (_force) {
                _force->reviseResource(1, value);
            }
            else {
                value = fixed::Zero;
            }
            break;
        case 9:
            if (_force) {
                if (value > _force->resource(1)) {
                    value = _force->resource(1);
                }
                _force->reviseResource(1, -value);
            }
            else {
                value = fixed::Zero;
            }
            break;
    }
    return value;
}
fixed Unit::recover(fixed value) {
    if (_health + value > _maxHealth) {
        fixed v = _maxHealth - _health;
        _health = _maxHealth;
        
        if (_shield + value - v > _maxShield) {
            value = v + _maxShield - _shield;
            _shield = _maxShield;
        }
        else {
            _shield = _shield + value - v;
        }
    }
    else {
        _health = _health + value;
    }
    _alive = AliveYes;

    return value;
}
void Unit::resurrect(fixed value) {
    if (_alive == AliveNo) {
        if (!value) value = _maxHealth;

        if (_located) {
            _resurrection = value;
            _alive = AliveResurrect;
        }
        else {
            recover(value);
            
			_nextActions->removeAllObjects();
            _state = UnitStateJoin;
            changeAction();
            
            if (_forceDisposed) {
                if (_force) {
                    if (_data->population) {
                        _force->revisePopulation(_data->population);
                    }
                    if (_supply) {
                        _force->reviseSupply(_supply);
                    }
                }
                _forceDisposed = false;
            }
            if (_parent) {
                synchronized(_parent) {
                    if (!_parent->_children) {
                        _parent->_children = new CSArray<Unit>();
                    }
                    if (!_parent->_children->containsObjectIdenticalTo(this)) {
                        _parent->_children->addObject(this);
                    }
                }
            }
            if (compareType(ObjectMaskUnit)) {
                Map::sharedMap()->autoSelection(this);
                
                Map::sharedMap()->registerUnit(this);
            }
            Map::sharedMap()->registerObject(this);
            
            locate(_point);
        }
    }
}
fixed Unit::attack(Unit* target, fixed rate, fixed time) {
    return target->revise(1, ability(AbilityAttack, fixed::Zero, target) * rate, time, this);
}
void Unit::freeze(fixed time) {
    if (time > _freezing) _freezing = time;
}
void Unit::escape(const FPoint& point, fixed time) {
    if (ability(AbilityMoveSpeed)) {
        stopBuilding(StopBuildingTarget);
        
        release(_path);
        _movement.dir = 0;
        _pathDelay = 0;
        
        if (!_escaping) {
            _escaping = new Escaping();
            _escaping->commit = false;
        }
        _escaping->point = point;
        _escaping->waiting = time;
    }
}
void Unit::jump(const FPoint& point, fixed distance, float altitude, fixed time, bool pushing) {
    if (_jumping) {
        return;
    }
    distance += _collider;
    
    fixed d = _point.distance(point);
    
    if (pushing) distance += d;
    else if (d < distance) return;
    
    FPoint p;
    if (_point != point) {
        p = _point - point;
        p.normalize();
    }
    else {
        p.x = Map::sharedMap()->random()->nextFixed(-fixed::One, fixed::One);
        p.y = CSMath::sqrt(fixed::One - p.x * p.x);
    }
    p *= distance;
    p += point;

    if (Map::sharedMap()->locatablePosition(this, p, NULL, MapLocationEnabled) != MapLocationUnabled) {
        stopBuilding(StopBuildingTarget);
        
        releaseWatchings();
        
        release(_path);
        _movement.dir = 0;
        _pathDelay = 0;
        
        _jumping = new Jumping();
        _jumping->waiting = time;
        _jumping->da = altitude;
        _jumping->gda = altitude * 2 / (float)_jumping->waiting;
        _jumping->dest = p;
        _jumping->commit = false;
    }
}
bool Unit::warp(const FPoint& point) {
	FPoint p = point;
	if (Map::sharedMap()->locatablePosition(this, p, NULL) != MapLocationUnabled) {
		stopBuilding(StopBuildingTarget);

		releaseWatchings();

		release(_path);
		_movement.dir = 0;
		_pathDelay = 0;

		locate(p);
		return true;
	}
    return false;
}

bool Unit::commitFreezing() {
    if (_freezing) {
        if (_state != UnitStateDestroy) {
            stopBuilding(StopBuildingTarget);
            _waitingState = WaitingStateFreeze;
            if (_freezing > _waiting) _waiting = _freezing;
            _freezing = fixed::Zero;
            return true;
        }
        else _freezing = fixed::Zero;
    }
    return false;
}
bool Unit::commitEscaping() {
    if (_escaping && !_escaping->commit) {
        _escaping->commit = true;
        return true;
    }
    return false;
}
bool Unit::commitJumping() {
    if (_jumping && !_jumping->commit) {
        stopBuilding(StopBuildingTarget);

        _jumping->move = (_jumping->dest - _point) / _jumping->waiting;
        _jumping->commit = true;
        return true;
    }
    return false;
}
bool Unit::updateEscaping() {
    if (_escaping && _escaping->commit) {
        fixed frameDelay = Map::sharedMap()->frameDelayFixed();
        _escaping->waiting -= frameDelay;
        if (_escaping->waiting > fixed::Zero) {
            fixed move = ability(AbilityMoveSpeed) * frameDelay;
            
            if (move) {
                setBaseMotion(MotionMove, 0);
                
                FPoint p = _point - (_escaping->point - _point);
                
                turnMotion(direction(p));
                
                Map::sharedMap()->move(this, NULL, p, move, 0);
            }
        }
        else {
            setBaseMotion(MotionStop, 0);
            delete _escaping;
            _escaping = NULL;
        }
        return true;
    }
    return false;
}

bool Unit::updateJumping() {
    if (_jumping && _jumping->commit) {
        fixed frameDelay = Map::sharedMap()->frameDelayFixed();
        
        _jumping->waiting -= frameDelay;
        
        if (_jumping->waiting <= fixed::Zero) {
            locate(_jumping->dest);
            delete _jumping;
            _jumping = NULL;
            _z = 0;
        }
        else {
            locate(_point + _jumping->move * frameDelay);
            _z += _jumping->da * TileSize;
            _jumping->da -= _jumping->gda * (float)frameDelay;
        }
        return true;
    }
    return false;
}

void Unit::spriteElement(CSGraphics *graphics, const byte *data) {
    switch (data[0]) {
        case SpriteOpUnitColorOff:
            graphics->popColor();
            break;
        case SpriteOpUnitColorOn:
            {
                const Force* force = _force;
                CSColor color = force ? force->originColor() : CSColor::White;
                color.r *= 2;
                color.g *= 2;
                color.b *= 2;
                graphics->pushColor();
                graphics->setColor(color);
            }
            break;
		case SpriteOpUnitColorMinimapOn:
			{
				const Force* force = _force;
				CSColor color = force ? force->allianceColor() : CSColor::White;
				graphics->pushColor();
				graphics->setColor(color);
			}
			break;
        default:
            CSWarnLog("unknown sprite extern code:%d", data[0]);
            break;
    }
}

Animation* Unit::captureMotion() const {
	Animation* capture = NULL;
	synchronized(this, GameLockObject) {
		capture = copy(_motion.animation);
	}
	return capture;
}

void Unit::setMotion(const AnimationIndex& index, float rate, bool rewind, bool buff) {
    CSAssert(rate, "invalid state");
	if (buff || !_motion.buff) {
		if (_motion.index != index) {
			_motion.remaining = 0;
			_motion.relocated = true;
		}
		else if (rewind) {
			_motion.remaining = 0;
			_motion.rewind = true;
		}
		_motion.stop = false;
		synchronized(this, GameLockObject) {
			_motion.index = index;
		}
		_motion.rate = rate;
	}
}
bool Unit::turnMotion(int dir) {
    if (_motion.dir != dir) {
        int a = dir - _motion.dir;
        if (a < 0) a += 12;
        int b = _motion.dir - dir;
        if (b < 0) b += 12;
        
        _motion.dir = (_motion.dir + (a < b ? 1 : 11)) % 12;
        return false;
    }
    return true;
}
void Unit::setBaseMotion(int index, bool buff) {
    const MotionData* data = _data->skin(skin)->motions[_motions[index]];
    
    if (data) {
        setMotion(data->animation, data->rateAbility != -1 ? (float)ability(data->rateAbility, fixed::One) : 1.0f, data->rewind, buff);
    }
    else {
		removeMotion(buff);
    }
}
void Unit::cancelBaseMotion(int from, int to) {
	const MotionData* data = _data->skin(skin)->motions[_motions[from]];

	if (data && _motion.index == data->animation) {
		setBaseMotion(to, false);
	}
}
void Unit::stopMotion(bool buff) {
	if (buff || !_motion.buff) {
		_motion.stop = true;
	}
}
void Unit::removeMotion(bool buff) {
	if (buff || !_motion.buff) {
		synchronized(this, GameLockObject) {
			_motion.index = AnimationIndex::None;
		}
	}
}

static void locateMotionAnimation(CSAnimation* element, const CSVector3& wp, float z) {
    static const uint MotionKeyOverBottom = AnimationKeyLayerMiddle | AnimationKeyLayerTop | AnimationKeyPositionBody | AnimationKeyPositionHead;
    static const uint MotionKey = AnimationKeyPosition | MotionKeyOverBottom;
    
    if (element->key() & MotionKey) {
        CSVector3 ewp = (element->key() & AnimationKeyPosition) ? wp : CSVector3::Zero;
        if (z && (element->key() & MotionKeyOverBottom)) {
            ewp.z += z;
        }
        element->setClientPosition(ewp);
    }
    CSAnimationDerivation* derivation = element->derivation();
    if (derivation) {
        foreach(CSAnimation*, child, derivation->animations()) {
            locateMotionAnimation(child, wp, z);
        }
    }
}
void Unit::locateMotion() {
    CSVector3 wp = Map::sharedMap()->convertMapToWorldSpace(_point);
    locateMotionAnimation(_motion.animation->root(), wp, _z);
}
void Unit::updateMotion() {
    if (_waitingState != WaitingStateFreeze && _motion.index) {
		_motion.remaining += Map::sharedMap()->frameDelayFloat() * _motion.rate;
    }
}

void Unit::updateScale() {
	fixed scale = ability(AbilityScale, fixed::One) * (_scaleBase + (level - 1) * _scaleUp);

	if (_scale != scale) {
		if (_located) {
			if (!Map::sharedMap()->relocate(this, true, _type, _data->collider * scale, _collision)) {
				return;
			}
			changeVision(false);
		}
		_scale = scale;
		_collider = _data->collider * scale;
		_radius = _data->radius * (float)scale;

		if (_located) {
			_sight = ability(AbilitySight);
			_visionPoint = _point;
			changeVision(true);
		}
		_motion.rescaled = true;

		release(_path);
		_movement.dir = 0;
		_pathDelay = 0;
	}
}

fixed Unit::buffStack(const BuffIndex& index) const {
    if (_buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            for (Buff* current = _buffs; current; current = current->next) {
                if (current->index == index) {
                    return current->stack;
                }
            }
        }
    }
    return fixed::Zero;
}
bool Unit::isBuffed(const BuffIndex& index, fixed stack) const {
    if (_buffs) {
        synchronized_to_update_thread(this, GameLockObject) {
            for (Buff* current = _buffs; current; current = current->next) {
                if (current->index == index) {
                    return current->stack >= stack;
                }
            }
        }
    }
    return false;
}
void Unit::removeBuff(Buff*& buff, bool lock) {
    if (_located && buff->vision && buff->originForce) {
        Map::sharedMap()->changeVision(this, _visionPoint, _sight, false, buff->originForce->alliance(), false);
    }
    if (lock) this->lock(GameLockObject);
    Buff* next = buff->next;
	if (buff->origin) {
		if (buff->origin->asUnit()) buff->origin->asUnit()->release();
		else if (buff->origin->asSpawn()) buff->origin->asSpawn()->release();
	}
	release(buff->target);
	release(buff->damageOrigins);
    for (int i = 0; i < EffectCount; i++) {
        if (buff->effects[i]) {
			buff->effects[i]->dispose();
			buff->effects[i]->release();
        }
    }
	if (buff->motion) _motion.buff--;
    delete buff;
	buff = next;
    if (lock) this->unlock();
}
void Unit::clearBuffs(bool lock) {
    while (_buffs) {
        removeBuff(_buffs, lock);
    }
}
void Unit::buff(const BuffIndex& index, int timeOp, fixed time, fixed stack, UnitBaseGame* origin) {
    for (Buff* current = _buffs; current; current = current->next) {
        if (current->index == index) {
            if (stack) {
                current->stack = CSMath::min(current->stack + stack, Asset::sharedAsset()->buff(index).maxStack);
            }
			switch (timeOp) {
				case 0:
					current->remaining = fixed::Max;
					break;
				case 1:
					if (time > current->remaining) {
						current->remaining = time;
					}
					break;
				case 2:
					if (current->remaining < fixed::Max - time) {
						current->remaining += time;
					}
					break;
				case 3:
					current->remaining = CSMath::max(current->remaining - time, fixed::Zero);
					break;
				case 4:
					current->remaining = fixed::Zero;
					break;
			}
			return;
        }
    }

	switch (timeOp) {
		case 0:
			time = fixed::Max;
			break;
		case 1:
		case 2:
			break;
		default:
			return;
	}
    Buff** next = &_buffs;
    while (*next) {
        if ((int)(*next)->index > (int)index) break;
        next = &(*next)->next;
    }
    Buff* current = new Buff();
    current->index = index;
	current->origin = origin;
	current->target = NULL;
	if (origin) {
		Unit* unit = origin->asUnit();
		if (unit) {
			unit->retain();
			current->target = unit;
			unit->retain();
		}
		else if (origin->asSpawn()) origin->asSpawn()->retain();

		current->originForce = origin->force();
	}
    current->stack = CSMath::min(stack, Asset::sharedAsset()->buff(index).maxStack);
    current->ip = 0;
    current->loop = 0;
    current->remaining = time;
    current->waiting = fixed::Zero;
	current->revision = fixed::Zero;
	current->damage = fixed::Zero;
	current->damageCount = 0;
	current->damageOrigins = NULL;
	current->vision = false;
    current->cloaking = false;
    current->detecting = false;
	current->resurrecting = false;
	current->motion = false;
    for (int i = 0; i < CooltimeCount; i++) {
        current->cooltimes[i] = fixed::Zero;
    }
    for (int i = 0; i < EffectCount; i++) {
        current->effects[i] = NULL;
    }
    current->next = *next;
    synchronized(this, GameLockObject) {
        *next = current;
    }
}
void Unit::updateBuffs() {
    Buff** current = &_buffs;
    
    while(*current) {
        if (updateBuff(*current)) {
            current = &(*current)->next;
        }
        else {
            removeBuff(*current, true);
        }
    }
}

typename Unit::SendCallReturn Unit::sendCall(Unit* src, bool commit) {
    if (src != this && src->_force == _force) {
        int capacity = ability(AbilityTransportingCapacity);
        
        if (capacity) {
			fixed range = ability(AbilityTransportingRange);
			
			int population = src->_data->population;

            synchronized(this, GameLockObject) {
                if (!_transporting) {
                    if (commit) {
                        _transporting = new Transporting();
                        _transporting->crews = new CSArray<Unit>(MaxTransporting);
                        _transporting->follows = new CSArray<Unit>(MaxTransporting);
                    }
                }
                else {
                    foreach (const Unit*, crew, _transporting->crews) {
                        population += crew->_data->population;
                    }
                }
                
                if ((!_transporting || _transporting->crews->count() < MaxTransporting) && population <= capacity) {
                    fixed d2 = _point.distanceSquared(src->_point);
                    fixed c = _collider + src->_collider + range + EpsilonCollider;
                    if (d2 <= c * c) {
                        if (commit) {
                            _transporting->follows->removeObjectIdenticalTo(src);
                            _transporting->crews->addObject(src);
                            
                            if (_transporting->crews->count() == MaxTransporting) {
                                _transporting->follows->removeAllObjects();
                            }
                            else {
                                int i = 0;
                                while (i < _transporting->follows->count()) {
                                    const Unit* follow = _transporting->follows->objectAtIndex(i);
                                    
                                    int followPopulation = follow->data().population;
                                    
                                    if (population + followPopulation > capacity) {
                                        _transporting->follows->removeObjectAtIndex(i);
                                    }
                                    else {
                                        population += followPopulation;
                                        i++;
                                    }
                                }
                            }
                            _dirtyui = true;
                        }
                        return SendCallReturnNear;
                    }
                    else if (commit && !_transporting->follows->containsObjectIdenticalTo(src)) {
                        _transporting->follows->addObject(src);
                    }
                    return SendCallReturnFar;
                }
            }
        }
    }
    return SendCallReturnNone;
}

void Unit::clearEffects() {
    for (int i = 0; i < EffectCount; i++) {
        if (_effects[i]) {
            _effects[i]->dispose();
            _effects[i]->release();
            _effects[i] = NULL;
        }
    }
}

void Unit::commandAction(CommandStack cs, const CommandIndex& command, const CSArray<byte, 2>* actionTrigger, Object* target, bool stack) {
	switch (cs) {
		case CommandStackEnabled:
			if (stack) stack = _nextActions->count() || _command == command;
			else _nextActions->removeAllObjects();
			new (&_nextActions->addObject()) NextAction(command, actionTrigger, target, stack, true);
			break;
		case CommandStackSingle:
			_nextActions->removeAllObjects();
			new (&_nextActions->addObject()) NextAction(command, actionTrigger, target, false, false);
			break;
		case CommandStackRewind:
			{
				int i = 0;
				while (i < _nextActions->count()) {
					if (_nextActions->objectAtIndex(i).rewind) i++;
					else _nextActions->removeObjectAtIndex(i);
				}
			}
			if (_commandRewind) {
				Action* first = _action;
				while (first->prev) {
					first = first->prev;
				}
				new (&_nextActions->insertObject(0)) NextAction(_command, first->trigger, _originTarget, true, true);
			}
			new (&_nextActions->insertObject(0)) NextAction(command, actionTrigger, target, false, false);
			break;
	}
}

CommandReturn Unit::command(const CommandIndex& key, Object* target, fixed& estimation, int selectionFlag, int commit, bool stack) {
    CommandReturn rtn;
    rtn.state = CommandStateEmpty;
    
    const CommandUnitData* subdata = _data->commands->tryGetObjectForKey(key);

    if (subdata) {
        rtn = executeCommand(key, subdata->commitTrigger, &target, estimation, selectionFlag, commit);
        
        if (commit) {
            SyncLog("Sync (%d) (Command):%d, %d, %d", Map::sharedMap()->frame(), this->key, (uint)key, rtn.state);
            
            if (rtn.state == CommandStateEnabled && subdata->actionTrigger) {
				commandAction(rtn.parameter.enabled.stack, key, subdata->actionTrigger, target, stack);
            }
        }
    }
    return rtn;
}
void Unit::cancelBuildingCommand() {
    stopBuilding(StopBuildingCancel);
}
void Unit::cancelProducingCommand(int slot) {
    if (_producing && slot < _producing->count) {
        const UnitIndex& index = _producing->indices[slot];
        
        const UnitData& data = Asset::sharedAsset()->unit(index);
        
        if (_force) {
            _force->reviseResource(0, (fixed)data.resources[0]);
            _force->reviseResource(1, (fixed)data.resources[1]);
        }
        if (slot == 0) {
            if (_producing->progress) {
                if (_force) {
                    _force->revisePopulation(-data.population * (data.addition + 1));
                }
                _producing->progress = fixed::Zero;
            }
        }
        synchronized(this, GameLockObject) {
            if (--_producing->count == 0) {
                if (!_producing->rallyPoint) {
                    delete _producing;
                    _producing = NULL;
                }
                else {
                    _producing->progress = fixed::Zero;
                    _producing->stuck = 0;
                }
                _dirtyui = true;
            }
            else if (slot != _producing->count) {
                memmove(_producing->indices + slot, _producing->indices + slot + 1, (_producing->count - slot) * sizeof(UnitIndex));
            }
        }
    }
}
bool Unit::cancelProducingCommand(const UnitIndex& index) {
    if (_producing) {
        for (int i = _producing->count - 1; i >= 0; i--) {
            if (_producing->indices[i] == index) {
                cancelProducingCommand(i);
                return true;
            }
        }
    }
    return false;
}
void Unit::cancelTrainingCommand(int slot) {
    if (_training && slot < _training->count) {
        const RuneIndex index = _training->indices[slot];
        
        const RuneData& data = Asset::sharedAsset()->rune(index);
        
        _force->reviseResource(0, (fixed)data.resources[0]);
        _force->reviseResource(1, (fixed)data.resources[1]);
        _force->cancelTraining(index);

        if (slot == 0) {
            _training->progress = fixed::Zero;
        }
        synchronized(this, GameLockObject) {
            if (--_training->count == 0) {
                delete _training;
                _training = NULL;
                
                _dirtyui = true;
            }
            else if (slot != _training->count) {
                memmove(_training->indices + slot, _training->indices + slot + 1, (_training->count - slot) * sizeof(RuneIndex));
            }
        }
    }
}

void Unit::unloadCommand(int slot) {
    if (_transporting && slot < _transporting->crews->count()) {
        Unit* crew = _transporting->crews->objectAtIndex(slot);
        
        FPoint point = _point;
        
        if (Map::sharedMap()->locatablePosition(crew, point, NULL) != MapLocationUnabled) {
            crew->locate(point);
            
            synchronized(this, GameLockObject) {
                _transporting->crews->removeObjectAtIndex(slot);
                
                if (!_transporting->crews->count() && !_transporting->follows->count()) {
                    _transporting->crews->release();
                    _transporting->follows->release();
                    delete _transporting;
                    _transporting = NULL;
                }
            }
            _dirtyui = true;
        }
        else {
            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandMoveUnabled)));
        }
    }
}

Unit::NextAction::NextAction(const CommandIndex& command, const CSArray<byte, 2>* trigger, Object* target, bool stack, bool rewind) : 
	command(command),
	trigger(trigger),
	target(CSObject::retain(target)),
	stack(stack),
	rewind(rewind)
{

}

Unit::NextAction::~NextAction() {
	CSObject::release(target);
}

void Unit::changeAction() {
    while (_action) {
        Action* prev = _action->prev;
        delete _action;
        _action = prev;
    }
    _action = new Action();

    release(_originTarget);

    if (_nextActions->count()) {
        NextAction& nextAction = _nextActions->objectAtIndex(0);
        _action->trigger = nextAction.trigger;
        _command = nextAction.command;
        _commandRewind = nextAction.rewind;
        _originTarget = retain(nextAction.target);
        _nextActions->removeObjectAtIndex(0);
    }
    else {
        _action->trigger = _data->actionTriggers->objectAtIndex(_state);
        _command = CommandIndex::None;
        _commandRewind = false;
    }
    _busy = 0;
    _action->loop = 0;
    _action->ip = 0;
    _action->prev = NULL;
    _revision = fixed::Zero;
    _damage = fixed::Zero;
    _damageCount = 0;
    release(_damageOrigins);
    _waitingReserved = fixed::Zero;
    _waitingReservedOn = false;

    if (_gathering) {
        synchronized(this, GameLockObject) {
            if (_gathering->resource) {
                if (_gathering->occupied) {
                    _gathering->resource->occupy(false);
                }
                _gathering->resource->endGathering();
                _gathering->resource->release();
                _gathering->resource = NULL;
            }
            release(_gathering->base);
            _gathering->waiting = 0;
            if (!_gathering->cargo) {
                delete _gathering;
                _gathering = NULL;
            }
        }
    }
    if (_transporting) {
        if (_transporting->crews->count()) {
            _transporting->follows->removeAllObjects();
        }
        else {
            synchronized(this, GameLockObject) {
                _transporting->crews->release();
                _transporting->follows->release();
                delete _transporting;
                _transporting = NULL;
            }
        }
    }
    if (_originPoint) {
        delete _originPoint;
        _originPoint = NULL;
    }
    stopBuilding(StopBuildingTarget);

    releaseWatchings();
    release(_watchingExclusions);

    retain(_target, _originTarget);
    release(_aggro);
    _aggroDuration = fixed::Zero;

    clearEffects();

    release(_path);
    _movement.dir = 0;
    _pathDelay = 0;
}

bool Unit::interruptAction() {
    if (_state == UnitStateDestroy) {
        return false;
    }
    else if (_alive == AliveNo || _alive == AliveResurrect) {
        _state = UnitStateDestroy;
        
		Map::sharedMap()->voice(VoiceDestroy, this);

        deactivate();
        changeAction();
        return true;
    }
    else {
        if (commitFreezing()) {
            clearEffects();
            return false;
        }
        if (commitJumping()) {
            clearEffects();
            return true;
        }
		if (commitEscaping()) {
			_nextActions->removeAllObjects();
			changeAction();
			return true;
		}
        if (_nextActions->count() && !_nextActions->objectAtIndex(0).stack) {
            changeAction();
			return true;
        }
        return false;
    }
}

void Unit::updateRevisions() {
    if (_revisionTimes) {
        fixed frameDelay = Map::sharedMap()->frameDelayFixed();
        int i = 0;
        while (i < _revisionTimes->count()) {
            RevisionTime& revising = _revisionTimes->objectAtIndex(i);
            
            if (revising.time > frameDelay) {
                revising.time -= frameDelay;
                i++;
            }
            else {
                _revisionTimes->removeObjectAtIndex(i);
            }
        }
    }
	if (_damaging) {
		_damaging = CSMath::max(_damaging - Map::sharedMap()->frameDelayFixed(), fixed::Zero);
	}
    if (_aggroDuration) {
		if (!_force || !_aggro || !_aggro->_force || _force->alliance() == _aggro->_force->alliance() || 
            !_aggro->compareType(ObjectMaskUnit) || !_aggro->isAlive() || !_aggro->_located || !_aggro->isDetected(_force->alliance()))
        {
            release(_aggro);
			_aggroDuration = fixed::Zero;
		}
		else {
			_aggroDuration = CSMath::max(_aggroDuration - Map::sharedMap()->frameDelayFixed(), fixed::Zero);
		}
    }
    else {
        release(_aggro);
    }
}

void Unit::updateDegrees() {
	fixed nextMaxHealth = ability(AbilityHealth);
	fixed nextMaxShield = ability(AbilityShield);
	fixed frameDelay = Map::sharedMap()->frameDelayFixed();
	bool changed = false;

	if (_maxHealth != nextMaxHealth) {
		_health = _health < _maxHealth ? (_health * nextMaxHealth / _maxHealth) : nextMaxHealth;
		_maxHealth = nextMaxHealth;
		changed = true;
	}
	if (_maxShield != nextMaxShield) {
		_shield = CSMath::max(_shield + nextMaxShield - _maxShield, fixed::Zero);
		_maxShield = nextMaxShield;
		changed = true;
	}
	if (!nextMaxHealth && !nextMaxShield) {
        _alive = AliveNone;
        _healthDisplay = 0.0f;
    }
    else {
        if (_alive == AliveNone) _alive = AliveYes;
        if (changed) {
            _healthDisplay = HealthChangeDisplayDuration;
        }
        else if (_healthDisplay) {
            _healthDisplay = CSMath::max(_healthDisplay - (float)frameDelay, 0.0f);
        }
    }
#ifdef UseCheatProgress
    fixed manaRecovery = ability(AbilityManaRecovery) * frameDelay * UseCheatProgress;
#else
    fixed manaRecovery = ability(AbilityManaRecovery) * frameDelay;
#endif
    
    _maxMana = ability(AbilityMana);
    if (_building <= BuildingTargetProgress && _mana < _maxMana) {
        _mana = CSMath::min(_mana + manaRecovery, _maxMana);
    }
    else if (_mana > _maxMana) {
        _mana = _maxMana;
    }
    
    _rangeDisplay = _rangeDisplayValue ? _rangeDisplayValue->value(this) : 0;
    
    for (int i = 0; i < CooltimeCount; i++) {
        fixed& remaining = _cooltimes[i].remaining;
        if (remaining) {
			remaining = CSMath::max(remaining - frameDelay, fixed::Zero);
        }
    }
    if (_pathDelay) {
        _pathDelay--;
    }
}

void Unit::updateSight() {
    fixed sight = ability(AbilitySight);
    
    if (_sight != sight || (_visionPoint != _point && (key + Map::sharedMap()->frame()) % Map::sharedMap()->visionFrameDivision() == 0)) {
		synchronized(Map::sharedMap()->phaseLock(), GameLockMap) {		//시야 깜빡임 현상문제로 동기화처리
			changeVision(_point, sight, true, false);
			changeVision(_visionPoint, _sight, false, false);
		}
        _sight = sight;
        _visionPoint = _point;
    }
}

void Unit::updateProducing() {
    if (_producing) {
        if (_producing->rallyPoint && !_producing->rallyPoint->isVisible(_force)) {
            synchronized(this, GameLockObject) {
                _producing->rallyPoint->release();
                _producing->rallyPoint = NULL;
            }
        }
        if (_producing->count) {
            const UnitIndex& index = _producing->indices[0];
            
            const UnitData& data = Asset::sharedAsset()->unit(index);
            
            int population = data.population * (data.addition + 1);
            
            if (!_producing->progress) {
                if (_force && population) {
                    if (_force->population() + population > _force->supply()) {
                        if (_producing->stuck != 1) {
                            Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandNotEnoughSupply)));
                            _producing->stuck = 1;
                        }
                        return;
                    }
                    _force->revisePopulation(population);
                }
                _producing->stuck = 0;
            }
			
#ifdef UseCheatProgress
            _producing->progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One) * UseCheatProgress;
#else
            _producing->progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One);
#endif
            if (_producing->progress >= data.progress) {
                FPoint p = _producing->rallyPoint ? facingPoint(_producing->rallyPoint->point(), data.collider) : FPoint(_point.x, _point.y + _collider + data.collider);
                
                int summon = 0;
                for (int i = 0; i <= data.addition; i++) {
                    FPoint lp = p;
                    if (Map::sharedMap()->locatablePosition(data.type, lp, data.collider, NULL, MapLocationCollided) != MapLocationUnabled) {
                        Unit* unit = Unit::unit(_force, index, 0, 0, this);
						unit->locate(lp);

						Map::sharedMap()->voice(VoiceSpawn, unit);
						
                        if (_producing->rallyPoint) {
                            fixed estimation = fixed::Zero;
                            unit->command(CommandIndex(CommandSetTarget, 2), _producing->rallyPoint, estimation, 0, 1, false);
                        }
                        summon++;
                    }
                }
                if (summon) {
                    if (summon <= data.addition) {
                        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandSummonUnabled)));
                    }
                    _producing->progress = fixed::Zero;
                    _producing->stuck = 0;
                    if (_force) {
                        _force->revisePopulation(-population);
                        Map::sharedMap()->event(_force, AnimationIndex(AnimationSetGame, AnimationGameMinimapEvent), Asset::playerColor0, _point);
                    }
                    synchronized(this, GameLockObject) {
                        if (--_producing->count) {
                            memmove(_producing->indices, _producing->indices + 1, _producing->count * sizeof(UnitIndex));
                        }
                        else if (!_producing->rallyPoint) {
                            delete _producing;
                            _producing = NULL;
                        }
                    }
                    _dirtyui = true;
                }
                else {
                    _producing->progress = data.progress;
                    if (_producing->stuck != 2) {
                        Map::sharedMap()->pushMessage(_force, Message::message(smallIcon(), Asset::sharedAsset()->string(MessageSetCommand, MessageCommandSummonUnabled)));
                        _producing->stuck = 2;
                    }
                }
            }
        }
    }
}

void Unit::updateTraining() {
    if (_training && _training->count) {
        const RuneIndex& index = _training->indices[0];
        
        const RuneData& data = Asset::sharedAsset()->rune(index);
        
#ifdef UseCheatProgress
        _training->progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One) * UseCheatProgress;
#else
        _training->progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One);
#endif
        if (_training->progress >= data.progress) {
            _training->progress = fixed::Zero;
            
            _force->completeTraining(index);
            
            Map::sharedMap()->event(_force, AnimationIndex(AnimationSetGame, AnimationGameMinimapEvent), Asset::playerColor0, _point);
            
            if (Map::sharedMap()->isPlaying() && _force == Map::sharedMap()->force()) {
                const CSString* msg = CSString::stringWithFormat(*Asset::sharedAsset()->string(MessageSetCommand, MessageCommandTrainingComplete), (const char*)*data.name);

				Map::sharedMap()->pushMessage(Message::message(data.smallIcon, msg));
			}
			
			Map::sharedMap()->announce(_force, AnnounceTrainingComplete);
			
			synchronized(this, GameLockObject) {
                if (--_training->count) {
                    memmove(_training->indices, _training->indices + 1, _training->count * sizeof(RuneIndex));
                }
                else {
                    delete _training;
                    _training = NULL;
                }
            }
            _dirtyui = true;
        }
    }
}

void Unit::updateTransporting() {
    if (_transporting) {
        synchronized(this, GameLockObject) {
            int i = 0;
            while(i < _transporting->crews->count()) {
                if (_transporting->crews->objectAtIndex(i)->isAlive()) {
                    i++;
                }
                else {
                    _transporting->crews->removeObjectAtIndex(i);
                }
            }
            i = 0;
            while(i < _transporting->follows->count()) {
                const Unit* follow = _transporting->follows->objectAtIndex(i);
                if (follow->isLocated() && follow->isAlive()) {
                    i++;
                }
                else {
                    _transporting->follows->removeObjectAtIndex(i);
                }
            }
            if (!_transporting->crews->count() && !_transporting->follows->count()) {
                _transporting->crews->release();
                _transporting->follows->release();
                delete _transporting;
                _transporting = NULL;
            }
        }
    }
}

bool Unit::startBuilding(StartBuilding param) {
    if (_building == BuildingNone) {
        _building = (Building)param;
        _progress = fixed::Zero;
        
        switch (_building) {
            case BuildingDependent:
            case BuildingIndependent:
                if (_alive == AliveYes && _data->progress) {
                    _health = fixed::One;
                }
                break;
        }
        return true;
    }
    return false;
}

bool Unit::stopBuilding(StopBuilding param) {
    if (_target && _target->compareType(ObjectMaskUnit)) {
        Unit* target = static_cast<Unit*>(_target);
        if (target->_transporting) target->_transporting->follows->removeObjectIdenticalTo(this);
    }

    switch (_building) {
        case BuildingTargetReady:
            if (_target) {
                if (_target->compareType(ObjectMaskFrame)) {
                    Frame* target = static_cast<Frame*>(_target);
                    
                    const UnitData& data = target->data();
                    
                    if (_force) {
                        _force->reviseResource(0, (fixed)data.resources[0]);
                        _force->reviseResource(1, (fixed)data.resources[1]);
                        _force->revisePopulation(-data.population);
                    }
                }
                else if (_target->compareType(ObjectMaskUnit)) {
                    Unit* target = static_cast<Unit*>(_target);
                    if (target->_building == BuildingReserve) {
                        if (target->_reserveBuildingCount) target->_reserveBuildingCount--;
                        else {
                            target->_building = BuildingNeglact;
                            Map::sharedMap()->announce(_force, AnnounceBuildStopped);
                        }
                    }
                }
            }
            _building = BuildingNone;
            break;
        case BuildingTargetProgress:
            if (_target && _target->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(_target);
                if (target->_building == BuildingDependent) {
                    target->_building = BuildingNeglact;
					Map::sharedMap()->announce(_force, AnnounceBuildStopped);
                }
            }
            _building = BuildingNone;
            break;
        case BuildingNone:
            if (param == StopBuildingCancel && _alive == AliveYes) {
                _alive = AliveNo;
                _health = fixed::Zero;
                _shield = fixed::Zero;
            }
            break;
        case BuildingDependent:
        case BuildingNeglact:
        case BuildingReserve:
        case BuildingIndependent:
            switch (param) {
                case StopBuildingCancel:
                    if (_alive == AliveYes) {
                        if (_force) {
                            _force->reviseResource(0, (fixed)_data->resources[0]);
                            _force->reviseResource(1, (fixed)_data->resources[1]);
                        }
                        _alive = AliveNo;
                        _health = fixed::Zero;
                        _shield = fixed::Zero;
                    }
                case StopBuildingDestroy:
                    switch (_building) {
                        case BuildingDependent:
                        case BuildingReserve:
                            _building = BuildingNeglact;
							Map::sharedMap()->announce(_force, AnnounceBuildStopped);
                            break;
                    }
                    break;
            }
            break;
        case BuildingTransform:
            if (param == StopBuildingCancel) {
                if (_alive != AliveYes || !resetIndex()) {
                    return false;
                }
                if (_force) {
                    _force->reviseResource(0, (fixed)_data->resources[0]);
                    _force->reviseResource(1, (fixed)_data->resources[1]);
                }
                _building = BuildingNone;
            }
            break;
    }
    return true;
}

bool Unit::reserveBuilding() {
    switch (_building) {
        case BuildingNeglact:
            _building = BuildingReserve;
            _reserveBuildingCount = 0;
            return true;
        case BuildingReserve:
            _reserveBuildingCount++;
            return true;
    }
    return false;
}

bool Unit::resumeBuilding() {
    if (_building == BuildingReserve) {
        _building = BuildingDependent;
        return true;
    }
    return false;
}

void Unit::updateBuilding() {
    switch (_building) {
        case BuildingTargetReady:
            if (_target) {
                if (_target->compareType(ObjectMaskUnit)) {
                    Unit* target = static_cast<Unit*>(_target);
                    if (target->_building == BuildingReserve) {
                        return;
                    }
                }
                else if (_target->compareType(ObjectMaskFrame)) {
                    return;
                }
            }
            _building = BuildingNone;
            break;
        case BuildingTargetProgress:
            if (_target && _target->compareType(ObjectMaskUnit)) {
                Unit* target = static_cast<Unit*>(_target);
                if (target->_building == BuildingDependent) {
                    return;
                }
            }
            _building = BuildingNone;
            break;
        case BuildingIndependent:
        case BuildingDependent:
            if (_alive == AliveYes) {
                fixed progress = (fixed)(_data->progress * Map::sharedMap()->framePerSecond());
                
                fixed workingSpeed = ability(AbilityWorkingSpeed, fixed::One);

                if (progress && workingSpeed) {
                    progress /= workingSpeed;
                    
#ifdef UseCheatProgress
                    _health += _maxHealth * UseCheatProgress / progress;
#else
                    _health += _maxHealth / progress;
#endif
                    if (_health > _maxHealth) {
                        _health = _maxHealth;
                    }
                }
            }
        case BuildingTransform:
#ifdef UseCheatProgress
            _progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One) * UseCheatProgress;
#else
            _progress += Map::sharedMap()->frameDelayFixed() * ability(AbilityWorkingSpeed, fixed::One);
#endif
            if (_progress >= _data->progress) {
                if (_force) {
                    Map::sharedMap()->event(_force, AnimationIndex(AnimationSetGame, AnimationGameMinimapEvent), Asset::playerColor0, _point);
                }
                _originIndex = _index;
                _progress = fixed::Zero;
                _building = BuildingNone;

                Map::sharedMap()->voice(VoiceSpawn, this);

                if (compareType(ObjectMaskBase)) {
					Map::sharedMap()->announce(_force, AnnounceBuildComplete);
				}
            }
            break;
    }
}

bool Unit::updateWaiting() {
    fixed frameDelay = Map::sharedMap()->frameDelayFixed();
    
    if (_waitingReservedOn) {
        _waitingReserved += frameDelay;
    }
    if (_waiting) {
        _waiting -= frameDelay;
        if (_waiting > fixed::Zero) {
            if (_waitingState == WaitingStateInterrupt) {
                if (!interruptAction()) return true;
            }
            else if (_state != UnitStateDestroy && (_alive == AliveNo || _alive == AliveResurrect)) {
                _state = UnitStateDestroy;
                
				Map::sharedMap()->voice(VoiceDestroy, this);

                deactivate();
                changeAction();
            }
            else {
                return true;
            }
        }
        _waiting = fixed::Zero;
    }
    _waitingState = WaitingStateNone;
    return false;
}

bool Unit::endAction() {
    if (_state == UnitStateDestroy) {
        if (_alive == AliveResurrect) {
            recover(_resurrection);
            _resurrection = fixed::Zero;
            _state = UnitStateJoin;
			_nextActions->removeAllObjects();
        }
		else if (_resurrecting) {
			return false;
		}
        else {
            return true;
        }
    }
    else if (_alive == AliveNo) {
        _state = UnitStateDestroy;
        
		Map::sharedMap()->voice(VoiceDestroy, this);

        deactivate();
    }
    else {
        if (!commitJumping() && !commitFreezing()) {
            commitEscaping();
        }
    }
    changeAction();
    return false;
}

bool Unit::update(bool running) {
    Object::update(running);
    
    bool alive = true;
    
    updateSpeech();
    
    if (running) {
        updateRevisions();
        updateWatchings();
        updateDegrees();
        if (_located) {
            updateMotion();
            updateSight();
            updateBuffs();
            if (updateWaiting()) {
                goto fin;
            }
            updateProducing();
            updateTraining();
            updateTransporting();
            updateBuilding();
            if (updateJumping()) {
                goto fin;
            }
            if (updateEscaping()) {
                goto fin;
            }
            if (updateAction()) {
                goto fin;
            }
            if (endAction()) {
                alive = false;
                goto fin;
            }
        }
        else if (_alive == AliveNo || _alive == AliveResurrect) {
            alive = false;
            goto fin;
        }
    fin:
        _movement.update();
    }
#ifdef UseSyncTest
    if (!alive) {
        SyncLog("Sync (%d) (Destroy):%d", Map::sharedMap()->frame(), key);
    }
#endif
    return alive;
}

//======================================================================================================================================================
//from main thread

float Unit::damagingDisplay() const {
    float damaging = _damaging - DamagingDuration + DamagingDisplayDuration;
    
    return damaging > 0 ? damaging / DamagingDisplayDuration : 0.0f;
}

CSColor Unit::motionColor() const {
    float damaging = damagingDisplay();
    
    CSColor color;
    
    if (damaging) {
        static const CSColor3 DamageEnemyMotionColor(1.0f, 0.0f, 0.0f);
        static const CSColor3 DamageMotionColor(2.0f, 2.0f, 2.0f);
        
        const Force* playerForce = Map::sharedMap()->force();
        
        const Force* force = _force;
        
        const CSColor3& damageColor = force && playerForce && force->alliance() != playerForce->alliance() ? DamageEnemyMotionColor : DamageMotionColor;
        
        color = CSColor3::lerp(CSColor3::White, damageColor, damaging);
    }
    else {
        color = CSColor::White;
    }
	color.a = _alpha;
    
    return color;
}

CSColor Unit::selectionColor() const {
    const Force* force = _force;
    
    float damaging = damagingDisplay();
    
    CSColor color;
    
    if (!force) {
        color = Asset::neutralColor;
    }
    else {
        const Force* playerForce;
        
        if (Map::sharedMap()->isFocusing() && (playerForce = Map::sharedMap()->force())) {
            if (force == playerForce) {
                synchronized(Map::sharedMap()->selectionLock(), GameLockMap) {
                    color = Map::sharedMap()->selections()->containsObjectIdenticalTo(this) ? Asset::playerColor0 : Asset::playerColor1;
                }
            }
            else {
                if (force->alliance() == playerForce->alliance()) {
                    color = Asset::allianceColor;
                }
                else {
                    color = Asset::enemyColor;
                }
            }
        }
        else {
            color = force->originColor();
        }
    }
    if (damaging) {
        static const CSColor3 DamageSelectionColor(1.0f, 1.0f, 1.0f);
        
        color.r = CSMath::lerp(color.r, DamageSelectionColor.r, damaging);
        color.g = CSMath::lerp(color.g, DamageSelectionColor.g, damaging);
        color.b = CSMath::lerp(color.b, DamageSelectionColor.b, damaging);
    }
    return color;
}

void Unit::drawRallyPoint(CSGraphics* graphics, CSArray<const Object>* targets) {
    const Object* rallyPoint = NULL;
    if (_producing) {
        synchronized(this, GameLockObject) {
            if (_producing && _producing->rallyPoint) {
                rallyPoint = retain(_producing->rallyPoint);
            }
        }
    }
    if (rallyPoint) {
        if (!targets->containsObjectIdenticalTo(rallyPoint)) targets->addObject(rallyPoint);
        
        graphics->pushColor();
        graphics->setColor(CSColor(1.0f, 1.0f, 0.0f, _alpha * 0.75f));
        graphics->drawLineImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLine), Map::sharedMap()->frame(), worldPoint(ObjectLayerBottom), rallyPoint->worldPoint(ObjectLayerMiddle));
        graphics->popColor();
        
        rallyPoint->release();
    }
}

void Unit::drawBottomCursor(CSGraphics* graphics) {
    if (_alpha && (_alive == AliveYes || _alive == AliveNone)) {
        CSVector3 p = worldPoint(ObjectLayerBottom);
        
        const Force* force = _force;
        
        graphics->push();

		float hw = _radius * TileSize;

        bool selected;
        
        synchronized(Map::sharedMap()->selectionLock(), GameLockMap) {
            selected = (force == Map::sharedMap()->force() ? Map::sharedMap()->expandedSelections()->containsObjectIdenticalTo(this) : Map::sharedMap()->selectedTarget() == this);
        }
        if (selected) {
            int r = _radius;
            if (r > 4) {
                r = 4;
            }
            CSColor selectionColor = this->selectionColor();
            selectionColor.a *= _alpha;
            graphics->setColor(selectionColor);
            graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameSelectCursor_0 + r),
                                CSZRect(p.x - hw, p.y - hw, p.z, hw * 2, hw * 2));
        }
        if (selected || Map::sharedMap()->lightDegree() == 0) {
            float r = 0;
            if (_distanceDisplay) {
                r = _distanceDisplay + _collider;
            }
            else if (_rangeDisplay) {
                r = _rangeDisplay + _collider;
            }
            else if (compareType(ObjectMaskTower)) {
                r = ability(AbilityAttackRange) + _collider;
            }
            if (r) {
                r *= TileSize;
                
                graphics->setColor(CSColor(1.0f, 1.0f, 0.0f, _alpha * 0.75f));
                graphics->drawCircle(CSZRect(p.x - r, p.y - r, p.z, r * 2, r * 2), false);
            }
        }
#ifdef UsePathTest
        synchronized(Map::sharedMap()->updateLock(), GameLockMap) {
            if (_path && _target) {
                graphics->setColor(_path->isComplete() ? CSColor::White : CSColor::Black);
                CSVector3 p0, p1;
                p0 = Map::sharedMap()->convertMapToWorldSpace(_target->point());
                foreach(const FPoint&, point, _path->points()) {
                    p1 = Map::sharedMap()->convertMapToWorldSpace(point);
                    graphics->drawLine(p0, p1);
                    p0 = p1;
                }
                graphics->drawLine(p0, Map::sharedMap()->convertMapToWorldSpace(_point));
                
                graphics->setColor(CSColor::Red);
                graphics->drawLine(Map::sharedMap()->convertMapToWorldSpace(_point), Map::sharedMap()->convertMapToWorldSpace(_path->point(_point, _target->point(), false, NULL)));
            }
        }
#endif
        graphics->pop();
    }
}

void Unit::drawGage(CSGraphics* graphics, const CSRect& rect, float progress, const CSColor& color, float ext) {
    const CSImage* image0 = Asset::sharedAsset()->image(ImageSetGame, ImageGameGage_0);
    const CSImage* image1 = Asset::sharedAsset()->image(ImageSetGame, ImageGameGage_1);
    
    if (image0 && image1) {
        float scale = rect.size.height / image0->height();
        
        graphics->begin();
        
        float lu = (image0->frame().left() + 1) / image0->textureWidth();
        float ru = (image0->frame().right()) / image0->textureWidth();
        float tv = image0->frame().top() / image0->textureHeight();
        float bv = image0->frame().bottom() / image0->textureHeight();
        
        float w0 = image0->width() * scale;
        graphics->addVertex(CSVertex(CSVector3(rect.left(), rect.top(), 0), CSVector2(lu, tv)));
        graphics->addVertex(CSVertex(CSVector3(rect.right() - w0, rect.top(), 0), CSVector2(lu, tv)));
        graphics->addVertex(CSVertex(CSVector3(rect.right(), rect.top(), 0), CSVector2(ru, tv)));
        graphics->addVertex(CSVertex(CSVector3(rect.left(), rect.bottom(), 0), CSVector2(lu, bv)));
        graphics->addVertex(CSVertex(CSVector3(rect.right() - w0, rect.bottom(), 0), CSVector2(lu, bv)));
        graphics->addVertex(CSVertex(CSVector3(rect.right(), rect.bottom(), 0), CSVector2(ru, bv)));
        graphics->addIndex(0);
        graphics->addIndex(1);
        graphics->addIndex(3);
        graphics->addIndex(1);
        graphics->addIndex(4);
        graphics->addIndex(3);
        graphics->addIndex(1);
        graphics->addIndex(2);
        graphics->addIndex(4);
        graphics->addIndex(2);
        graphics->addIndex(5);
        graphics->addIndex(4);
        graphics->end(image0->root(), GL_TRIANGLES);
        
        float w1 = image1->width() * scale;
        float w = (rect.size.width - 4 * scale) * progress;
        float h = rect.size.height - 4 * scale;
        float y = rect.origin.y + 2 * scale;
        
        lu = (image1->frame().left() + 1) / image1->textureWidth();
        ru = (image1->frame().right()) / image1->textureWidth();
        tv = image1->frame().top() / image1->textureHeight();
        bv = image1->frame().bottom() / image1->textureHeight();
        
        if (w <= w1) {
            float lw = w / scale;
            graphics->setColor(color);
            graphics->drawImage(image1, CSRect(rect.left(), y, w, h), CSRect(image1->width() - lw, 0, lw, image1->height()));
            graphics->setColor(CSColor::White);
        }
        else if (ext != 1.0f) {
            float ew = CSMath::min(w * ext, w - w1);
            
            CSColor ec(0.75f, 1.0f, 1.0f, color.a);
            
            graphics->begin();
            graphics->addVertex(CSVertex(CSVector3(rect.left(), y, 0), color, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + ew, y, 0), color, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + ew, y, 0), ec, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w - w1, y, 0), ec, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w, y, 0), ec, CSVector2(ru, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left(), y + h, 0), color, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + ew, y + h, 0), color, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + ew, y + h, 0), ec, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w - w1, y + h, 0), ec, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w, y + h, 0), ec, CSVector2(ru, bv)));
            graphics->addIndex(0);
            graphics->addIndex(1);
            graphics->addIndex(5);
            graphics->addIndex(1);
            graphics->addIndex(6);
            graphics->addIndex(5);
            graphics->addIndex(2);
            graphics->addIndex(3);
            graphics->addIndex(7);
            graphics->addIndex(3);
            graphics->addIndex(8);
            graphics->addIndex(7);
            graphics->addIndex(3);
            graphics->addIndex(4);
            graphics->addIndex(8);
            graphics->addIndex(4);
            graphics->addIndex(9);
            graphics->addIndex(8);
            graphics->end(image1->root(), GL_TRIANGLES);
        }
        else {
            graphics->begin();
            graphics->addVertex(CSVertex(CSVector3(rect.left(), y, 0), color, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w - w1, y, 0), color, CSVector2(lu, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w, y, 0), color, CSVector2(ru, tv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left(), y + h, 0), color, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w - w1, y + h, 0), color, CSVector2(lu, bv)));
            graphics->addVertex(CSVertex(CSVector3(rect.left() + w, y + h, 0), color, CSVector2(ru, bv)));
            graphics->addIndex(0);
            graphics->addIndex(1);
            graphics->addIndex(3);
            graphics->addIndex(1);
            graphics->addIndex(4);
            graphics->addIndex(3);
            graphics->addIndex(1);
            graphics->addIndex(2);
            graphics->addIndex(4);
            graphics->addIndex(2);
            graphics->addIndex(5);
            graphics->addIndex(4);
            graphics->end(image1->root(), GL_TRIANGLES);
        }
    }
}

void Unit::drawTopCursor(CSGraphics* graphics) {
    if (_alpha && (_speech || _alive == AliveYes || _alive == AliveNone)) {
        const Force* force = _force;
        
        graphics->push();
        graphics->translate(worldPoint(ObjectLayerTop));
        graphics->transform(graphics->camera().view().billboard(NULL));
        graphics->setStrokeColor(CSColor::Black);
        
        if (_speech) {
            synchronized(this, GameLockObject) {
                if (_speech) {
                    CSSize size = graphics->stringSize(_speech->msg, 200);
                    
                    if (_speech->remaining < SpeechTransitionDuration) {
                        graphics->scale(CSMath::sin(_speech->remaining / SpeechTransitionDuration * FloatPiOverTwo));
                    }
                    else if (_speech->remaining > SpeechDuration - SpeechTransitionDuration) {
                        graphics->scale(CSMath::sin((SpeechDuration - _speech->remaining) / SpeechTransitionDuration * FloatPiOverTwo));
                    }
                    graphics->setColor(CSColor::TranslucentBlack);
                    graphics->drawRoundRect(CSRect(-size.width * 0.5f - 5, -size.height - 10, size.width + 10, size.height + 10), 5, true);
                    graphics->setColor(CSColor::White);
                    graphics->drawString(_speech->msg, CSRect(-size.width * 0.5f, -size.height - 5, size.width, size.height));
                }
            }
        }
		else {
            if (force == Map::sharedMap()->force()) {
                const char* str = NULL;
                bool isFull = false;
                int imageIndex = -1;
                
                if (_building >= BuildingTransform && _data->progress) {
                    str = CSString::cstringWithFormat(*Asset::sharedAsset()->string(MessageSetCommon, MessageCommonTimeSecond), (int)CSMath::ceil((fixed)_data->progress - _progress));        //TODO
                    imageIndex = ImageGameBuildGage;
                }
                else if (compareType(ObjectMaskBase)) {
                    Map::GatheringStateReturn state = Map::sharedMap()->findGatheringState(this, 0);
                    
                    if (state.current && state.max) {
                        isFull = state.current > state.max;
                        str = CSString::cstringWithFormat("%d / %d", state.current, state.max);
                        imageIndex = ImageGameGatherGage;
                    }
                }
                else if (compareType(ObjectMaskRefinery)) {
                    const Resource* r = Map::sharedMap()->findGas(_point);
                    
                    if (r) {
                        int gathering = r->gatherings();
                        int maxGathering = r->data().gatherings;
                        
                        if (gathering) {
                            isFull = gathering > maxGathering;
                            str = CSString::cstringWithFormat("%d / %d", gathering, maxGathering);
                            imageIndex = ImageGameGatherGage;
                        }
                    }
                }
                else if (_transporting) {
                    float rate = 0;
                    
                    int population = 0;
                    
                    synchronized(this, GameLockObject) {
                        if (_transporting && _transporting->crews->count()) {
                            rate = (float)_transporting->crews->count() / MaxTransporting;
                                
                            foreach (const Unit*, crew, _transporting->crews) {
                                population += crew->data().population;
                            }
                        }
                    }
                    if (rate) {
                        int capacity = ability(AbilityTransportingCapacity);
                        
                        if (capacity) rate = CSMath::max((float)population / capacity, rate);
                            
                        isFull = rate >= 1.0f;
                        str = CSString::cstringWithFormat("%d %%", (int)(rate * 100));
                        imageIndex = ImageGameTransportGage;
                    }
                }
                if (str) {
                    graphics->setColor(CSColor::White);
                    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, imageIndex), CSVector2(0, -14), CSAlignCenterMiddle);
                    if (isFull) {
                        graphics->setColor(CSColor::Red);
                    }
                    graphics->setStrokeWidth(2);
                    graphics->drawString(str, CSVector2(18, -15), CSAlignCenterMiddle);
                    graphics->setStrokeWidth(0);
                    
                    graphics->translate(CSVector2(0, -40));
                }
            }
            CSColor progressColor;
            float progress = 0.0f;
            if (_building >= BuildingTransform && _data->progress) {
                progressColor = CSColor(1.0f, 0.5f, 1.0f, 1.0f);
                progress = _progress / _data->progress;
            }
            if (!progress) {
                for (int i = 0; i < CooltimeCount; i++) {
                    if (_cooltimes[i].displaying) {
                        fixed initial = _cooltimes[i].initial;
                        if (initial) {
                            fixed remaining = _cooltimes[i].remaining;
                            if (remaining) {
                                progressColor = CSColor(0.5f, 0.5f, 0.5f, 1.0f);
                                progress = remaining / initial;
                                break;
                            }
                        }
                    }
                }
            }
            if (!progress) {
                if (_producing || _training) {
                    synchronized(this, GameLockObject) {
                        if (_producing && _producing->count) {
                            const UnitIndex& index = _producing->indices[0];
                            const UnitData& subdata = Asset::sharedAsset()->unit(index);
                            
                            if (subdata.progress) {
                                progressColor = CSColor(1.0f, 0.5f, 1.0f, 1.0f);
                                progress = _producing->progress / subdata.progress;
                            }
                        }
                        else if (_training && _training->count && force) {
                            const RuneIndex& index = _training->indices[0];
                            const RuneData& subdata = Asset::sharedAsset()->rune(index);
                            
                            if (subdata.progress) {
                                progressColor = CSColor(1.0f, 0.5f, 1.0f, 1.0f);
                                progress = _training->progress / subdata.progress;
                            }
                        }
                    }
                }
            }
            if (!progress) {
                fixed mana = this->mana();
                fixed maxMana = this->maxMana();
                
                if (mana && mana < maxMana) {
                    progressColor = CSColor(0.5f, 1.0f, 1.0f, 1.0f);
                    progress = mana / maxMana;
                }
                else if (compareType(ObjectMaskRefinery)) {
                    const Resource* r = Map::sharedMap()->findGas(_point);
                    
                    if (r) {
                        progressColor = CSColor(1.0f, 0.5f, 1.0f, 1.0f);
                        progress = r->remaining() / r->amount;
                    }
                }
            }
            fixed maxHealth = this->maxHealth();
            fixed health = this->health();
            
            bool healthGage = _healthDisplay || health < maxHealth;
            
            if (healthGage || progress) {
                float hw = _radius * TileSize;
                
                CSVector2 p;
                
                CSColor selectionColor = this->selectionColor();
                
                static const uint ObjectMaskLargeGage = ObjectMaskBase | ObjectMaskBarrack | ObjectMaskRefinery | ObjectMaskBuilding;
                
                graphics->setColor(CSColor::White);
                
                if (compareType(ObjectMaskLargeGage)) {
                    float x = -hw + 17;
                    float y = progress ? -36 : -26;
                    
                    if (healthGage) {
                        drawGage(graphics, CSRect(x, y, hw * 2, 18), health / maxHealth, selectionColor, health ? _health / health : fixed::Zero);
                        y += 20;
                    }
                    else {
                        y = -24;
                    }
                    if (progress) {
                        drawGage(graphics, CSRect(x, y, hw * 2, 14), progress, progressColor);
                    }
                    
                    p = CSVector2(-hw, -19);
                    
                    graphics->setColor(selectionColor);
                    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameGageIcon_0), p, CSAlignCenterMiddle);
                    if (compareType(ObjectMaskBase)) {
                        graphics->setColor(CSColor::White);
                        graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameGageIcon_2), p, CSAlignCenterMiddle);
                    }
                }
                else {
                    float x = -hw + 10;
                    float y = progress ? -22 : -17;
                    
                    if (healthGage) {
                        drawGage(graphics, CSRect(x, y, hw * 2, 10), health / maxHealth, selectionColor, health ? _health / health : fixed::Zero);
                        y += 12;
                    }
                    else {
                        y = -16;
                    }
                    if (progress) {
                        drawGage(graphics, CSRect(x, y, hw * 2, 8), progress, progressColor);
                    }
                    
                    p = CSVector2(-hw, -12);
                    graphics->setColor(selectionColor);
                    graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameGageIcon_1), p, CSAlignCenterMiddle);
                }
                graphics->setColor(CSColor::White);
                graphics->setStrokeWidth(2);
                graphics->drawDigit(level, false, CSVector2(p.x, p.y - 2), CSAlignCenterMiddle);
                graphics->setStrokeWidth(0);
                
                graphics->translate(CSVector2(0, -45));
            }
            if (force && compareType(ObjectMaskBase)) {
                graphics->setColor(force->originColor());
                graphics->setStrokeWidth(2);
                graphics->drawString(force->player->name, CSVector2::Zero, CSAlignCenterBottom);
                graphics->setStrokeWidth(0);
            }
        }
        graphics->pop();
    }
}


void Unit::draw(CSGraphics *graphics, ObjectLayer layer) {
    switch (layer) {
		case ObjectLayerReady:
			{
				AnimationIndex motion;
				synchronized(this, GameLockObject) {
					motion = _motion.index;
				}
				float remaining = _motion.remaining;
				bool relocated = _motion.relocated;
				bool rescaled = _motion.rescaled;
				bool rewind = _motion.rewind;
				bool stop = _motion.stop;

				_motion.remaining = 0;
				_motion.relocated = false;
				_motion.rescaled = false;
				_motion.rewind = false;
				_motion.stop = false;

				if (motion) {
					if (!_motion.animation || _motion.animation->index() != motion) {
						synchronized(this, GameLockObject) {
							if (_motion.animation) _motion.animation->release();
							_motion.animation = new Animation(motion, Map::ObjectAnimationSoundDelegate, false);
							_motion.animation->setSpriteDelegate0(this, &Unit::spriteElement);
							_motion.animation->checkForLayered();
							locateMotion();
							_motion.animation->setClientScale(_scale);
						}
					}
					else {
						if (relocated) locateMotion();
						if (rescaled) _motion.animation->setClientScale(_scale);
						if (rewind) _motion.animation->rewind();
						if (stop) _motion.animation->stop();
					}

					if (remaining && !_motion.animation->update(remaining, &graphics->camera())) {
						synchronized(this, GameLockObject) {
							_motion.animation->release();
							_motion.animation = NULL;
							_motion.index = AnimationIndex::None;
						}
					}
				}
				else if (_motion.animation) {
					synchronized(this, GameLockObject) {
						_motion.animation->release();
						_motion.animation = NULL;
					}
				}
			}
			break;
		case ObjectLayerBottomCursor:
			if (_motion.animation) drawBottomCursor(graphics);
			break;
		case ObjectLayerShadow:
		case ObjectLayerBottom:
        case ObjectLayerMiddle:
        case ObjectLayerTop:
			if (_motion.animation) {
#ifdef UsePathTest
				if (compareType(ObjectMaskLandmark)) {
					break;
				}
#endif
				graphics->pushColor();
				graphics->setColor(motionColor());
				_motion.animation->drawLayered(graphics, animationVisibleKey(_motion.dir, layer), layer == ObjectLayerShadow);
				graphics->popColor();
			}
            break;
        case ObjectLayerTopCursor:
			if (_motion.animation) drawTopCursor(graphics);
            break;
    }
}

void Unit::drawMinimap(CSGraphics* graphics, float scale, float delta) {
	if (_minimap && isVisible(Map::sharedMap()->force())) {
		Animation* minimap = NULL;
		synchronized(this, GameLockObject) {
			minimap = retain(_minimap);
		}
		if (minimap) {
			CSVector2 pos = ((CSVector2)_point) * scale;

			minimap->update(delta);

			minimap->drawScaled(graphics, pos, scale);
			
			float r = _rangeDisplay * scale;

			if (r) {
				const Force* force = _force;
				if (force) graphics->setColor(force->allianceColor());
				graphics->drawCircle(CSRect(pos.x - r, pos.y - r, r * 2, r * 2), false);
				graphics->setColor(CSColor::White);
			}
			minimap->release();
		}
	}
}

void Unit::updateSpeech() {
    if (_speech) {
        synchronized(this, GameLockObject) {
            if (_speech) {
                _speech->remaining -= Map::sharedMap()->frameDelayFloat();
                if (_speech->remaining <= 0.0f) {
                    _speech->msg->release();
                    delete _speech;
                    _speech = NULL;
                }
            }
        }
    }
}

void Unit::addSpeech(const CSString *msg) {
	if (msg) {
		synchronized(this, GameLockObject) {
			if (!_speech) {
				_speech = new Speech();
				_speech->msg = retain(msg);
			}
			else {
				retain(_speech->msg, msg);
			}
			_speech->remaining = SpeechDuration;
		}
	}
}

void Unit::removeSpeech() {
    synchronized(this, GameLockObject) {
        if (_speech && _speech->remaining > SpeechTransitionDuration) {
            _speech->remaining = SpeechTransitionDuration;
        }
    }
}

float Unit::speechRemaining() const {
    float rtn = 0;
    
    if (_speech) {
        synchronized(this, GameLockObject) {
            if (_speech && _speech->remaining > SpeechTransitionDuration) {
                rtn = _speech->remaining - SpeechTransitionDuration;
            }
        }
    }

    return rtn;
}

float Unit::progress() const {
    switch (_building) {
        case BuildingTargetProgress:
            {
                const Object* target = retain(_target);
            
                if (target) {
                    if (target->compareType(ObjectMaskUnit)) {
                        const Unit* unit = static_cast<const Unit*>(target);

                        if (unit->_building == BuildingDependent) {
                            fixed progress = unit->_progress;
                            fixed maxProgress = (fixed)unit->_data->progress;

                            target->release();

                            return _progress < maxProgress ? (float)(_progress / maxProgress) : 1.0f;
                        }
                    }
                    target->release();
                }
            }
            break;
        case BuildingTransform:
        case BuildingIndependent:
        case BuildingDependent:
        case BuildingNeglact:
            {
                fixed progress = _progress;

                return progress < _data->progress ? (float)(progress / _data->progress) : 1.0f;
            }
    }

    const Force* force = _force;
    
    if (_producing) {
        synchronized(this, GameLockObject) {
            if (_producing && _producing->count) {
                const UnitIndex& index = _producing->indices[0];
                const UnitData& data = Asset::sharedAsset()->unit(index);
                
                return _producing->progress < data.progress ? (float)(_producing->progress / data.progress) : 1.0f;
            }
        }
    }
    if (_training && force) {
        synchronized(this, GameLockObject) {
            if (_training && _training->count) {
                const RuneIndex& index = _training->indices[0];
                const RuneData& data = Asset::sharedAsset()->rune(index);
                
                return _training->progress < data.progress ? (float)(_training->progress / data.progress) : 1.0f;
            }
        }
    }
    return 0.0f;
}
int Unit::workingCount() const {
    switch (_building) {
        case BuildingTargetProgress:
        case BuildingTransform:
        case BuildingIndependent:
        case BuildingDependent:
        case BuildingNeglact:
            return 1;
    }
    if (_producing) {
        synchronized(this, GameLockObject) {
            if (_producing && _producing->count) {
                return _producing->count;
            }
        }
    }
    if (_training) {
        synchronized(this, GameLockObject) {
            if (_training && _training->count) {
                return _training->count;
            }
        }
    }
    return 0;
}

