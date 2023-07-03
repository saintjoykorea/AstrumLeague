//
//  Valuations.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 4. 16..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef Valuations_h
#define Valuations_h

#include "AssetIndex.h"

class Valuations {
private:
    struct ValuationKey {
        UnitIndex index0;
        UnitIndex index1;
        ushort level0;
        ushort level1;

        ValuationKey(const UnitIndex& index0, int level0, const UnitIndex& index1, int level1);
        
        operator uint() const;
        
        bool operator ==(const ValuationKey& other) const;
    };
    CSDictionary<ValuationKey, fixed>* _valuations;
public:
    Valuations();
    ~Valuations();
    
    fixed valuation(const UnitIndex& index0, int level0, const UnitIndex& index1, int level1);
};

#endif /* Valuations_h */
