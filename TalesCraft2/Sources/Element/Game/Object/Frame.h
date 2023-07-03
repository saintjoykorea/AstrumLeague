//
//  Frame.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 2..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Frame_h
#define Frame_h

#include "Object.h"

#include "Force.h"

#include "CommandReturn.h"

class Frame : public Object {
public:
    const Force* force;
    const UnitIndex index;
    CommandBuildTarget target;
private:
    struct {
        Animation* animation;
        float remaining;
		bool relocated;
    }_motion;
public:
    Frame(const Force* force, const UnitIndex& index, CommandBuildTarget target, bool collision);
private:
    ~Frame();
public:
    static Frame* frame(const Force* force, const UnitIndex& index, CommandBuildTarget target, bool collision);
    
    CSVector3 worldPoint(ObjectLayer layer) const override;
    
    void locate(const FPoint& point) override;
    
    inline const UnitData& data() const {
        return Asset::sharedAsset()->unit(index);
    }
    inline bool isVisible(const Force* force) const override {
        return _located && (!force || force == this->force);
    }
    ObjectDisplay display() const override;
    
    inline void setCollision(bool collision) {
        _collision = collision;
    }
    
    bool update(bool running) override;
    void draw(CSGraphics* graphics, ObjectLayer layer) override;
};

#endif /* Frame_h */
