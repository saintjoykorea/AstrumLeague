//
//  Navigation.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 5. 29..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef Navigation_h
#define Navigation_h

#include "Unit.h"

class Navigation : public CSObject {
public:
    const UnitIndex index;
private:
    CSArray<Unit>* _preparedUnits;
    CSArray<Unit>* _registeredUnits;
    
public:
    Navigation(const UnitIndex& index);
private:
    ~Navigation();
public:
    inline CSArray<Unit>* preparedUnits() {
        return _preparedUnits;
    }
    inline CSArray<Unit>* registeredUnits() {
        return _registeredUnits;
    }
    inline const CSArray<Unit>* preparedUnits() const {
        return _preparedUnits;
    }
    inline const CSArray<Unit>* registeredUnits() const {
        return _registeredUnits;
    }
};

#endif /* Navigation_h */
