//
//  CSMeshAnimation.h
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifndef CSMeshAnimation_h
#define CSMeshAnimation_h

#include "CSGraphics.h"

#include "CSArray.h"

class CSMeshAnimation {
private:
    struct PositionTrack {
        int ticks;
        CSVector3 position;
        
        PositionTrack(CSBuffer* buffer);
    };
    struct RotationTrack {
        int ticks;
        CSQuaternion rotation;
        
        RotationTrack(CSBuffer* buffer);
    };
    struct ScaleTrack {
        int ticks;
        CSVector3 scale;
        CSVector3 scaleAxis;
        float scaleAxisAng;
        
        ScaleTrack(CSBuffer* buffer);
    };
    CSArray<PositionTrack>* _positions;
    CSArray<RotationTrack>* _rotations;
    CSArray<ScaleTrack>* _scales;
public:
    CSMeshAnimation(CSBuffer* buffer);
    ~CSMeshAnimation();
    
    inline bool hasPosition() const {
        return _positions != NULL;
    }
    inline bool hasRotation() const {
        return _rotations != NULL;
    }
    inline bool hasScale() const {
        return _scales != NULL;
    }
    
    CSVector3 position(float ticks) const;
    CSQuaternion rotation(float ticks) const;
    
    //TODO:scale 미구현
};

#endif /* CSMeshAnimation_h */
