//
//  Resource.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 3..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Resource_h
#define Resource_h

#include "Object.h"

typedef AssetIndex2 ResourceIndex;

class Resource : public Object {
public:
    const uint key;
    const uint amount;
    const byte kind;
	ResourceIndex index;
private:
    fixed _remaining;
    byte _level;
    bool _occupied;
    ushort _gatherings;
    struct {
        Animation* animation;
        float remaining;
		bool relocated;
		bool reset;
    } _motion;

    Resource(int kind, const ResourceIndex& index, uint amount);
    ~Resource();
public:
#ifdef GameImpl
    static Resource* resource(int kind, const ResourceIndex& index, uint amount);
#endif
private:
    bool resetMotion();
public:
	inline int group() const {
		return index.indices[0];
	}

	Animation* captureMotion() const;

    inline const ResourceData& data() const {
        return Asset::sharedAsset()->resource(kind);
    }
    
    inline fixed remaining() const {
        return _remaining;
    }
    inline bool occupied() const {
        return _occupied;
    }
    inline void occupy(bool occupied) {
        _occupied = occupied;
    }
    bool isGatherabled(const Force* force) const;
    
    fixed gather(fixed amount);
    inline void startGathering() {
        _gatherings++;
    }
    inline void endGathering() {
        CSAssert(_gatherings, "invalid state");
        _gatherings--;
    }
    inline int gatherings() const {
        return _gatherings;
    }
    
    bool isVisible(int alliance) const;
    bool isVisible(const Force* force) const override;
    bool isDisplaying(int alliance) const;
    bool isDisplaying(const Force* force) const;
    ObjectDisplay display() const override;
    
    inline const CSColor& color(bool visible, bool detecting) const override {
        return visible ? data().color : CSColor::Transparent;
    }
    
    CSVector3 worldPoint(ObjectLayer layer) const override;
    
    void locate(const FPoint& point) override;
    void unlocate() override;
    void dispose() override;
    bool update(bool running) override;
private:
    void drawBottomCursor(CSGraphics* graphics);
public:
    void draw(CSGraphics* graphics, ObjectLayer layer) override;
};

#endif /* Resource_h */
