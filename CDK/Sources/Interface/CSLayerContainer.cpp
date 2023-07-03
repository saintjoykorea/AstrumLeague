//
//  CSLayerContainer.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 3. 5..
//  Copyright © 2019년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLayer.h"

bool CSLayerContainer::contains(const CSLayerContainer* layer) const {
    do {
        layer = layer->parent();

        if (layer == this) return true;
    } while(layer);
    
    return false;
}
