//
//  ObjectType.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 4. 4..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef ObjectType_h
#define ObjectType_h

#include "Application.h"

enum ObjectType : byte {
	ObjectTypeUnderground,
    ObjectTypeUndergroundObstacle,
    ObjectTypeUndergroundCreature,
    ObjectTypeUndergroundAttacker,
    ObjectTypeUndergroundLabor,
	ObjectTypeGround,
    ObjectTypeGroundObstacle,
    ObjectTypeGroundCreature,
    ObjectTypeGroundAttacker,
    ObjectTypeGroundLabor,
    ObjectTypeMineral,
    ObjectTypeGas,
    ObjectTypeBuilding,
    ObjectTypeTower,
    ObjectTypeBarrack,
    ObjectTypeRefinery,
    ObjectTypeBase,
    ObjectTypeFogMineral,
    ObjectTypeFogGas,
    ObjectTypeFogBuilding,
    ObjectTypeFrame,
	ObjectTypeAir,
    ObjectTypeAirObstacle,
    ObjectTypeAirCreature,
    ObjectTypeAirAttacker,
    ObjectTypeAirLabor
};

#define ObjectMask(type)        (1 << (type))

extern const uint ObjectMaskUndergroundCollision;
extern const uint ObjectMaskUnderground;
extern const uint ObjectMaskMineral;
extern const uint ObjectMaskGas;
extern const uint ObjectMaskFogMineral;
extern const uint ObjectMaskFogGas;
extern const uint ObjectMaskFogResource;
extern const uint ObjectMaskFogBuilding;
extern const uint ObjectMaskFog;
extern const uint ObjectMaskLabor;
extern const uint ObjectMaskTower;
extern const uint ObjectMaskBarrack;
extern const uint ObjectMaskRefinery;
extern const uint ObjectMaskBase;
extern const uint ObjectMaskBuilding;
extern const uint ObjectMaskFrame;
extern const uint ObjectMaskWeapon;
extern const uint ObjectMaskAttacker;
extern const uint ObjectMaskUnit;
extern const uint ObjectMaskResource;
extern const uint ObjectMaskGroundCollision;
extern const uint ObjectMaskGround;
extern const uint ObjectMaskAirCollision;
extern const uint ObjectMaskAir;
extern const uint ObjectMaskLandmark;
extern const uint ObjectMaskPath;
extern const uint ObjectMaskMove;
extern const uint ObjectMaskTiled;
extern const uint ObjectMaskWatchFirst;

#endif /* ObjectType_h */
