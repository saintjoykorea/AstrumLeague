//
//  GameAnimation.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 10. 18..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef GameAnimation_h
#define GameAnimation_h

#include "Asset.h"

#include "GameLayerIndex.h"

struct GameAnimation {
    Animation* animation;
    struct {
        GameLayerIndex layerIndex;
        CSVector2 offset;
		float width;
    } positions[2];
	float scale;
    bool visible;

    GameAnimation(const byte*& cursor);
    GameAnimation(const AnimationIndex& index, const GameLayerIndex& layer0, const CSVector2& offset0, const GameLayerIndex& layer1, const CSVector2& offset1);
    ~GameAnimation();
    
    inline GameAnimation(const GameAnimation&) {
        CSAssert(false, "invalid operation");
    }
    inline GameAnimation& operator =(const GameAnimation&) {
        CSAssert(false, "invalid operation");
        return *this;
    }
};

#endif /* GameAnimation_h */
