//
//  Camera.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 9..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "Camera.h"

#include "Map.h"

#define ScrollDistanceFollowSpeed       150.0f
#define ScrollMoveDecreaseRate          0.6f
#define ScrollStopDecreaseRate          0.9f
#define ZoomBounceBackRate              0.1f
#define ZoomBounceBackRestoreRate       0.2f
#define ZoomBounceBackRestoreEpsilon    0.01f
#define ZoomStartDuration				2.0f

Camera::Camera(const Terrain* terrain, CSBuffer* buffer, const CSVector2& wp, bool inv) :
    _terrain(terrain),
    _fov(buffer->readFloat()),
    _znear(buffer->readFloat()),
    _zfar(buffer->readFloat()),
    _angle(buffer->readFloat()),
    _distance(buffer->readFloat()),
    _maxZoom(buffer->readFloat()),
    _padding(buffer->readByte()),
    _inverted(inv),
    _camera(_fov, ProjectionWidth, ProjectionHeight, _znear, _zfar),
    _zoom(1)
{
    if (inv) {
        _forward.y = CSMath::sin(_angle);
        _forward.z = -CSMath::cos(_angle);
        _up.y = -_forward.z;
        _up.z = _forward.y;
    }
    else {
        _forward.y = -CSMath::sin(_angle);
        _forward.z = -CSMath::cos(_angle);
        _up.y = _forward.z;
        _up.z = -_forward.y;
    }

	_target = convertToGroundPoint(wp);

	applyImpl(_target);

	setZoom(_maxZoom, ZoomStartDuration);

	IPoint p;
    _pickRange = 0.0f;
    for (p.y = 0; p.y <= _terrain->height(); p.y++) {
        for (p.x = 0; p.x <= _terrain->width(); p.x++) {
            float a = _terrain->altitude(p);
            if (_pickRange < a) {
                _pickRange = a;
            }
        }
    }
    _pickRange /= CSMath::cos(_angle);
}

Camera::~Camera() {
    
}

CSVector3 Camera::convertToGroundPoint(const CSVector2& wp) {
	return CSVector3(wp.x, wp.y, (float)_terrain->altitude(FPoint(wp.x / TileSize, wp.y / TileSize)) * TileSize);
}

void Camera::applyImpl(const CSVector3 &target) {
    _camera.setView(target - _forward * _distance / _zoom, target, _up);
}

void Camera::apply() {
    applyImpl(_target);

    static const CSVector2 corners[4] = {
        CSVector2::Zero,
        CSVector2(ProjectionWidth, 0),
        CSVector2(0, ProjectionHeight),
        CSVector2(ProjectionWidth, ProjectionHeight)
    };
    
    float px = 0, py = 0;
    
    for (int i = 0; i < 4; i++) {
        CSRay pickRay = _camera.pickRay(corners[i]);
        
        CSVector3 point;
        if (pickRay.intersects(CSPlane::Ground, point)) {
            float p;
            
            if (point.x < 0) {
                p = -point.x;
                if (px < p) {
                    px = p;
                }
            }
            else if (px <= 0) {
                float max = _terrain->width() * TileSize;
                if (point.x > max) {
                    p = max - point.x;
                    if (px > p) {
                        px = p;
                    }
                }
            }
            if (point.y < 0) {
                p = -point.y;
                if (py < p) {
                    py = p;
                }
            }
            else if (py <= 0) {
                float max = (_terrain->height() - _padding) * TileSize;		//카메라가 회전하지 않는다고 가정하고 하단 부분만 패딩 적용
                if (point.y > max) {
                    p = max - point.y;
                    if (py > p) {
                        py = p;
                    }
                }
            }
        }
    }
    if (px || py) {
        applyImpl(CSVector3(_target.x + px, _target.y + py, _target.z));
    }
    _updated = true;
}

void Camera::update(const CSVector2& scroll, float zoom, float altitude) {
    bool updated = false;
    if (scroll.x) {
        _target.x = CSMath::clamp(_target.x + scroll.x, 0.0f, (float)_terrain->width() * TileSize);
        updated = true;
    }
    if (scroll.y) {
		_target.y = CSMath::clamp(_target.y + scroll.y, 0.0f, (float)_terrain->height() * TileSize);
        updated = true;
    }
    if (zoom) {
        _zoom += zoom;
        updated = true;
    }
    else if (_zoom < 1.0f) {
        _zoom = CSMath::min(_zoom + CSMath::max((1.0f - _zoom) * ZoomBounceBackRestoreRate, ZoomBounceBackRestoreEpsilon), 1.0f);
        updated = true;
    }
    else if (_zoom > _maxZoom) {
        _zoom = CSMath::max(_zoom - CSMath::max((_zoom - _maxZoom) * ZoomBounceBackRestoreRate, ZoomBounceBackRestoreEpsilon), _maxZoom);
        updated = true;
    }
    if (altitude) {
        float z = _terrain->altitude(FPoint(_target.x / TileSize, _target.y / TileSize)) * TileSize;
        
        if (z != _target.z) {
            if (z > _target.z + altitude) {
                _target.z += altitude;
            }
            else if (z < _target.z - altitude) {
                _target.z -= altitude;
            }
            else {
                _target.z = z;
            }
            updated = true;
        }
    }
    if (updated) {
        apply();
    }
}

void Camera::updateMoveScroll(float delta) {
    update(CSVector2::Zero, 0.0f, ScrollDistanceFollowSpeed * delta);
	_delta.pos *= ScrollMoveDecreaseRate;
}

void Camera::updateStopScroll(float delta) {
    update(_delta.pos, 0.0f, ScrollDistanceFollowSpeed * delta);
    
    if ((int)_delta.pos.x) {
        _delta.pos.x *= ScrollStopDecreaseRate;
    }
    else {
        _delta.pos.x = 0.0f;
    }
    if ((int)_delta.pos.y) {
        _delta.pos.y *= ScrollStopDecreaseRate;
    }
    else {
        _delta.pos.y = 0.0f;
    }
    if (_delta.pos == CSVector2::Zero) {
        _state = StateNone;
    }
}

void Camera::updateAuto(float delta) {
    float pt, zt;
	bool flag = false;
    if (_delta.posTime > delta) {
        pt = delta;
		_delta.posTime -= delta;
		flag = true;
    }
    else {
        pt = _delta.posTime;
		_delta.posTime = 0.0f;
    }
	if (_delta.zoomTime > delta) {
		zt = delta;
		_delta.zoomTime -= delta;
		flag = true;
	}
	else {
		zt = _delta.zoomTime;
		_delta.zoomTime = 0.0f;
	}
	update(_delta.pos * pt, _delta.zoom * zt, ScrollDistanceFollowSpeed * delta);

	if (!flag) _state = StateNone;
}

bool Camera::update(float delta) {
    synchronized(this, GameLockMap) {
        switch (_state) {
            case StateNone:
                update(CSVector2::Zero, 0.0f, ScrollDistanceFollowSpeed * delta);
                break;
            case StateMoveScroll:
                updateMoveScroll(delta);
                break;
            case StateStopScroll:
                updateStopScroll(delta);
                break;
            case StateAuto:
                updateAuto(delta);
                break;
        }
    }
    if (_updated) {
        _updated = false;
        return true;
    }
    return false;
}

void Camera::setScroll(const CSVector2& pos) {
    synchronized(this, GameLockMap) {
        switch (_state) {
            case StateNone:
            case StateMoveScroll:
            case StateStopScroll:
            case StateMoveZoom:
				_target = convertToGroundPoint(pos);
                apply();
                _state = StateNone;
                break;
        }
    }
}

void Camera::setScroll(const CSVector2& pos, float time) {
	synchronized(this, GameLockMap) {
		if (time > 0) {
			_target = _camera.target();
			_delta.posTime = time;
			_delta.pos = (pos - (CSVector2)_target) / time;
			_state = StateAuto;
		}
		else {
			_target = convertToGroundPoint(pos);
			_delta.posTime = 0;
			apply();
			if (_state != StateAuto || !_delta.zoomTime) _state = StateNone;
		}
	}
}

void Camera::moveScroll(CSVector2 diff) {
    synchronized(this, GameLockMap) {
        switch (_state) {
            case StateNone:
                _delta.pos = CSVector2::Zero;
            case StateStopScroll:
                _state = StateMoveScroll;
            case StateMoveScroll:
                if (_inverted) diff = -diff;

				_target = _camera.target() + diff;
				_delta.pos *= ScrollMoveDecreaseRate;
                _delta.pos += diff;
                apply();
				break;
        }
    }
}

void Camera::stopScroll(bool interrupt) {
    synchronized(this, GameLockMap) {
        if (_state == StateMoveScroll) {
            _state = interrupt ? StateNone : StateStopScroll;
        }
    }
}

void Camera::setZoom(float zoom) {
    synchronized(this, GameLockMap) {
        switch (_state) {
            case StateNone:
            case StateMoveScroll:
            case StateStopScroll:
            case StateMoveZoom:
                _zoom = zoom;
                apply();
                _state = StateNone;
                break;
        }
    }
}

void Camera::setZoom(float zoom, float time) {
	synchronized(this, GameLockMap) {
		if (time > 0) {
			_delta.zoomTime = time;
			_delta.zoom = (zoom - _zoom) / time;
			_state = StateAuto;
		}
		else {
			_delta.zoomTime = 0;
			_zoom = zoom;
			apply();
			if (_state != StateAuto || !_delta.posTime) _state = StateNone;
		}
	}
}

void Camera::moveZoom(float diff) {
    synchronized(this, GameLockMap) {
        switch (_state) {
            case StateNone:
            case StateMoveScroll:
            case StateStopScroll:
                _state = StateMoveZoom;
            case StateMoveZoom:
                {
                    float next = _zoom + diff;
                    
                    if (next < 1.0f && diff < 0.0f) {
                        float minZoom = 1.0f - ZoomBounceBackRate;
                        
                        _zoom = _zoom < minZoom ? minZoom : _zoom + diff * (_zoom - minZoom) / ZoomBounceBackRate;
                    }
                    else if (next > _maxZoom && diff > 0.0f) {
                        float maxZoom = _maxZoom + _maxZoom * ZoomBounceBackRate;
                        
                        _zoom = _zoom > maxZoom ? maxZoom : _zoom + diff * (maxZoom - _zoom) / (_maxZoom * ZoomBounceBackRate);
                    }
                    else {
                        _zoom = next;
                    }
                    apply();
                }
                break;
        }
    }
}

void Camera::stopZoom() {
    synchronized(this, GameLockMap) {
        if (_state == StateMoveZoom) {
            _state = StateNone;
        }
    }
}

const CSCamera& Camera::capture() const {
    assertOnLocked();
    _camera.viewProjection();
    return _camera;
}

void Camera::capture(CSCamera& capture) const {
    synchronized(this, GameLockMap) {
        _camera.viewProjection();
        capture = _camera;
    }
}


