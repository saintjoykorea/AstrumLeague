//
//  IPoint.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 12..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef IPoint_h
#define IPoint_h

#include "Application.h"

struct FPoint;

struct IPoint {
    int x, y;
    
    static const IPoint Zero;
    
    inline IPoint() {
    
    }
    inline IPoint(int x, int y) : x(x), y(y) {
    
    }
    inline IPoint(const CSVector2& p) : x(p.x), y(p.y) {
    
    }
    IPoint(const FPoint& p);
    
    inline IPoint operator +(const IPoint& vector) const {
        return IPoint(x + vector.x, y + vector.y);
    }
    
    inline IPoint operator +(int scalar) const {
        return IPoint(x + scalar, y + scalar);
    }
    
    inline IPoint operator -(const IPoint& vector) const {
        return IPoint(x - vector.x, y - vector.y);
    }
    
    inline IPoint operator -(int scalar) const {
        return IPoint(x - scalar, y - scalar);
    }
    
    inline IPoint operator *(const IPoint& vector) const {
        return IPoint(x * vector.x, y * vector.y);
    }
    
    inline IPoint operator *(int scale) const {
        return IPoint(x * scale, y * scale);
    }
    
    inline IPoint operator /(const IPoint& vector) const {
        return IPoint(x / vector.x, y / vector.y);
    }
    
    inline IPoint operator /(int scale) const {
        return IPoint(x / scale, y / scale);
    }
    
    inline IPoint operator -() const {
        return IPoint(-x, -y);
    }
    
    inline IPoint& operator +=(const IPoint& vector) {
        x += vector.x;
        y += vector.y;
        return *this;
    }
    
    inline IPoint& operator +=(int scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    
    inline IPoint& operator -=(const IPoint& vector) {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }
    
    inline IPoint& operator -=(int scalar) {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    
    inline IPoint& operator *=(const IPoint& vector) {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    
    inline IPoint& operator *=(int scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    
    inline IPoint& operator /=(const IPoint& vector) {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    
    inline IPoint& operator /=(fixed scale) {
        x /= scale;
        y /= scale;
        return *this;
    }
    
    inline operator CSVector2() const {
        return CSVector2(x, y);
    }
    
    inline explicit operator uint() const {      //hash
        return (x & 0xffff << 16) | (y & 0xffff);
    }
    
    inline bool operator ==(const IPoint& other) const {
        return x == other.x && y == other.y;
    }
    inline bool operator !=(const IPoint& other) const {
        return x != other.x || y != other.y;
    }
};



#endif /* IPoint_h */
