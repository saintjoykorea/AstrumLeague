//
//  Card.h
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 27..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef Card_h
#define Card_h

#include "Asset.h"

#include "UnitBase.h"

class ForceBase;

class Card : public CSObject, public UnitBase {
private:
    const ForceBase* _force;
public:
    Card(const ForceBase* force, const UnitIndex& index, int level, int skin);
    Card(CSBuffer* buffer, const ForceBase* force);
    Card(const CSJSONObject* json, const ForceBase* force);
protected:
    virtual ~Card() = default;
public:
    static inline Card* card(const ForceBase* force, const UnitIndex& index, int level, int skin) {
        return autorelease(new Card(force, index, level, skin));
    }
    static inline Card* cardWithBuffer(CSBuffer* buffer, const ForceBase* force) {
        return autorelease(new Card(buffer, force));
    }
    static inline Card* cardWithJSON(const CSJSONObject* json, const ForceBase* force) {
        return autorelease(new Card(json, force));
    }
public:
    inline const ForceBase* force() const override {
        return _force;
    }
    void writeTo(CSBuffer* buffer) const;

	inline bool isEnabled() const {
		return level > 0;
	}
};

#endif /* Card_h */
