//
//  Effect.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 14..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef Effect_h
#define Effect_h

#include "Asset.h"

#include "ObjectLayer.h"

enum EffectDisplay : byte {
    EffectDisplayInfinite,
    EffectDisplayBlink,
    EffectDisplayVanish
};

enum EffectType {
	EffectTypeLight,
	EffectTypeStroke,
	EffectTypeLine,
	EffectTypeAnimation
};

class Object;

class Effect : public CSObject {
protected:
    const Object* _dest;
private:
    int _tag;
protected:
    Effect(const Object* dest, int tag);
    virtual ~Effect();
public:
	virtual EffectType type() const = 0;

    inline const Object* destination() const {
        return _dest;
    }
    void setDestination(const Object* dest);
    
    virtual const Object* source() const = 0;
    
    inline bool isVisible() const {
        return _dest != NULL;
    }

    inline int tag() const {
        return _tag;
    }

	inline virtual bool isVisible(const CSArray<Effect>* others, int count) const {
		return true;
	}

    virtual void dispose();
    virtual void update() = 0;
    virtual void draw(CSGraphics* graphics, ObjectLayer layer, bool cover) = 0;
};

#endif /* Effect_h */
