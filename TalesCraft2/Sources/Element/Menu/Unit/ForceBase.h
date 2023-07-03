//
//  ForceBase.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 3. 27..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef ForceBase_h
#define ForceBase_h

#include "UnitBase.h"

#include "Card.h"
#include "Rune.h"

class ForceBase {
protected:
    inline ForceBase() {
    
    }
public:
    inline virtual ~ForceBase() {
        
    }
    inline virtual int alliance() const {
        return 0;
    }
    inline virtual fixed resource(int kind) const {
        return fixed::Zero;
    }
    inline virtual const FPoint& reachablePoint() const {
        return FPoint::Zero;
    }
    
    virtual const CSArray<Card>* cards() const = 0;
    virtual const CSArray<Rune>* runes() const = 0;

    const Card* cardForIndex(const UnitIndex& index) const;
    const Rune* runeForIndex(const RuneIndex& index) const;

    virtual int unitCount(const UnitIndex& index, bool alive) const;
    
    inline virtual bool isTrained(const RuneIndex& index) const {
        return false;		//TODO:아웃게임에서 룬효과를 보고 싶을 경우 변경 필요
    }
};

#endif /* ForceBase_h */
