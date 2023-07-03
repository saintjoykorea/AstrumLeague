//
//  ObjectLayer.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 4. 24..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef ObjectLayer_h
#define ObjectLayer_h

#include "Application.h"

enum ObjectLayer : byte {
	ObjectLayerReady,
    ObjectLayerShadow,
    ObjectLayerBottomCursor,
    ObjectLayerBottom,
    ObjectLayerMiddle,
    ObjectLayerTop,
    ObjectLayerTopCursor,
    ObjectLayerEnvironment
};

#endif /* ObjectLayer_h */
