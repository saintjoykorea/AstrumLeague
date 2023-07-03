//
//  FogBuilding.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef FogBuilding_h
#define FogBuilding_h

#include "Fog.h"

#include "Unit.h"

class FogBuilding : public Fog {
public:
    const Force* const force;
    const UnitIndex index;
private:
    Animation* _motion;
	float _alpha;
	byte _dir;

    FogBuilding(const Unit* unit, int alliance);
    ~FogBuilding();
public:
#ifdef GameImpl
    static FogBuilding* fog(const Unit* unit, int alliance);
#endif
    inline const UnitData& data() const {
        return Asset::sharedAsset()->unit(index);
    }
    const CSColor& color(bool visible, bool detecting) const override;
    
    void locate(const FPoint& point) override;
    void dispose() override;
	ObjectDisplay display() const override;
	bool update(bool running) override;
private:
    void spriteElement(CSGraphics* graphics, const byte* data);
public:
    void draw(CSGraphics* graphics, ObjectLayer layer) override;
};

#endif /* FogBuilding_h */
