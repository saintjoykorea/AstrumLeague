//
//  Action.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Action_h
#define Action_h

#include "Unit.h"

enum ActionCode : byte {
    ActionNone,
    ActionTouch,
    ActionScroll,
    ActionSelect,
    ActionAllSelect,
    ActionFocusSelect,
    ActionCancelSelect,
    ActionDragSelect,
    ActionClickSelect,
    ActionSelectNumbering,
    ActionSetNumbering,
    ActionCancelNumbering,
    ActionSelectNavigation,
    ActionCommand,
    ActionCancelProducingCommand,
    ActionCancelBuildingCommand,
    ActionCancelTrainingCommand,
    ActionUnloadCommand,
    ActionSpawn,
    ActionPing
};

enum ActionRestriction : byte {
    ActionRestrictionSelection = 1,
    ActionRestrictionCommand = 2,
    ActionRestrictionSpawn = 4
};

class ActionTarget : public CSObject {
private:
    enum {
        ActionTargetPoint,
        ActionTargetObject
    } _type;
    
    union target {
        const CSArray<Object>* objects;
        struct {
            FPoint point;
            fixed range;
        } point;
        
        inline target() {}
    } _target;
public:
    ActionTarget(const FPoint& point, fixed range);
    ActionTarget(const CSArray<Object>* objects);
private:
    ~ActionTarget();
public:
	static inline ActionTarget* target(const FPoint& point, fixed range) {
		return autorelease(new ActionTarget(point, range));
	}
	static inline ActionTarget* target(const CSArray<Object>* objects) {
		return autorelease(new ActionTarget(objects));
	}

    bool accept(const Object* target, const FPoint& point) const;
};

struct Action {
public:
	ushort func;
	ushort ip;
private:
	ActionCode _code;
    const CSArray<Unit>* _units;
    const CommandIndex* _command;
    const ActionTarget* _target;
    bool _all;
    byte _restriction;
    bool _passed;
public:
    Action(int func, int ip, ActionCode code, const CSArray<Unit>* units, const CommandIndex* command, const ActionTarget* target, bool all, uint restriction);
    ~Action();
    
    bool pass(ActionCode code, const CSArray<Unit>* units, const CommandIndex* command, const Object* target);
    
    inline ActionCode code() const {
        return _code;
    }
    
    inline uint restriction() const {
        return _restriction;
    }
    
    inline bool passed() const {
        return _passed;
    }
    
    inline Action(const Action& other) {
        CSAssert(false, "invalid operation");
    }
    inline Action& operator =(const Action& other) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* Action_h */
