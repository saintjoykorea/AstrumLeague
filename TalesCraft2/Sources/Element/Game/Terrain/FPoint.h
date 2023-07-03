//
//  FPoint.h
//  TalesCraft2
//
//  Created by 김찬 on 2016. 2. 9..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef FPoint_h
#define FPoint_h

#include "Application.h"

struct IPoint;

struct FPoint {
    fixed x, y;
    
    mutable uint slabId;
public:
    static const FPoint Zero;
    
    inline FPoint() : slabId(0) {
    
    }
	inline FPoint(fixed x, fixed y) : x(x), y(y), slabId(0) {

	}
    inline FPoint(float x, float y) : x(x), y(y), slabId(0) {
    
    }
    inline FPoint(const CSVector2& p) : x(p.x), y(p.y), slabId(0) {
    
    }
    inline FPoint(CSBuffer* buffer) : x(buffer->readFixed()), y(buffer->readFixed()), slabId(0) {
        
    }
	inline FPoint(const byte*& raw) : x(readFixed(raw)), y(readFixed(raw)), slabId(0) {

	}
    FPoint(const IPoint& p);

    inline fixed lengthSquared() const {
        return (x * x) + (y * y);
    }
    
    inline fixed length() const {
        return CSMath::sqrt(lengthSquared());
    }
    
    static void normalize(const FPoint& vector, FPoint& result);
    static inline FPoint normalize(const FPoint& vector) {
        FPoint result;
        normalize(vector, result);
        return result;
    }
    void normalize();
    
    static inline fixed distanceSquared(const FPoint& left, const FPoint& right) {
        fixed x = left.x - right.x;
        fixed y = left.y - right.y;
        return (x * x) + (y * y);
    }
    static inline fixed distance(const FPoint& left, const FPoint& right) {
        return CSMath::sqrt(distanceSquared(left, right));
    }
    inline fixed distanceSquared(const FPoint& other) const {
        return distanceSquared(*this, other);
    }
    inline fixed distance(const FPoint& other) const {
        return distance(*this, other);
    }
    
    fixed distanceSquaredToLine(const FPoint& p0, const FPoint& p1, FPoint* np = NULL) const;
    inline fixed distanceToLine(const FPoint& p0, const FPoint& p1, FPoint* np = NULL) const {
        return CSMath::sqrt(distanceSquaredToLine(p0, p1, np));
    }
    static bool intersectsLineToLine(const FPoint& p0, const FPoint& p1, const FPoint& p2, const FPoint& p3, FPoint* cp = NULL);
    
    bool between(const FPoint& cp, const FPoint& p0, const FPoint& p1) const;
    
    static inline void lerp(const FPoint& start, const FPoint& end, fixed amount, FPoint& result) {
        result.x = CSMath::lerp(start.x, end.x, amount);
        result.y = CSMath::lerp(start.y, end.y, amount);
    }
    static inline FPoint lerp(const FPoint& start, const FPoint& end, fixed amount) {
        FPoint result;
        lerp(start, end, amount, result);
        return result;
    }
    
    void rotate(fixed a);
    
    inline FPoint operator +(const FPoint& vector) const {
        return FPoint(x + vector.x, y + vector.y);
    }
    
	template<typename T>
    inline FPoint operator +(T scalar) const {
        return FPoint(x + scalar, y + scalar);
    }
    
    inline FPoint operator -(const FPoint& vector) const {
        return FPoint(x - vector.x, y - vector.y);
    }
    
	template<typename T>
    inline FPoint operator -(T scalar) const {
        return FPoint(x - scalar, y - scalar);
    }
    
    inline FPoint operator *(const FPoint& vector) const {
        return FPoint(x * vector.x, y * vector.y);
    }
    
	template<typename T>
    inline FPoint operator *(T scale) const {
        return FPoint(x * scale, y * scale);
    }
    
    inline FPoint operator /(const FPoint& vector) const {
        return FPoint(x / vector.x, y / vector.y);
    }
    
	template<typename T>
    inline FPoint operator /(T scale) const {
        return FPoint(x / scale, y / scale);
    }
    
    inline FPoint operator -() const {
        return FPoint(-x, -y);
    }
    
    inline FPoint& operator +=(const FPoint& vector) {
        x += vector.x;
        y += vector.y;
        return *this;
    }
    
	template<typename T>
    inline FPoint& operator +=(T scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    
    inline FPoint& operator -=(const FPoint& vector) {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }
    
	template<typename T>
    inline FPoint& operator -=(T scalar) {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    
    inline FPoint& operator *=(const FPoint& vector) {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    
	template<typename T>
    inline FPoint& operator *=(T scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    
    inline FPoint& operator /=(const FPoint& vector) {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    
	template<typename T>
    inline FPoint& operator /=(T scale) {
        x /= scale;
        y /= scale;
        return *this;
    }
    
    inline FPoint& operator =(const FPoint& other) {
        x = other.x;
        y = other.y;
        if (other.slabId) {
            slabId = other.slabId;
        }
        return *this;
    }
    
    inline operator CSVector2() const {
        return CSVector2(x, y);
    }
    
    inline explicit operator uint() const {      //hash
        return ((int)x & 0xffff << 16) | ((int)y & 0xffff);
    }
    
    inline bool operator ==(const FPoint& other) const {
        return x == other.x && y == other.y;
    }
    inline bool operator !=(const FPoint& other) const {
        return x != other.x || y != other.y;
    }
};

#endif /* FPoint_h */
