//
//  Object.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 14..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include "Force.h"

#include "NavMesh.h"

#include "IBounds.h"
#include "Effect.h"
#include "Movement.h"

enum ObjectUIValidation {
    ObjectUIValidationDeselect,
    ObjectUIValidationMaintain,
    ObjectUIValidationSelect
};
enum ObjectVisible : byte {
    ObjectVisibleNone,
    ObjectVisibleDistortion,
    ObjectVisibleNormal,
	ObjectVisibleEnvironment
};
struct ObjectDisplay {
	ObjectVisible visible;
	byte layer;

	inline ObjectDisplay(ObjectVisible visible, int layer) : visible(visible), layer(layer) {

	}
};
class Map;

class Movement;

class Object : public PathObject {
protected:
	bool _located;
	bool _collision;
	ObjectType _type;
	float _radius;
	mutable uint _mark;
	mutable CSArray<Effect>* _effects;

protected:
    Object();
    
    virtual ~Object();
public:
    void registerEffect(Effect* effect) const;
	
	inline bool hasEffect() const {
		return _effects != NULL;
	}
    
    inline bool isLocated() const {
        return _located;
    }
    virtual CSVector3 worldPoint(ObjectLayer layer) const;
    
    FPoint facingPoint(const FPoint& point, fixed range = fixed::Zero) const;
    
    inline IBounds bounds() const {
        return IBounds(_point, collider(), true);
    }
    inline IBounds bounds(const FPoint& point) const {
        return IBounds(point, collider(), true);
    }
    
#ifdef GameImpl
    bool mark(uint mark) const;
#endif
    
    int direction(const FPoint& point) const;
    
	inline ObjectType type() const {
		return _type;
	}
	
	static inline bool compareType(ObjectType type, uint mask) {
        return (ObjectMask(type) & mask) != 0;
    }
    inline bool compareType(uint mask) const {
        return compareType(_type, mask);
    }
    
    virtual void locate(const FPoint& point);
    virtual void unlocate();
    
    inline virtual bool isVisible(const Force* force) const {
        return _located;
    }
    virtual ObjectDisplay display() const;
    
	float displayRange() const;

    inline float radius() const {
        return _radius;
    }
    inline bool collision() const {
        return _collision;
    }
    inline virtual Movement* movement() {
        return NULL;
    }
    inline virtual const CSColor& color(bool visible, bool detecting) const {
        return CSColor::Transparent;
    }
    virtual void dispose();
    virtual bool update(bool running);
    virtual void draw(CSGraphics* graphics, ObjectLayer layer) = 0;
    void drawEffect(CSGraphics* graphics, ObjectLayer layer, bool cover);

    static uint animationVisibleKey(int dir, ObjectLayer layer);
};

#endif /* Object_h */
