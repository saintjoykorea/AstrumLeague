//
//  FogResource.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef FogResource_h
#define FogResource_h

#include "Fog.h"

#include "Resource.h"

class FogResource : public Fog {
public:
    const byte kind;
private:
    Animation* _motion;

    FogResource(const Resource* resource, int alliance);
    ~FogResource();
public:
#ifdef GameImpl
    static FogResource* fog(const Resource* resource, int alliance);
#endif
    
    inline const ResourceData& data() const {
        return Asset::sharedAsset()->resource(kind);
    }
    const CSColor& color(bool visible, bool detecting) const override;
    
    void locate(const FPoint& point) override;
	ObjectDisplay display() const override;
	
	inline bool update(bool running) override {
		Object::update(running);
		return _located;
	}

    void draw(CSGraphics* graphics, ObjectLayer layer) override;
};

#endif /* FogResource_h */
