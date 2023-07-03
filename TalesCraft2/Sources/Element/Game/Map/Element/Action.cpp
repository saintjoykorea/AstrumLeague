//
//  Action.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Action.h"

ActionTarget::ActionTarget(const FPoint& point, fixed range) : _type(ActionTargetPoint) {
    _target.point.point = point;
    _target.point.range = range;
}
ActionTarget::ActionTarget(const CSArray<Object>* objects) : _type(ActionTargetObject) {
    _target.objects = objects;
    _target.objects->retain();
}
ActionTarget::~ActionTarget() {
    switch (_type) {
		case ActionTargetObject:
			_target.objects->release();
			break;
	}
}

bool ActionTarget::accept(const Object* target, const FPoint& point) const {
    switch (_type) {
		case ActionTargetPoint:
			return (!target || target->isLocated()) && point.distance(_target.point.point) <= _target.point.range;
		case ActionTargetObject:
            return target && _target.objects->containsObjectIdenticalTo(target);
    }
    return false;
}

Action::Action(int func, int ip, ActionCode code, const CSArray<Unit>* units, const CommandIndex* command, const ActionTarget* target, bool all, uint restriction) :
	func(func),
	ip(ip),
    _code(code),
    _units(CSObject::retain(units)),
    _command(command),
    _target(CSObject::retain(target)),
    _all(all),
    _restriction(restriction),
    _passed(false)
{
}

Action::~Action() {
    CSObject::release(_units);
	CSObject::release(_target);
}

bool Action::pass(ActionCode code, const CSArray<Unit> *units, const CommandIndex *command, const Object* target) {
    if (!_passed && _code == code) {
        if (_units) {
            if (units) {
                bool flag = false;
                foreach (const Unit*, unit, _units) {
                    if (units->containsObjectIdenticalTo(unit)) {
                        flag = true;
                        if (!_all) {
                            break;
                        }
                    }
                    else if (_all) {
                        return false;
                    }
                }
                if (!flag) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        if (_command) {
            if (!command || *command != *_command) {
                return false;
            }
        }
        if (_target) {
            if (!target || !_target->accept(target, target->point())) {
                return false;
            }
        }
        _passed = true;
    }
    return _passed;
}
