//
//  CSRandom.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 3..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSRandom__
#define __CDK__CSRandom__

#include "CSFixed.h"

#include "CSObject.h"

class CSRandom : public CSObject {
private:
    int64 _seed;
public:
    inline CSRandom() : _seed(rand()) {
    
    }
    inline CSRandom(int64 seed) : _seed(seed) {
    
    }
private:
    inline ~CSRandom() {
    
    }
public:
    static inline CSRandom* random() {
        return autorelease(new CSRandom());
    }
    static inline CSRandom* randomWithSeed(int64 seed) {
        return autorelease(new CSRandom(seed));
    }
    
    inline void setSeed(int64 seed) {
        _seed = seed;
    }
    inline int64 seed() const {
        return _seed;
    }
    int nextInt();
    int64 nextLong();
    int nextInt(int min, int max);
    int64 nextLong(int64 min, int64 max);
    float nextFloat();
    float nextFloat(float min, float max);
    fixed nextFixed();
    fixed nextFixed(fixed min, fixed max);
};
static inline void randSeed(int seed) {
    srand(seed);
}
static inline int randInt() {
    return rand();
}
int64 randLong();
int randInt(int min, int max);
int64 randLong(int64 min, int64 max);
float randFloat();
float randFloat(float min, float max);
fixed randFixed();
fixed randFixed(fixed min, fixed max);

#endif /* defined(__CDK__CSRandom__) */
