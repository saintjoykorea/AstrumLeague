//
//  VirtualForce.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 6. 5..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef VirtualForce_h
#define VirtualForce_h

#include "ForceBase.h"

class VirtualForce : public CSObject, public ForceBase {
private:
    byte _alliance;
    fixed _resources[2];
    CSArray<Card>* _cards;
    CSArray<Rune>* _runes;
    CSDictionary<UnitIndex, ushort>* _units;
public:
    VirtualForce();
    VirtualForce(const ForceBase* other);
    VirtualForce(CSBuffer* buffer);
private:
    ~VirtualForce();
public:
    static inline VirtualForce* force() {
        return autorelease(new VirtualForce());
    }
    static inline VirtualForce* forceWithForce(const ForceBase* other) {
        return autorelease(new VirtualForce(other));
    }
    static inline VirtualForce* forceWithBuffer(CSBuffer* buffer) {
        return autorelease(new VirtualForce(buffer));
    }
    
    inline void setAlliance(int alliance) {
        _alliance = alliance;
    }
    inline void setResource(int kind, fixed amount) {
        _resources[kind] = amount;
    }
    void removeAllCards();
    void addCard(const Card* card);
    void removeCard(const UnitIndex& index);
    void removeAllRunes();
    void addRune(const Rune* rune);
    void removeRune(const RuneIndex& index);
    //====================================================
    //overide ForceBase
    inline int alliance() const override {
        return _alliance;
    }
    inline fixed resource(int kind) const override {
        return _resources[kind];
    }
    inline const CSArray<Card>* cards() const override {
        return _cards;
    }
    inline const CSArray<Rune>* runes() const override {
        return _runes;
    }
    int unitCount(const UnitIndex& index, bool alive) const override;
    //====================================================
};


#endif /* VirtualForce_h */
