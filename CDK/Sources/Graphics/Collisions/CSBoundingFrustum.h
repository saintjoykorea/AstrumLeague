//
//  CSBoundingFrustum.h
//  CDK
//
//  Created by Kim Chan on 2016. 3. 17..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSBoundingFrustum__
#define __CDK__CSBoundingFrustum__

#include "CSPlane.h"
#include "CSRay.h"
#include "CSBoundingBox.h"
#include "CSBoundingSphere.h"

struct CSBoundingFrustum {
public:
    CSMatrix matrix;
    CSPlane planes[6];      //left, right, top, bottom, znear, zfar
    
    inline CSBoundingFrustum() {
    
    }
    CSBoundingFrustum(const CSMatrix& matrix);
    
    static void planesFromMatrix(const CSMatrix& matrix, CSPlane* planes);
    static CSVector3 pointInter3Planes(const CSPlane& p1, const CSPlane& p2, const CSPlane& p3);
    
    void corners(CSVector3* corners) const;
    
    CSBoundingContainment contains(const CSVector3& point) const;
    CSBoundingContainment contains(const CSBoundingBox& box) const;
    CSBoundingContainment contains(const CSBoundingSphere& sphere) const;
    bool intersects(const CSBoundingSphere& sphere) const;
    bool intersects(const CSBoundingBox& box) const;
    CSPlaneIntersection intersects(const CSPlane& plane) const;
    
    float widthAtDepth(float depth) const;
    float heightAtDepth(float depth) const;
    
    bool intersects(const CSRay& ray, float& inDistance, float& outDistance) const;
    
    float zoomToExtentsShiftDistance(CSVector3* points, uint count) const;
    float zoomToExtentsShiftDistance(const CSBoundingBox& boundingBox) const;
    CSVector3 zoomToExtentsShiftVector(CSVector3* points, uint count) const;
    CSVector3 zoomToExtentsShiftVector(const CSBoundingBox& boundingBox) const;
};



#endif /* __CDK__CSBoundingFrustum__ */
