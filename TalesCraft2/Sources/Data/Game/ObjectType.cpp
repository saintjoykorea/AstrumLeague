//
//  ObjectType.cpp
//  TalesCraft2
//
//  Created by 김찬 on 2014. 10. 22..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#include "ObjectType.h"

const uint ObjectMaskUndergroundCollision =
	ObjectMask(ObjectTypeUndergroundObstacle) |
    ObjectMask(ObjectTypeUndergroundCreature) |
    ObjectMask(ObjectTypeUndergroundAttacker) |
    ObjectMask(ObjectTypeUndergroundLabor);

const uint ObjectMaskUnderground		= ObjectMask(ObjectTypeUnderground) | ObjectMaskUndergroundCollision;
const uint ObjectMaskMineral			= ObjectMask(ObjectTypeMineral);
const uint ObjectMaskGas				= ObjectMask(ObjectTypeGas);
const uint ObjectMaskFogMineral			= ObjectMask(ObjectTypeFogMineral);
const uint ObjectMaskFogGas				= ObjectMask(ObjectTypeFogGas);
const uint ObjectMaskFogResource		= ObjectMaskFogMineral | ObjectMaskFogGas;
const uint ObjectMaskFogBuilding		= ObjectMask(ObjectTypeFogBuilding);
const uint ObjectMaskFog				= ObjectMaskFogResource | ObjectMaskFogBuilding;
const uint ObjectMaskLabor				= ObjectMask(ObjectTypeUndergroundLabor) | ObjectMask(ObjectTypeGroundLabor) | ObjectMask(ObjectTypeAirLabor);
const uint ObjectMaskTower				= ObjectMask(ObjectTypeTower);
const uint ObjectMaskBarrack			= ObjectMask(ObjectTypeBarrack);
const uint ObjectMaskRefinery			= ObjectMask(ObjectTypeRefinery);
const uint ObjectMaskBase				= ObjectMask(ObjectTypeBase);
const uint ObjectMaskBuilding			= ObjectMask(ObjectTypeBuilding) | ObjectMaskTower | ObjectMaskBarrack | ObjectMaskRefinery | ObjectMaskBase;
const uint ObjectMaskFrame				= ObjectMask(ObjectTypeFrame);

const uint ObjectMaskAttacker			=
    ObjectMask(ObjectTypeUndergroundAttacker) |
    ObjectMask(ObjectTypeGroundAttacker) |
    ObjectMask(ObjectTypeAirAttacker);

const uint ObjectMaskWeapon				= ObjectMaskAttacker | ObjectMaskLabor;
const uint ObjectMaskUnit				= ObjectMaskBuilding | ObjectMaskWeapon;
const uint ObjectMaskResource			= ObjectMaskMineral | ObjectMaskGas;
const uint ObjectMaskLandmark			= ObjectMask(ObjectTypeBuilding) | ObjectMaskTower | ObjectMaskBarrack | ObjectMaskBase | ObjectMaskResource;

const uint ObjectMaskGroundCollision	=
	ObjectMask(ObjectTypeGroundObstacle) |
    ObjectMask(ObjectTypeGroundCreature) |
    ObjectMask(ObjectTypeGroundAttacker) |
    ObjectMask(ObjectTypeGroundLabor) |
    ObjectMaskLandmark;

const uint ObjectMaskGround				= ObjectMask(ObjectTypeGround) | ObjectMaskGroundCollision | ObjectMaskRefinery | ObjectMaskFrame | ObjectMaskFog;
const uint ObjectMaskAirCollision		=
	ObjectMask(ObjectTypeAirObstacle) |
    ObjectMask(ObjectTypeAirCreature) |
    ObjectMask(ObjectTypeAirAttacker) |
    ObjectMask(ObjectTypeAirLabor);

const uint ObjectMaskAir				= ObjectMask(ObjectTypeAir) | ObjectMaskAirCollision;

const uint ObjectMaskPath				=
    ObjectMask(ObjectTypeUndergroundCreature) |
    ObjectMask(ObjectTypeUndergroundAttacker) |
    ObjectMask(ObjectTypeUndergroundLabor) |
    ObjectMask(ObjectTypeGroundCreature) |
    ObjectMask(ObjectTypeGroundAttacker) |
    ObjectMask(ObjectTypeGroundLabor);

const uint ObjectMaskMove				= 
	ObjectMaskWeapon | 
	ObjectMask(ObjectTypeUndergroundCreature) | 
	ObjectMask(ObjectTypeGroundCreature) | 
	ObjectMask(ObjectTypeAirCreature);

const uint ObjectMaskTiled				= ObjectMaskBuilding | ObjectMaskGas;

const uint ObjectMaskWatchFirst = ObjectMaskWeapon | ObjectMaskTower;
