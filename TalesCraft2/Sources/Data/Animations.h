//
//  Animations.hpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 10. 13..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef Animations_h
#define Animations_h

#include "Animation.h"

#include "User.h"

class Animations {
private:
    typedef AssetIndex4 AnimationEntryIndex;
    CSDictionary<AnimationEntryIndex, Animation>* _entries;
public:
    Animations();
    ~Animations();
private:
    inline bool contains(const AnimationEntryIndex& index) const {
        return _entries->containsKey(index);
    }

    Animation* get(const AnimationEntryIndex& index);

    inline void remove(const AnimationEntryIndex& index) {
        _entries->removeObject(index);
    }
public:
    inline void clear() {
        _entries->removeAllObjects();
    }
    inline bool contains(const AnimationGroupIndex& group, int index, int seq = 0) const {
        return contains(AnimationEntryIndex(group, index, seq));
    }
    inline bool contains(const AnimationIndex& index, int seq = 0) const {
        return contains(AnimationEntryIndex(index, seq));
    }
    inline Animation* get(const AnimationGroupIndex& group, int index, int seq = 0) {
        return get(AnimationEntryIndex(group, index, seq));
    }
    inline Animation* get(const AnimationIndex& index, int seq = 0) {
        return get(AnimationEntryIndex(index, seq));
    }
    inline void remove(const AnimationGroupIndex& group, int index, int seq = 0) {
        remove(AnimationEntryIndex(group, index, seq));
    }
    inline void remove(const AnimationIndex& index, int seq = 0) {
        remove(AnimationEntryIndex(index, seq));
    }
    bool update(float delta, const CSCamera* camera = NULL);
};

#endif /* Animations_h */
