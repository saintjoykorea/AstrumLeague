//
//  CSMatrix+billboard.cpp
//  CDK
//
//  Created by Kim Chan on 2016. 10. 20..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSMatrix.h"

static CSMatrix billboardRotation(const CSMatrix& world) {
    CSVector3 scale(CSMath::sqrt(world.m11 * world.m11 + world.m12 * world.m12 + world.m13 * world.m13),
                    CSMath::sqrt(world.m21 * world.m21 + world.m22 * world.m22 + world.m23 * world.m23),
                    CSMath::sqrt(world.m31 * world.m31 + world.m32 * world.m32 + world.m33 * world.m33));
    
    if (!scale.x || !scale.y || !scale.z) {
        return CSMatrix::Identity;
    }
    CSMatrix result;
    result.m11 = world.m11 / scale.x;
    result.m21 = world.m12 / scale.x;
    result.m31 = world.m13 / scale.x;
    result.m41 = 0.0f;
    result.m12 = world.m21 / scale.y;
    result.m22 = world.m22 / scale.y;
    result.m32 = world.m23 / scale.y;
    result.m42 = 0.0f;
    result.m13 = world.m31 / scale.z;
    result.m23 = world.m32 / scale.z;
    result.m33 = world.m33 / scale.z;
    result.m43 = 0.0f;
    result.m14 = 0.0f;
    result.m24 = 0.0f;
    result.m34 = 0.0f;
    result.m44 = 1.0f;
    
    return result;
}

void CSMatrix::billboard(const CSMatrix* world, CSMatrix& result) const {
    result.m11 = m11;
    result.m12 = m21;
    result.m13 = m31;
    result.m14 = 0.0f;
    result.m21 = -m12;
    result.m22 = -m22;
    result.m23 = -m32;
    result.m24 = 0.0f;
    result.m31 = -m13;
    result.m32 = -m23;
    result.m33 = -m33;
    result.m34 = 0.0f;
    result.m41 = 0.0f;
    result.m42 = 0.0f;
    result.m43 = 0.0f;
    result.m44 = 1.0f;
    if (world) {
        result *= billboardRotation(*world);
    }
}

void CSMatrix::horizontalBillboard(const CSMatrix* world, CSMatrix& result) const {
    result.m11 = m11;
    result.m12 = m21;
    result.m13 = m31;
    result.m14 = 0.0f;
    result.m21 = -m21;
    result.m22 = m11;
    result.m23 = 0.0f;
    result.m24 = 0.0f;
    result.m31 = m12;
    result.m32 = m22;
    result.m33 = m32;
    result.m34 = 0.0f;
    result.m41 = 0.0f;
    result.m42 = 0.0f;
    result.m43 = 0.0f;
    result.m44 = 1.0f;
    if (world) {
        result *= billboardRotation(*world);
    }
}

void CSMatrix::verticalBillboard(const CSMatrix* world, CSMatrix& result) const {
    result.m11 = m11;
    result.m12 = m21;
    result.m13 = m31;
    result.m14 = 0.0f;
    result.m21 = 0.0f;
    result.m22 = 0.0f;
    result.m23 = -1.0f;
    result.m24 = 0.0f;
    result.m31 = -m13;
    result.m32 = -m23;
    result.m33 = -m33;
    result.m34 = 0.0f;
    result.m41 = 0.0f;
    result.m42 = 0.0f;
    result.m43 = 0.0f;
    result.m44 = 1.0f;
    if (world) {
        result *= billboardRotation(*world);
    }
}

bool CSMatrix::stretchBillboard(const CSMatrix* world, const CSVector3& dir, float rate, CSMatrix& result) const {
    float length = dir.length();
    if (!length) {
        result = CSMatrix::Identity;
        return false;
    }
    CSVector3 ndir = dir / length;
    CSVector3 forward(m13, m23, m33);
    if (world) {
        CSVector3::transformCoordinate(forward, billboardRotation(*world), forward);
    }
    CSVector3 xaxis = CSVector3::normalize(CSVector3::cross(-forward, ndir));
    CSVector3 zaxis = CSVector3::normalize(CSVector3::cross(ndir, xaxis));
    
    result.m11 = xaxis.x;
    result.m12 = xaxis.y;
    result.m13 = xaxis.z;
    result.m14 = 0.0f;
    result.m21 = -ndir.x;
    result.m22 = -ndir.y;
    result.m23 = -ndir.z;
    result.m24 = 0.0f;
    result.m31 = zaxis.x;
    result.m32 = zaxis.y;
    result.m33 = zaxis.z;
    result.m34 = 0.0f;
    result.m41 = 0.0f;
    result.m42 = 0.0f;
    result.m43 = 0.0f;
    result.m44 = 1.0f;
    
    if (rate) {
        rate *= length;
        result.m21 *= rate;
        result.m22 *= rate;
        result.m23 *= rate;
    }
    return true;
}
