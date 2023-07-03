//
//  CSCamera.cpp
//  CDK
//
//  Created by 김찬 on 2016. 1. 28..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSCamera.h"

#include "CSMath.h"

CSCamera::CSCamera() {
    _viewProjection = NULL;
    _projection = NULL;
    _view = NULL;
}
CSCamera::CSCamera(float fov, float width, float height, float znear, float zfar) {
    _var.fov = fov;
    _var.width = width;
    _var.height = height;
    _var.znear = znear;
    _var.zfar = zfar;
    
    _var.position = CSVector3(0.0f, 0.0f, defaultDistance());
    _var.target = CSVector3::Zero;
    _var.up = CSVector3(0, -1, 0);
    
    _viewProjection = NULL;
    _projection = NULL;
    _view = NULL;
}
CSCamera::CSCamera(const CSCamera& other) {
    memcpy(&_var, &other._var, sizeof(_var));
    _viewProjection = CSObject::retain(other._viewProjection);
    _projection = CSObject::retain(other._projection);
    _view = CSObject::retain(other._view);
}
CSCamera::~CSCamera() {
    CSObject::release(_viewProjection);
    CSObject::release(_projection);
    CSObject::release(_view);
}

CSCamera& CSCamera::operator =(const CSCamera& other) {
    memcpy(&_var, &other._var, sizeof(_var));
    CSObject::retain(_viewProjection, other._viewProjection);
    CSObject::retain(_projection, other._projection);
    CSObject::retain(_view, other._view);
    return *this;
}
const CSMatrix& CSCamera::projection() const {
    if (!_projection) {
        _projection = new CSValue<CSMatrix>(_var.fov ? CSMatrix::perspectiveFovLH(_var.fov, _var.width / _var.height, _var.znear, _var.zfar) : CSMatrix::orthoLH(_var.width, _var.height, _var.znear, _var.zfar));
    }
    return _projection->value();
}
const CSMatrix& CSCamera::view() const {
    if (!_view) {
        _view = new CSValue<CSMatrix>(CSMatrix::lookAtLH(_var.position, _var.target, _var.up));
    }
    return _view->value();
}
const CSMatrix& CSCamera::viewProjection() const {
    if (!_viewProjection) {
        _viewProjection = new CSValue<CSMatrix>(view() * projection());
    }
    return _viewProjection->value();
}

void CSCamera::projectionChanged() {
    CSObject::release(_viewProjection);
    CSObject::release(_projection);
}
void CSCamera::viewChanged() {
    CSObject::release(_viewProjection);
    CSObject::release(_view);
}
void CSCamera::setProjection(float fov, float width, float height, float znear, float zfar) {
    _var.fov = fov;
    _var.width = width;
    _var.height = height;
    _var.znear = znear;
    _var.zfar = zfar;
    projectionChanged();
}
void CSCamera::setView(const CSVector3& position, const CSVector3& target, const CSVector3& up) {
    _var.position = position;
    _var.target = target;
    _var.up = up;
    viewChanged();
}
void CSCamera::resetView() {
    _var.position = CSVector3(0, 0, defaultDistance());
    _var.target = CSVector3::Zero;
    _var.up = CSVector3(0, -1, 0);
    viewChanged();
}
void CSCamera::move(const CSVector3& dir) {
    _var.position += dir;
    _var.target += dir;
    viewChanged();
}
void CSCamera::rotate(const CSVector3& axis, float angle) {
    CSVector3 t = _var.target - _var.position;
    CSQuaternion rotation = CSQuaternion::rotationAxis(axis, angle);
    CSVector3::transform(t, rotation, t);
    CSVector3::transform(_var.up, rotation, _var.up);
    _var.target = t + _var.position;
    viewChanged();
}
void CSCamera::orbit(const CSVector3& axis, float angle) {
    CSVector3 p = _var.position - _var.target;
    CSQuaternion rotation = CSQuaternion::rotationAxis(axis, angle);
    CSVector3::transform(p, rotation, p);
    CSVector3::transform(_var.up, rotation, _var.up);
    _var.position = p + _var.target;
    viewChanged();
}

float CSCamera::defaultDistance() const {
    return _var.fov ? _var.height / (2.0f * CSMath::tan(_var.fov * 0.5f)) : _var.height;
}

CSRay CSCamera::pickRay(const CSVector2& screenPosition) const {
    CSVector3 nearSource(screenPosition.x, screenPosition.y, _var.znear);
    CSVector3 farSource(screenPosition.x, screenPosition.y, _var.zfar);
    
    const CSMatrix& viewProjection = this->viewProjection();
    CSVector3 nearPoint = CSVector3::unproject(nearSource, 0, 0, _var.width, _var.height, _var.znear, _var.zfar, viewProjection);
    CSVector3 farPoint = CSVector3::unproject(farSource, 0, 0, _var.width, _var.height, _var.znear, _var.zfar, viewProjection);
    CSVector3 direction = farPoint - nearPoint;
    direction.normalize();
    
    return CSRay(nearPoint, direction);
}

CSBoundingFrustum CSCamera::boundingFrustum(const CSRect& screenRect) const {
    float yScale = (float)(1.0 / CSMath::tan(_var.fov * 0.5f));
    float xScale = yScale / (_var.width / _var.height);
    
    float nearWidth = _var.znear * 2.0f / xScale;
    float nearHeight = _var.znear * 2.0f / yScale;
    
    float left = (screenRect.left() / screenRect.size.width - 0.5f) * nearWidth;
    float right = (screenRect.right() / screenRect.size.width - 0.5f) * nearWidth;
    float top = (screenRect.top() / screenRect.size.height - 0.5f) * nearHeight;
    float bottom = (screenRect.bottom() / screenRect.size.height - 0.5f) * nearHeight;
    
    CSMatrix projection;
    CSMatrix::perspectiveOffCenterLH(left, right, top, bottom, _var.znear, _var.zfar, projection);
    return CSBoundingFrustum(view() * projection);
}
