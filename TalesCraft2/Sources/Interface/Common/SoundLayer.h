//
//  SoundLayer.h
//  TalesCraft2
//
//  Created by ChangSun on 2016. 10. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef SoundLayer_h
#define SoundLayer_h

#include "AssetIndex.h"

class SoundLayer : public CSLayer {
private:
    SoundIndex _index;
public:
    SoundLayer(const SoundIndex& index);
protected:
    inline virtual ~SoundLayer() {}
public:
    static inline SoundLayer* layer(const SoundIndex& index) {
        return autorelease(new SoundLayer(index));
    }
protected:
    virtual void onTouchesEnded() override;
};

#endif /* SoundLayer_h */
