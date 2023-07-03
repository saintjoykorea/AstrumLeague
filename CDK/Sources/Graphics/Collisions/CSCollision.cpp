//
//  CSCollision.cpp
//  CDK
//
//  Created by ChangSun on 2016. 1. 7..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSCollision.h"

#include "CSMath.h"

#include "CSBoundingBox.h"
#include "CSBoundingSphere.h"
#include "CSVector3.h"
#include "CSPlane.h"
#include "CSRay.h"

float CSCollision::distancePlanePoint(const CSPlane& plane, const CSVector3& point) {
    float dot = CSVector3::dot(plane.normal, point);
    return dot - plane.d;
}

void CSCollision::closestPointPointTriangle(const CSVector3& point, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, CSVector3& result) {
    CSVector3 ab = vertex2 - vertex1;
    CSVector3 ac = vertex3 - vertex1;
    CSVector3 ap = point - vertex1;
    
    float d1 = CSVector3::dot(ab, ap);
    float d2 = CSVector3::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f)
        result = vertex1; //Barycentric coordinates (1,0,0)
        
    //Check if P in vertex region outside B
    CSVector3 bp = point - vertex2;
    float d3 = CSVector3::dot(ab, bp);
    float d4 = CSVector3::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3)
        result = vertex2; // Barycentric coordinates (0,1,0)
        
    //Check if P in edge region of AB, if so return projection of P onto AB
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        result = vertex1 + ab * v; //Barycentric coordinates (1-v,v,0)
    }
    
    //Check if P in vertex region outside C
    CSVector3 cp = point - vertex3;
    float d5 = CSVector3::dot(ab, cp);
    float d6 = CSVector3::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6)
        result = vertex3; //Barycentric coordinates (0,0,1)
        
    //Check if P in edge region of AC, if so return projection of P onto AC
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        result = vertex1 + ac * w; //Barycentric coordinates (1-w,0,w)
    }
    
    //Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        result = vertex2 + (vertex3 - vertex2) * w; //Barycentric coordinates (0,1-w,w)
    }
    
    //P inside face region. Compute Q through its Barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v2 = vb * denom;
    float w2 = vc * denom;
    result = vertex1 + ab * v2 + ac * w2; //= u*vertex1 + v*vertex2 + w*vertex3, u = va * denom = 1.0f - v - w
}

bool CSCollision::rayIntersectsPlane(const CSRay& ray, const CSPlane& plane, float& distance) {
    float direction;
    direction = CSVector3::dot(plane.normal, ray.direction);
    
    if (!direction) {
        distance = 0.0f;
        return false;
    }
    
    float position;
    position = CSVector3::dot(plane.normal, ray.position);
    
    distance = (-plane.d - position) / direction;
    
    if (distance < 0.0f) {
        distance = 0.0f;
        return false;
    }
    
    return true;
}

bool CSCollision::rayIntersectsPlane(const CSRay& ray, const CSPlane& plane, CSVector3& point) {
    float distance;
    if (!rayIntersectsPlane(ray, plane, distance)) {
        return false;
    }
    
    point = ray.position + (ray.direction * distance);
    return true;
}


bool CSCollision::rayIntersectsTriangle(const CSRay& ray, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, float& distance) {
    CSVector3 edge1, edge2;
    
    edge1.x = vertex2.x - vertex1.x;
    edge1.y = vertex2.y - vertex1.y;
    edge1.z = vertex2.z - vertex1.z;
    
    //edge2
    edge2.x = vertex3.x - vertex1.x;
    edge2.y = vertex3.y - vertex1.y;
    edge2.z = vertex3.z - vertex1.z;
    
    //Cross product of ray direction and edge2 - first part of determinant.
    CSVector3 directioncrossedge2;
    directioncrossedge2.x = (ray.direction.y * edge2.z) - (ray.direction.z * edge2.y);
    directioncrossedge2.y = (ray.direction.z * edge2.x) - (ray.direction.x * edge2.z);
    directioncrossedge2.z = (ray.direction.x * edge2.y) - (ray.direction.y * edge2.x);
    
    //Compute the determinant.
    float determinant;
    //Dot product of edge1 and the first part of determinant.
    determinant = (edge1.x * directioncrossedge2.x) + (edge1.y * directioncrossedge2.y) + (edge1.z * directioncrossedge2.z);
    
    //If the ray is parallel to the triangle plane, there is no collision.
    //This also means that we are not culling, the ray may hit both the
    //back and the front of the triangle.
    if (!determinant) {
        distance = 0.0f;
        return false;
    }
    
    float inversedeterminant = 1.0f / determinant;
    
    //Calculate the U parameter of the intersection point.
    CSVector3 distanceVector;
    distanceVector.x = ray.position.x - vertex1.x;
    distanceVector.y = ray.position.y - vertex1.y;
    distanceVector.z = ray.position.z - vertex1.z;
    
    float triangleU;
    triangleU = (distanceVector.x * directioncrossedge2.x) + (distanceVector.y * directioncrossedge2.y) + (distanceVector.z * directioncrossedge2.z);
    triangleU *= inversedeterminant;
    
    //Make sure it is inside the triangle.
    if (triangleU < 0.0f || triangleU > 1.0f) {
        distance = 0.0f;
        return false;
    }
    
    //Calculate the V parameter of the intersection point.
    CSVector3 distancecrossedge1;
    distancecrossedge1.x = (distanceVector.y * edge1.z) - (distanceVector.z * edge1.y);
    distancecrossedge1.y = (distanceVector.z * edge1.x) - (distanceVector.x * edge1.z);
    distancecrossedge1.z = (distanceVector.x * edge1.y) - (distanceVector.y * edge1.x);
    
    float triangleV;
    triangleV = ((ray.direction.x * distancecrossedge1.x) + (ray.direction.y * distancecrossedge1.y)) + (ray.direction.z * distancecrossedge1.z);
    triangleV *= inversedeterminant;
    
    //Make sure it is inside the triangle.
    if (triangleV < 0.0f || triangleU + triangleV > 1.0f) {
        distance = 0.0f;
        return false;
    }
    
    //Compute the distance along the ray to the triangle.
    float raydistance;
    raydistance = (edge2.x * distancecrossedge1.x) + (edge2.y * distancecrossedge1.y) + (edge2.z * distancecrossedge1.z);
    raydistance *= inversedeterminant;
    
    //Is the triangle behind the ray origin?
    if (raydistance < 0.0f) {
        distance = 0.0f;
        return false;
    }
    
    distance = raydistance;
    return true;
}


bool CSCollision::rayIntersectsTriangle(const CSRay& ray, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3, CSVector3& point) {
    float distance;
    if (!rayIntersectsTriangle(ray, vertex1, vertex2, vertex3, distance)) {
        return false;
    }
    
    point = ray.position + (ray.direction * distance);
    return true;
}

CSPlaneIntersection CSCollision::planeIntersectsPoint(const CSPlane& plane, const CSVector3& point) {
    float distance = CSVector3::dot(plane.normal, point);
    
    distance += plane.d;
    
    if (distance > 0)
        return CSPlaneIntersectionFront;
        
    if (distance < 0)
        return CSPlaneIntersectionBack;
        
    return CSPlaneIntersectionIntersecting;
}


bool CSCollision::planeIntersectsPlane(const CSPlane& plane1, const CSPlane& plane2) {
    CSVector3 direction;
    CSVector3::cross(plane1.normal, plane2.normal, direction);
    float denominator = CSVector3::dot(direction, direction);
    
    if (!denominator)
        return false;
        
    return true;
}

bool CSCollision::planeIntersectsPlane(const CSPlane& plane1, const CSPlane& plane2, CSRay& line) {
    CSVector3 direction;
    CSVector3::cross(plane1.normal, plane2.normal, direction);
    
    float denominator = CSVector3::dot(direction, direction);
    
    if (!denominator) {
        return false;
    }
    
    CSVector3 point;
    CSVector3 temp = (plane2.normal * plane1.d) - (plane1.normal * plane2.d);
    CSVector3::cross(temp, direction, point);
    
    line.position = point;
    line.direction = direction;
    line.direction.normalize();
    
    return true;
}

bool CSCollision::rayIntersectsBox(const CSRay& ray, const CSBoundingBox& box, float& distance) {
    //Source: Real-Time Collision Detection by Christer Ericson
    //Reference: Page 179
    
    distance = 0;
    float tmax = FLT_MAX;
    
    if (!ray.direction.x) {
        if (ray.position.x < box.minimum.x || ray.position.x > box.maximum.x) {
            distance = 0;
            return false;
        }
    }
    else {
        float inverse = 1.0f / ray.direction.x;
        float t1 = (box.minimum.x - ray.position.x) * inverse;
        float t2 = (box.maximum.x - ray.position.x) * inverse;
        
        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = CSMath::max(t1, distance);
        tmax = CSMath::min(t2, tmax);
        
        if (distance > tmax) {
            distance = 0;
            return false;
        }
    }
    
    if (!ray.direction.y) {
        if (ray.position.y < box.minimum.y || ray.position.y > box.maximum.y) {
            distance = 0;
            return false;
        }
    }
    else {
        float inverse = 1.0f / ray.direction.y;
        float t1 = (box.minimum.y - ray.position.y) * inverse;
        float t2 = (box.maximum.y - ray.position.y) * inverse;
        
        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = CSMath::max(t1, distance);
        tmax = CSMath::min(t2, tmax);
        
        if (distance > tmax)
        {
            distance = 0;
            return false;
        }
    }
    
    if (!ray.direction.z) {
        if (ray.position.z < box.minimum.z || ray.position.z > box.maximum.z) {
            distance = 0;
            return false;
        }
    }
    else {
        float inverse = 1.0f / ray.direction.z;
        float t1 = (box.minimum.z - ray.position.z) * inverse;
        float t2 = (box.maximum.z - ray.position.z) * inverse;
        
        if (t1 > t2) {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = CSMath::max(t1, distance);
        tmax = CSMath::min(t2, tmax);
        
        if (distance > tmax) {
            distance = 0;
            return false;
        }
    }
    
    return true;
}

bool CSCollision::rayIntersectsBox(const CSRay& ray, const CSBoundingBox& box, CSVector3& point) {
    float distance;
    if (!rayIntersectsBox(ray, box, distance)) {
        return false;
    }
    
    point = ray.position + (ray.direction * distance);
    return true;
}



bool CSCollision::rayIntersectsSphere(const CSRay& ray, const CSBoundingSphere& sphere, float& distance) {
    //Source: Real-Time Collision Detection by Christer Ericson
    //Reference: Page 177
    
    CSVector3 m = ray.position - sphere.center;
    
    float b = CSVector3::dot(m, ray.direction);
    float c = CSVector3::dot(m, m) - (sphere.radius * sphere.radius);
    
    if (c > 0 && b > 0) {
        distance = 0;
        return false;
    }
    
    float discriminant = b * b - c;
    
    if (discriminant < 0) {
        distance = 0;
        return false;
    }
    
    distance = -b - CSMath::sqrt(discriminant);
    
    if (distance < 0)
        distance = 0;
        
    return true;
}

bool CSCollision::rayIntersectsSphere(const CSRay& ray, const CSBoundingSphere& sphere, CSVector3& point) {
    float distance;
    if (!rayIntersectsSphere(ray, sphere, distance)) {
        return false;
    }
    
    point = ray.position + (ray.direction * distance);
    return true;
}

CSPlaneIntersection CSCollision::planeIntersectsBox(const CSPlane& plane, const CSBoundingBox& box) {
    CSVector3 min;
    CSVector3 max;
    
    max.x = (plane.normal.x >= 0.0f) ? box.minimum.x : box.maximum.x;
    max.y = (plane.normal.y >= 0.0f) ? box.minimum.y : box.maximum.y;
    max.z = (plane.normal.z >= 0.0f) ? box.minimum.z : box.maximum.z;
    min.x = (plane.normal.x >= 0.0f) ? box.maximum.x : box.minimum.x;
    min.y = (plane.normal.y >= 0.0f) ? box.maximum.y : box.minimum.y;
    min.z = (plane.normal.z >= 0.0f) ? box.maximum.z : box.minimum.z;
    
    float distance;
    distance = CSVector3::dot(plane.normal, max);
    
    if (distance + plane.d > 0)
        return CSPlaneIntersectionFront;
        
    distance = CSVector3::dot(plane.normal, min);
    
    if (distance + plane.d < 0)
        return CSPlaneIntersectionBack;
        
    return CSPlaneIntersectionIntersecting;
}


CSPlaneIntersection CSCollision::planeIntersectsSphere(const CSPlane& plane, const CSBoundingSphere& sphere) {
    float distance;
    distance = CSVector3::dot(plane.normal, sphere.center);
    distance += plane.d;
    
    if (distance > sphere.radius)
        return CSPlaneIntersectionFront;
        
    if (distance < -sphere.radius)
        return CSPlaneIntersectionBack;
        
    return CSPlaneIntersectionIntersecting;
}

bool CSCollision::boxIntersectsBox(const CSBoundingBox& box1, const CSBoundingBox& box2) {
    if (box1.minimum.x > box2.maximum.x || box2.minimum.x > box1.maximum.x)
        return false;
        
    if (box1.minimum.y > box2.maximum.y || box2.minimum.y > box1.maximum.y)
        return false;
        
    if (box1.minimum.z > box2.maximum.z || box2.minimum.z > box1.maximum.z)
        return false;
        
    return true;
}

bool CSCollision::boxIntersectsSphere(const CSBoundingBox& box, const CSBoundingSphere& sphere) {
    CSVector3 vector;
    CSVector3::clamp(sphere.center, box.minimum, box.maximum, vector);
    float distance = CSVector3::distanceSquared(sphere.center, vector);
    
    return distance <= sphere.radius * sphere.radius;
}

bool CSCollision::sphereIntersectsTriangle(const CSBoundingSphere& sphere, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3) {
    CSVector3 point;
    closestPointPointTriangle(sphere.center, vertex1, vertex2, vertex3, point);
    CSVector3 v = point - sphere.center;
    
    float dot;
    dot = CSVector3::dot(v, v);
    
    return dot <= sphere.radius * sphere.radius;
}

bool CSCollision::sphereIntersectsSphere(const CSBoundingSphere& sphere1, const CSBoundingSphere& sphere2)
{
    float radiisum = sphere1.radius + sphere2.radius;
    return CSVector3::distanceSquared(sphere1.center, sphere2.center) <= radiisum * radiisum;
}

CSBoundingContainment CSCollision::boxContainsPoint(const CSBoundingBox& box, const CSVector3& point) {
    if (box.minimum.x <= point.x && box.maximum.x >= point.x &&
            box.minimum.y <= point.y && box.maximum.y >= point.y &&
            box.minimum.z <= point.z && box.maximum.z >= point.z)
    {
        return CSBoundingContainmentContains;
    }
    
    return CSBoundingContainmentDisjoint;
}

CSBoundingContainment CSCollision::boxContainsBox(const CSBoundingBox& box1, const CSBoundingBox& box2) {
    if (box1.maximum.x < box2.minimum.x || box1.minimum.x > box2.maximum.x)
        return CSBoundingContainmentDisjoint;
        
    if (box1.maximum.y < box2.minimum.y || box1.minimum.y > box2.maximum.y)
        return CSBoundingContainmentDisjoint;
        
    if (box1.maximum.z < box2.minimum.z || box1.minimum.z > box2.maximum.z)
        return CSBoundingContainmentDisjoint;
        
    if (box1.minimum.x <= box2.minimum.x && (box2.maximum.x <= box1.maximum.x &&
            box1.minimum.y <= box2.minimum.y && box2.maximum.y <= box1.maximum.y) &&
            box1.minimum.z <= box2.minimum.z && box2.maximum.z <= box1.maximum.z)
    {
        return CSBoundingContainmentContains;
    }
    
    return CSBoundingContainmentIntersects;
}


CSBoundingContainment CSCollision::boxContainsSphere(const CSBoundingBox& box, const CSBoundingSphere& sphere) {
    CSVector3 vector;
    CSVector3::clamp(sphere.center, box.minimum, box.maximum, vector);
    float distance = CSVector3::distanceSquared(sphere.center, vector);
    
    if (distance > sphere.radius * sphere.radius)
        return CSBoundingContainmentDisjoint;
        
    if ((((box.minimum.x + sphere.radius <= sphere.center.x) && (sphere.center.x <= box.maximum.x - sphere.radius)) && ((box.maximum.x - box.minimum.x > sphere.radius) &&
            (box.minimum.y + sphere.radius <= sphere.center.y))) &&
            (((sphere.center.y <= box.maximum.y - sphere.radius) && (box.maximum.y - box.minimum.y > sphere.radius)) &&
             (((box.minimum.z + sphere.radius <= sphere.center.z) && (sphere.center.z <= box.maximum.z - sphere.radius)) && (box.maximum.z - box.minimum.z > sphere.radius))))
    {
        return CSBoundingContainmentContains;
    }
    
    return CSBoundingContainmentIntersects;
}

CSBoundingContainment CSCollision::sphereContainsPoint(const CSBoundingSphere& sphere, const CSVector3& point) {
    if (CSVector3::distanceSquared(point, sphere.center) <= sphere.radius * sphere.radius)
        return CSBoundingContainmentContains;
        
    return CSBoundingContainmentDisjoint;
}

CSBoundingContainment CSCollision::sphereContainsTriangle(const CSBoundingSphere& sphere, const CSVector3& vertex1, const CSVector3& vertex2, const CSVector3& vertex3) {
    CSBoundingContainment test1 = sphereContainsPoint(sphere, vertex1);
    CSBoundingContainment test2 = sphereContainsPoint(sphere, vertex2);
    CSBoundingContainment test3 = sphereContainsPoint(sphere, vertex3);
    
    if (test1 == CSBoundingContainmentContains && test2 == CSBoundingContainmentContains && test3 == CSBoundingContainmentContains)
        return CSBoundingContainmentContains;
        
    if (sphereIntersectsTriangle(sphere, vertex1, vertex2, vertex3))
        return CSBoundingContainmentIntersects;
        
    return CSBoundingContainmentDisjoint;
}

CSBoundingContainment CSCollision::sphereContainsBox(const CSBoundingSphere& sphere, const CSBoundingBox& box) {
    CSVector3 vector;
    
    if (!boxIntersectsSphere(box, sphere))
        return CSBoundingContainmentDisjoint;
        
    float radiussquared = sphere.radius * sphere.radius;
    vector.x = sphere.center.x - box.minimum.x;
    vector.y = sphere.center.y - box.maximum.y;
    vector.z = sphere.center.z - box.maximum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.maximum.x;
    vector.y = sphere.center.y - box.maximum.y;
    vector.z = sphere.center.z - box.maximum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.maximum.x;
    vector.y = sphere.center.y - box.minimum.y;
    vector.z = sphere.center.z - box.maximum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.minimum.x;
    vector.y = sphere.center.y - box.minimum.y;
    vector.z = sphere.center.z - box.maximum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.minimum.x;
    vector.y = sphere.center.y - box.maximum.y;
    vector.z = sphere.center.z - box.minimum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.maximum.x;
    vector.y = sphere.center.y - box.maximum.y;
    vector.z = sphere.center.z - box.minimum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.maximum.x;
    vector.y = sphere.center.y - box.minimum.y;
    vector.z = sphere.center.z - box.minimum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    vector.x = sphere.center.x - box.minimum.x;
    vector.y = sphere.center.y - box.minimum.y;
    vector.z = sphere.center.z - box.minimum.z;
    
    if (vector.lengthSquared() > radiussquared)
        return CSBoundingContainmentIntersects;
        
    return CSBoundingContainmentContains;
}

CSBoundingContainment CSCollision::sphereContainsSphere(const CSBoundingSphere& sphere1, const CSBoundingSphere& sphere2) {
    float distance = CSVector3::distance(sphere1.center, sphere2.center);
    
    if (sphere1.radius + sphere2.radius < distance)
        return CSBoundingContainmentDisjoint;
        
    if (sphere1.radius - sphere2.radius < distance)
        return CSBoundingContainmentIntersects;
        
    return CSBoundingContainmentContains;
}
