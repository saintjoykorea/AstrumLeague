//
//  CSCamera.h
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSCamera__
#define __CDK__CSCamera__

#include "CSValue.h"

#include "CSMatrix.h"
#include "CSRect.h"

#include "CSRay.h"
#include "CSBoundingFrustum.h"

struct CSCamera {
private:
    struct {
        CSVector3 position;
        CSVector3 target;
        CSVector3 up;
        float fov;
        float width;
        float height;
        float znear;
        float zfar;
    } _var;
    
    mutable const CSValue<CSMatrix>* _viewProjection;
    mutable const CSValue<CSMatrix>* _projection;
    mutable const CSValue<CSMatrix>* _view;
public:
    CSCamera();
    CSCamera(float fov, float width, float height, float znear, float zfar);
    CSCamera(const CSCamera& other);
    ~CSCamera();
    
    CSCamera& operator =(const CSCamera& other);
    
    const CSMatrix& projection() const;
    const CSMatrix& view() const;
    const CSMatrix& viewProjection() const;
    
private:
    void projectionChanged();
    void viewChanged();
public:
    inline float fov() const {
        return _var.fov;
    }
    inline void setFov(float fov) {
        if (_var.fov != fov) {
            _var.fov = fov;
            projectionChanged();
        }
    }
    inline float width() const {
        return _var.width;
    }
    inline void setWidth(float width) {
        if (_var.width != width) {
            _var.width = width;
            projectionChanged();
        }
    }
    inline float height() const {
        return _var.height;
    }
    inline void setHeight(float height) {
        if (_var.height != height) {
            _var.height = height;
            projectionChanged();
        }
    }
    inline float znear() const {
        return _var.znear;
    }
    inline void setZNear(float znear) {
        if (_var.znear != znear) {
            _var.znear = znear;
            projectionChanged();
        }
    }
    inline float zfar() const {
        return _var.zfar;
    }
    inline void setZFar(float zfar) {
        if (_var.zfar != zfar) {
            _var.zfar = zfar;
            projectionChanged();
        }
    }
    inline const CSVector3& position() const {
        return _var.position;
    }
    inline void setPosition(const CSVector3& position) {
        if (_var.position != position) {
            _var.position = position;
            viewChanged();
        }
    }
    inline const CSVector3& target() const {
        return _var.target;
    }
    inline void setTarget(const CSVector3& target) {
        if (_var.target != target) {
            _var.target = target;
            viewChanged();
        }
    }
    inline const CSVector3& up() const {
        return _var.up;
    }
    inline void setUp(const CSVector3& up) {
        if (_var.up != up) {
            _var.up = up;
            viewChanged();
        }
    }
    inline CSVector3 forward() const {
        const CSMatrix& view = this->view();
        
        return CSVector3(view.m13, view.m23, view.m33);
    }
    inline CSVector3 right() const {
        const CSMatrix& view = this->view();
        
        return CSVector3(view.m11, view.m21, view.m31);
    }
    
    void setProjection(float fov, float width, float height, float znear, float zfar);
    void setView(const CSVector3& position, const CSVector3& target, const CSVector3& up);
    void resetView();
    void move(const CSVector3& dir);
    void rotate(const CSVector3& axis, float angle);
    void orbit(const CSVector3& axis, float angle);
    
    float defaultDistance() const;
    
    CSRay pickRay(const CSVector2& screenPosition) const;
    inline CSBoundingFrustum boundingFrustum() const {
        return CSBoundingFrustum(viewProjection());
    }
    CSBoundingFrustum boundingFrustum(const CSRect& screenRect) const;
    
    inline bool operator ==(const CSCamera& other) const {
        return memcmp(&_var, &other._var, sizeof(_var)) == 0;
    }
    inline bool operator !=(const CSCamera& other) const {
        return !(*this == other);
    }
};


#endif /* __CDK__CSCamera__ */
