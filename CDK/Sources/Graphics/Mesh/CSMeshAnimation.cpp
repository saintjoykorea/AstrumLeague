//
//  CSMeshAnimation.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 6..
//  Copyright © 2020년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMeshAnimation.h"

#include "CSBuffer.h"

CSMeshAnimation::PositionTrack::PositionTrack(CSBuffer* buffer) : ticks(buffer->readInt()), position(buffer) {
    
}

CSMeshAnimation::RotationTrack::RotationTrack(CSBuffer* buffer) : ticks(buffer->readInt()), rotation(buffer)  {
    
}

CSMeshAnimation::ScaleTrack::ScaleTrack(CSBuffer* buffer) : ticks(buffer->readInt()), scale(buffer), scaleAxis(buffer), scaleAxisAng(buffer->readFloat()) {
    
}

CSMeshAnimation::CSMeshAnimation(CSBuffer* buffer) {
    _positions = CSObject::retain(buffer->readArray<PositionTrack>());
    _rotations = CSObject::retain(buffer->readArray<RotationTrack>());
    _scales = CSObject::retain(buffer->readArray<ScaleTrack>());
}

CSMeshAnimation::~CSMeshAnimation() {
    CSObject::release(_positions);
    CSObject::release(_rotations);
    CSObject::release(_scales);
}

CSVector3 CSMeshAnimation::position(float ticks) const {
    CSAssert(_positions, "invalid operation");
    
    for (int i = 0; i < _positions->count(); i++) {
        const PositionTrack& current = _positions->objectAtIndex(i);
        
        if (ticks <= current.ticks) {
            if (i > 0) {
                const PositionTrack& prev = _positions->objectAtIndex(i - 1);
                
                return CSVector3::lerp(prev.position, current.position, (ticks - prev.ticks) / (current.ticks - prev.ticks));
            }
            else {
                return current.position;
            }
        }
    }
    return _positions->lastObject().position;
}

CSQuaternion CSMeshAnimation::rotation(float ticks) const {
    CSAssert(_rotations, "invalid operation");
    
    for (int i = 0; i < _rotations->count(); i++) {
        const RotationTrack& current = _rotations->objectAtIndex(i);
        
        if (ticks <= current.ticks) {
            if (i > 0) {
                const RotationTrack& prev = _rotations->objectAtIndex(i - 1);
                
                return CSQuaternion::slerp(prev.rotation, current.rotation, (ticks - prev.ticks) / (current.ticks - prev.ticks));
            }
            else {
                return current.rotation;
            }
        }
    }
    return _rotations->lastObject().rotation;
}
