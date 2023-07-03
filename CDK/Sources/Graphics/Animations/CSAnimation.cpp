//
//  CSAnimation.cpp
//  CDK
//
//  Created by 김찬 on 13. 11. 19..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSAnimation.h"

#include "CSBuffer.h"

#include "CSRandom.h"

#include "CSLocaleString.h"

CSAnimation::CSAnimation(const CSAnimation* other, bool capture) {
    _key = other->_key;
    _subkey = other->_subkey;
    _weight = other->_weight;
    _reverse = other->_reverse;
    _facing = other->_facing;
    _globals[0] = other->_globals[0];
    _globals[1] = other->_globals[1];
    _radial = retain(other->_radial);
    _tangential = retain(other->_tangential);
    _tangentialAngle = other->_tangentialAngle;
    _tangentialAngleVar = other->_tangentialAngleVar;
    _x = retain(other->_x);
    _y = retain(other->_y);
    _z = retain(other->_z);
    _billboardOffset = other->_billboardOffset;
    _positionUsingSpeed = other->_positionUsingSpeed;
    _positionDegree = other->_positionDegree;
    _positionLoop = other->_positionLoop;
    _rotationX = retain(other->_rotationX);
    _rotationY = retain(other->_rotationY);
    _rotationZ = retain(other->_rotationZ);
    _rotationDuration = other->_rotationDuration;
    _rotationLoop = other->_rotationLoop;
    _rotationFixedComponent = other->_rotationFixedComponent;
    _scaleX = retain(other->_scaleX);
    _scaleY = retain(other->_scaleY);
    _scaleZ = retain(other->_scaleZ);
    _scaleDuration = other->_scaleDuration;
    _scaleLoop = other->_scaleLoop;
    _scaleFixedComponent = other->_scaleFixedComponent;
    _rotationFirst = other->_rotationFirst;
    _usingLight = other->_usingLight;
    _usingDepth = other->_usingDepth;
    _depthMode = other->_depthMode;
    _depthBias = other->_depthBias;
    _usingStencil = other->_usingStencil;
    _usingAlphaTest = other->_usingAlphaTest;
    _alphaTestDuration = other->_alphaTestDuration;
    _alphaTest = retain(other->_alphaTest);
    _alphaTestDuration = other->_alphaTestDuration;
    _alphaTestLoop = other->_alphaTestLoop;
    _duration = other->_duration;
    _latency = other->_latency;
    _derivationFinish = other->_derivationFinish;
    _closing = other->_closing;
    
    _soundIndices = retain(other->_soundIndices);
    _soundDelegate = retain(other->_soundDelegate);
    _soundVolume = other->_soundVolume;
    _soundLoop = other->_soundLoop;
    _soundLatency = other->_soundLatency;
    _soundEssential = other->_soundEssential;
    _soundEnabled = other->_soundEnabled;
    
    _localeVisible = other->_localeVisible;
    _locales = other->_locales ? new CSSet<CSString>(other->_locales) : NULL;

    if (other->_substance) {
        _substance = other->_substance->copy(capture);
        
        _substance->attach(this);
    }
    if (other->_derivation) {
        _derivation = other->_derivation->copy(capture);
        
        _derivation->attach(this);
    }
    if (capture) {
        _clientPositions[0] = other->_clientPositions[0];
        _clientPositions[1] = other->_clientPositions[1];
        _clientScale = other->_clientScale;
        _clientRotation = other->_clientRotation;

        if (other->_facingCapture) {
            _facingCapture = new CSMatrix(*other->_facingCapture);
        }
        if (other->_facingRotation) {
            _facingRotation = new CSQuaternion(*other->_facingRotation);
        }
        _positionRandom = other->_positionRandom;
        _rotationRandom = other->_rotationRandom;
        _scaleRandom = other->_scaleRandom;
        _alphaTestRandom = other->_alphaTestRandom;
        _derivationRandom = other->_derivationRandom;
        _soundCounter = other->_soundCounter;
        _progress = other->_progress;
        _visible = other->_visible;
    }
    else {
        _clientScale = 1.0f;
        _clientRotation = CSQuaternion::Identity;
        
        _positionRandom = randLong();
        _rotationRandom = randLong();
        _scaleRandom = randLong();
        _alphaTestRandom = randLong();
        _derivationRandom = randLong();
        _progress = -_latency;
        _visible = !_closing && !_latency && checkLocale();
    }
}

CSAnimation::CSAnimation(CSBuffer* buffer) {
    _key = buffer->readInt();
    _weight = buffer->readByte();
    _reverse = buffer->readBoolean();
    _facing = buffer->readBoolean();
    _globals[0] = buffer->readBoolean();
    _globals[1] = buffer->readBoolean();
    _radial = CSMovement::createWithBuffer(buffer);
    _tangential = CSMovement::createWithBuffer(buffer);
    if (_tangential) {
        _tangentialAngle = buffer->readFloat();
        _tangentialAngleVar = buffer->readFloat();
    }
    _x = CSMovement::createWithBuffer(buffer);
    _y = CSMovement::createWithBuffer(buffer);
    _z = CSMovement::createWithBuffer(buffer);
    _positionDegree = buffer->readFloat();
    _positionUsingSpeed = buffer->readBoolean();
    new (&_positionLoop) CSAnimationLoop(buffer);
    _billboardOffset = buffer->readBoolean();
    
    _rotationX = CSMovement::createWithBuffer(buffer);
    _rotationY = CSMovement::createWithBuffer(buffer);
    _rotationZ = CSMovement::createWithBuffer(buffer);
    _rotationDuration = buffer->readFloat();
    new (&_rotationLoop) CSAnimationLoop(buffer);
    _rotationFixedComponent = buffer->readBoolean();
    
    _scaleX = CSMovement::createWithBuffer(buffer);
    _scaleY = CSMovement::createWithBuffer(buffer);
    _scaleZ = CSMovement::createWithBuffer(buffer);
    _scaleDuration = buffer->readFloat();
    new (&_scaleLoop) CSAnimationLoop(buffer);
    _scaleFixedComponent = buffer->readBoolean();
    _rotationFirst = buffer->readBoolean();
    
    _usingLight = buffer->readBoolean();
    if ((_usingDepth = buffer->readBoolean())) {
        _depthMode = (CSDepthMode)buffer->readByte();
        _depthBias = buffer->readFloat();
    }
    _usingStencil = buffer->readBoolean();
    
    if ((_usingAlphaTest = buffer->readBoolean())) {
        _alphaTest = CSMovement::createWithBuffer(buffer);
        _alphaTestDuration = buffer->readFloat();
        new (&_alphaTestLoop) CSAnimationLoop(buffer);
    }
    _duration = buffer->readFloat();
    _latency = buffer->readFloat();
    _derivationFinish = buffer->readBoolean();
    _closing = buffer->readBoolean();
    
    if (buffer->readBoolean()) {
        _soundIndices = retain(buffer->readArray<ushort>());
        _soundVolume = buffer->readFloat();
        _soundLoop = buffer->readFloat();
        _soundLatency = buffer->readFloat();
        _soundEssential = buffer->readBoolean();
    }
    else {
        _soundVolume = 1.0f;
    }
    _soundEnabled = true;
    
    _localeVisible = buffer->readBoolean();
    int localeCount = buffer->readLength();
    if (localeCount) {
        _locales = new CSSet<CSString>();
        for (int i = 0; i < localeCount; i++) {
            _locales->addObject(buffer->readString());
        }
    }

    _substance = CSAnimationSubstance::createWithBuffer(buffer);
    if (_substance) {
        _substance->attach(this);
    }
    _derivation = CSAnimationDerivation::createWithBuffer(buffer);
    if (_derivation) {
        _derivation->attach(this);
        
        foreach(const CSAnimation*, child, _derivation->animations()) {
            _subkey |= child->_key | child->_subkey;
        }
    }
    _clientScale = 1.0f;
    _clientRotation = CSQuaternion::Identity;
    
    _positionRandom = randLong();
    _rotationRandom = randLong();
    _scaleRandom = randLong();
    _alphaTestRandom = randLong();
    _derivationRandom = randLong();
    _progress = -_latency;
    _visible = !_closing && !_latency && checkLocale();
}
CSAnimation::~CSAnimation() {
    release(_radial);
    release(_tangential);
    release(_x);
    release(_y);
    release(_z);
    release(_rotationX);
    release(_rotationY);
    release(_rotationZ);
    release(_scaleX);
    release(_scaleY);
    release(_scaleZ);
    release(_alphaTest);
    release(_soundIndices);
    release(_soundDelegate);
    release(_locales);
    
    if (_substance) {
        _substance->attach(NULL);
        _substance->release();
    }
    if (_derivation) {
        _derivation->attach(NULL);
        _derivation->release();
    }
    if (_transform) {
        delete _transform;
    }
    if (_facingRotation) {
        delete _facingRotation;
    }
    if (_facingCapture) {
        delete _facingCapture;
    }
}

void CSAnimation::attach(CSAnimation *parent) {
    CSAssert(!parent || !_parent, "invalid operation");
    _parent = parent;
}

bool CSAnimation::checkLocale() const {
    bool visible = _localeVisible;
    
    if (_locales) {
        uint localeMark = CSLocaleString::localeMark();
        if (_localeCurrentMark != localeMark) {
            _localeCurrentMark = localeMark;

            const CSString* locale = CSLocaleString::locale();
            _localeCurrentContains = (locale && _locales->containsObject(locale));
        }
        if (_localeCurrentContains) {
            visible = !visible;
        }
    }
    return visible;
}

void CSAnimation::addLocale(const CSString* locale) {
    if (!_locales) _locales = new CSSet<CSString>();
    _locales->addObject(locale);
    _localeCurrentMark = 0;
}

void CSAnimation::removeLocale(const CSString* locale) {
    if (_locales) {
        _locales->removeObject(locale);
        _localeCurrentMark = 0;
        if (!_locales->count()) {
            _locales->release();
            _locales = NULL;
        }
    }
}

void CSAnimation::setSubstance(CSAnimationSubstance* substance) {
    if (substance != _substance) {
        if (_substance) {
            _substance->attach(NULL);
        }
        _substance = retain(substance);
        if (_substance) {
            _substance->attach(this);
        }
    }
}

void CSAnimation::setDerivation(CSAnimationDerivation* derivation) {
    if (_derivation != derivation) {
        if (_derivation) {
            _derivation->attach(NULL);
        }
        _derivation = retain(derivation);
        if (_derivation) {
            _derivation->attach(this);
        }
    }
}

void CSAnimation::setTransform(const CSMatrix* transform) {
    if (transform) {
        if (!_transform) {
            _transform = new CSMatrix(*transform);
        }
        else {
            *_transform = *transform;
        }
    }
    else if (_transform) {
        delete _transform;
        _transform = NULL;
    }
}

void CSAnimation::makeSubkey() {
    _subkey = 0;
    if (_derivation) {
        foreach(CSAnimation*, child, _derivation->animations()) {
            child->makeSubkey();
            _subkey |= child->_key | child->_subkey;
        }
    }
}

void CSAnimation::rewind() {
    if (_facingCapture) {
        delete _facingCapture;
        _facingCapture = NULL;
    }
    if (_facingRotation) {
        delete _facingRotation;
        _facingRotation = NULL;
    }
    _positionRandom = randLong();
    _rotationRandom = randLong();
    _scaleRandom = randLong();
    _alphaTestRandom = randLong();
    _derivationRandom = randLong();
    _soundCounter = 0;
    _progress = -_latency;
    if (_substance) {
        _substance->rewind();
    }
    if (_derivation) {
        _derivation->rewind();
    }
    _visible = !_closing && !_latency && checkLocale();
}
float CSAnimation::remaining(const CSCamera* camera, CSAnimationDuration type) const {
    float remaining = this->duration(camera, type);
    
    remaining -= _progress + _latency;
    
    return remaining > 0.0f ? remaining : 0.0f;
}

static float randomRate(int64 r, int s) {
    return ((CSMath::shift((uint64)r, s) & 0xffff) / 65535.0f);
}

static float randomRate(int64 r, int ss, int es, float p) {
    return CSMath::lerp(randomRate(r, ss), randomRate(r, es), p);
}

static CSVector3 tangentialDirection(const CSVector3& dir, float angle) {
    CSVector3 rtn(dir.y, -dir.x, 0.0f);
    CSVector3::cross(rtn, dir, rtn);
    CSQuaternion rotation = CSQuaternion::rotationAxis(dir, angle);
    CSVector3::transform(rtn, rotation, rtn);
    rtn.normalize();
    return rtn;
}

bool CSAnimation::capture(float progress, const CSCamera* camera, CSMatrix& capture, bool facing) const {
    if (progress < 0.0f) {
        return false;
    }
    
    bool transformed;
    
    if (_parent) {
        if (!_parent->capture(progress + _parent->progress() - this->progress(), camera, capture)) {
            return false;
        }
        if (_transform) {
            capture = *_transform * capture;
        }
        transformed = (capture != CSMatrix::Identity);
    }
    else if (_transform) {
        capture = *_transform;
        transformed = (capture != CSMatrix::Identity);
    }
    else {
        capture = CSMatrix::Identity;
        transformed = false;
    }

    CSVector3 translation;
    CSQuaternion rotation;
    CSVector3 scale;
    
    if (!_rotationX && !_rotationY && !_rotationZ) {
        rotation = _clientRotation;
    }
    else {
        if (_rotationDuration) {
            int rotation0RandomSeq = 0;
            int rotation1RandomSeq = 0;
            
            float rotationProgress = _rotationLoop.getProgress(progress / _rotationDuration, &rotation0RandomSeq, &rotation1RandomSeq);
            
            rotation0RandomSeq *= 3;
            rotation1RandomSeq *= 3;
            float xr = randomRate(_rotationRandom, rotation0RandomSeq, rotation1RandomSeq, rotationProgress);
            float yr, zr;
            
            if (_rotationFixedComponent) {
                yr = randomRate(_rotationRandom, rotation0RandomSeq + 1, rotation1RandomSeq + 1, rotationProgress);
                zr = randomRate(_rotationRandom, rotation0RandomSeq + 2, rotation1RandomSeq + 2, rotationProgress);
            }
            else {
                yr = xr;
                zr = xr;
            }
            rotation = CSQuaternion::rotationYawPitchRoll(_rotationX ? _rotationX->value(rotationProgress, xr) : 0.0f,
                                                          _rotationY ? _rotationY->value(rotationProgress, yr) : 0.0f,
                                                          _rotationZ ? _rotationZ->value(rotationProgress, zr) : 0.0f);
        }
        else
        {
            float xr = randomRate(_rotationRandom, 0);
            float yr, zr;
            
            if (_rotationFixedComponent) {
                yr = randomRate(_rotationRandom, 1);
                zr = randomRate(_rotationRandom, 2);
            }
            else {
                yr = xr;
                zr = xr;
            }
            rotation = CSQuaternion::rotationYawPitchRoll(_rotationX ? _rotationX->value(1.0f, xr) : 0.0f,
                                                          _rotationY ? _rotationY->value(1.0f, yr) : 0.0f,
                                                          _rotationZ ? _rotationZ->value(1.0f, zr) : 0.0f);
        }
        rotation *= _clientRotation;
    }
    if (!_scaleX && !_scaleY && !_scaleZ) {
        scale = CSVector3(_clientScale, _clientScale, _clientScale);
    }
    else {
        if (_scaleDuration) {
            int scale0RandomSeq = 0;
            int scale1RandomSeq = 0;
            
            float scaleProgress = _scaleLoop.getProgress(progress / _scaleDuration, &scale0RandomSeq, &scale1RandomSeq);
            
            scale0RandomSeq *= 3;
            scale1RandomSeq *= 3;
            float xr = randomRate(_scaleRandom, scale0RandomSeq, scale1RandomSeq, scaleProgress);
            float yr, zr;
            
            if (_scaleFixedComponent) {
                yr = randomRate(_scaleRandom, scale0RandomSeq + 1, scale1RandomSeq + 1, scaleProgress);
                zr = randomRate(_scaleRandom, scale0RandomSeq + 2, scale1RandomSeq + 2, scaleProgress);
            }
            else
            {
                yr = xr;
                zr = xr;
            }
            scale = CSVector3(_scaleX ? _scaleX->value(scaleProgress, xr) : 1.0f, _scaleY ? _scaleY->value(scaleProgress, yr) : 1.0f, _scaleZ ? _scaleZ->value(scaleProgress, zr) : 1.0f);
        }
        else {
            float xr = randomRate(_scaleRandom, 0);
            float yr, zr;
            
            if (_scaleFixedComponent) {
                yr = randomRate(_scaleRandom, 1);
                zr = randomRate(_scaleRandom, 2);
            }
            else
            {
                yr = xr;
                zr = xr;
            }
            scale = CSVector3(_scaleX ? _scaleX->value(1.0f, xr) : 1.0f, _scaleY ? _scaleY->value(1.0f, yr) : 1.0f, _scaleZ ? _scaleZ->value(1.0f, zr) : 1.0f);
        }
        scale *= _clientScale;
    }
    if (scale.x <= 0.0f || scale.y <= 0.0f || scale.z <= 0.0f) {
        return false;
    }
    
    CSVector3 points[2];
    for (int i = 0; i < 2; i++) {
        points[i] = CSVector3(_x ? _x->value(i) : 0.0f, _y ? _y->value(i) : 0.0f, _z ? _z->value(i) : 0.0f);
        if (_billboardOffset && camera) {
            points[i] = camera->right() * points[i].x - camera->up() * points[i].y + camera->forward() * points[i].z;
        }
        points[i] += _clientPositions[i];
        if (!_globals[i] && transformed) {
            CSVector3::transformCoordinate(points[i], capture, points[i]);
        }
    }
    
    CSVector3 normal = _radial || _tangential ? CSVector3::normalize(points[1] - points[0]) : CSVector3::Zero;
    
    if (normal != CSVector3::Zero && _radial) {
        for (int i = 0; i < 2; i++) {
            float radial = _radial->value(i);
            if (radial) {
                points[i] += normal * radial;
            }
        }
    }
    float positionDuration = _positionDegree && _positionUsingSpeed ? points[0].distance(points[1]) / _positionDegree : _positionDegree;
    
    int pos0RandomSeq = 0;
    int pos1RandomSeq = 0;
    
    float positionProgress = positionDuration ? _positionLoop.getProgress(progress / positionDuration, &pos0RandomSeq, &pos1RandomSeq) : 1.0f;
    
    if (_reverse) {
        positionProgress = 1.0f - positionProgress;
    }
    
    pos0RandomSeq *= 6;
    pos1RandomSeq *= 6;
    
    CSVector3 offset(_x ? _x->value(positionProgress, randomRate(_positionRandom, pos0RandomSeq, pos1RandomSeq, positionProgress)) : 0.0f,
                     _y ? _y->value(positionProgress, randomRate(_positionRandom, pos0RandomSeq + 1, pos1RandomSeq + 1, positionProgress)) : 0.0f,
                     _z ? _z->value(positionProgress, randomRate(_positionRandom, pos0RandomSeq + 2, pos1RandomSeq + 2, positionProgress)) : 0.0f);
                     
    if (_billboardOffset && camera) {
        offset = camera->right() * offset.x - camera->up() * offset.y + camera->forward() * offset.z;
    }
    CSVector3 originPoints[2];
    for (int i = 0; i < 2; i++) {
        originPoints[i] = _clientPositions[i] + offset;
        if (!_globals[i] && transformed) {
            CSVector3::transformCoordinate(originPoints[i], capture, originPoints[i]);
        }
    }
    
    CSVector3::lerp(originPoints[0], originPoints[1], positionProgress, translation);
    
    if (normal != CSVector3::Zero) {
        if (_radial) {
            float radial = _radial->value(positionProgress, randomRate(_positionRandom, pos0RandomSeq + 3, pos1RandomSeq + 3, positionProgress));
            if (radial) {
                translation += normal * radial;
            }
        }
        if (_tangential) {
            float tangential = _tangential->value(positionProgress, randomRate(_positionRandom, pos0RandomSeq + 4, pos1RandomSeq + 4, positionProgress));
            if (tangential) {
                float tangentialAngle = CSMath::lerp(_tangentialAngle - _tangentialAngleVar, _tangentialAngle + _tangentialAngleVar, randomRate(_positionRandom, pos0RandomSeq + 5));
                
                translation += tangentialDirection(normal, tangentialAngle) * tangential;
            }
        }
    }
    if (_globals[_reverse] && transformed) {
        capture = CSMatrix::Identity;
    }
    if (_facing && facing) {
        if (_facingRotation) {
            rotation = *_facingRotation * rotation;
        }
        else {
            return false;
        }
    }
    
    capture.m41 = translation.x;
    capture.m42 = translation.y;
    capture.m43 = translation.z;

    if (_rotationFirst) {
        if (rotation != CSQuaternion::Identity) {
            capture = CSMatrix::rotationQuaternion(rotation) * capture;
        }
        if (scale.x != 1.0f) {
            capture.m11 *= scale.x;
            capture.m12 *= scale.x;
            capture.m13 *= scale.x;
        }
        if (scale.y != 1.0f) {
            capture.m21 *= scale.y;
            capture.m22 *= scale.y;
            capture.m23 *= scale.y;
        }
        if (scale.z != 1.0f) {
            capture.m31 *= scale.z;
            capture.m32 *= scale.z;
            capture.m33 *= scale.z;
        }
    }
    else {
        if (scale.x != 1.0f) {
            capture.m11 *= scale.x;
            capture.m12 *= scale.x;
            capture.m13 *= scale.x;
        }
        if (scale.y != 1.0f) {
            capture.m21 *= scale.y;
            capture.m22 *= scale.y;
            capture.m23 *= scale.y;
        }
        if (scale.z != 1.0f) {
            capture.m31 *= scale.z;
            capture.m32 *= scale.z;
            capture.m33 *= scale.z;
        }
        if (rotation != CSQuaternion::Identity) {
            capture = CSMatrix::rotationQuaternion(rotation) * capture;
		}
	}
	return true;
}

float CSAnimation::positionDuration(const CSCamera* camera) const {
	if (_positionUsingSpeed && _positionDegree) {
		CSMatrix capture;
		if (_parent) {
			_parent->capture(_parent->progress(), camera, capture, false);
		}
		else {
			capture = CSMatrix::Identity;
		}
		CSVector3 points[2];
		for (int i = 0; i < 2; i++) {
			points[i] = CSVector3(_x ? _x->value(i) : 0.0f, _y ? _y->value(i) : 0.0f, _z ? _z->value(i) : 0.0f);
			if (_billboardOffset && camera) {
				points[i] = camera->right() * points[i].x - camera->up() * points[i].y + camera->forward() * points[i].z;
			}
			points[i] += _clientPositions[i];
			if (capture != CSMatrix::Identity) {
				CSVector3::transformCoordinate(points[i], capture, points[i]);
			}
		}
		if (_radial) {
			CSVector3 normal = CSVector3::normalize(points[1] - points[0]);

			if (normal) {
				for (int i = 0; i < 2; i++) {
					float radial = _radial->value(i);
					if (radial != 0) {
						points[i] += normal * radial;
					}
				}
			}
		}
		return CSMath::max(points[0].distance(points[1]) / _positionDegree, 0.001f);
	}
	else {
		return _positionDegree;
	}
}

float CSAnimation::duration(const CSCamera* camera, CSAnimationDuration type) const {
	if (!checkLocale()) {
		return 0.0f;
	}

	float duration = _duration;

	float positionDuration = this->positionDuration(camera);

	if (positionDuration) {
		positionDuration = _positionLoop.getDuration(positionDuration);
		if (duration < positionDuration) {
			duration = positionDuration;
		}
	}
	if (_rotationDuration) {
		float rotationDuration = _rotationLoop.getDuration(_rotationDuration);
		if (duration < rotationDuration) {
			duration = rotationDuration;
		}
	}
	if (_scaleDuration) {
		float scaleDuration = _scaleLoop.getDuration(_scaleDuration);
		if (duration < scaleDuration) {
			duration = scaleDuration;
		}
	}
	if (_usingAlphaTest && _alphaTestDuration) {
		float alphaTestDuration = _alphaTestLoop.getDuration(_alphaTestDuration);
		if (duration < alphaTestDuration) {
			duration = alphaTestDuration;
		}
	}
	if (_substance) {
		float substanceDuration = _substance->duration(camera, type, duration);

		if (duration < substanceDuration) {
			duration = substanceDuration;
		}
	}
	if (_derivation) {
		float derivationDuration = _derivation->duration(camera, type, duration);

		if (duration < derivationDuration) {
			duration = derivationDuration;
		}
	}
	if (duration) {
		duration += _latency;
	}

	return duration;
}

bool CSAnimation::isFinite(bool finite) const {
	if (!checkLocale()) {
		return finite;
	}

	if (_duration ||
		(_positionDegree && _positionLoop.finish) ||
		(_rotationDuration && _rotationLoop.finish) ||
		(_scaleDuration && _scaleLoop.finish) ||
		(_alphaTestDuration && _alphaTestLoop.finish))
	{
		finite = true;
	}
	if (_substance) {
		finite = _substance->isFinite(finite);
	}
	if (_derivation) {
		finite = _derivation->isFinite(finite);
	}
	return finite;
}

void CSAnimation::updateFacing(const CSCamera *camera) {
    if (_facing) {
        CSMatrix capture;
        if (this->capture(_progress, camera, capture, false)) {
            if (!_facingCapture) {
                _facingCapture = new CSMatrix(capture);
            }
            else {
                CSVector3 start = _facingCapture->translationVector();
                CSVector3 end = capture.translationVector();
                
                if (start != end) {
                    CSVector3 forward = capture.forward();
                    CSVector3 dir = end - start;
                    CSVector3 w = CSVector3::cross(forward, dir);
                    CSQuaternion rotation(w.x, w.y, w.z, CSVector3::dot(forward, dir));
                    rotation.w += rotation.length();
                    rotation.normalize();
                    
                    if (_facingRotation) {
                        *_facingRotation = rotation;
                    }
                    else {
                        _facingRotation = new CSQuaternion(rotation);
                    }
                    *_facingCapture = capture;
                }
            }
        }
        else {
            if (_facingCapture) {
                delete _facingCapture;
                _facingCapture = NULL;
            }
            if (_facingRotation) {
                delete _facingRotation;
                _facingRotation = NULL;
            }
        }
    }
}

CSAnimationState CSAnimation::update(float delta, const CSCamera* camera, bool alive) {
    if (!checkLocale()) {
        _visible = false;
        return CSAnimationStateNone;
    }
    if (_closing) {
        if (alive) {
            _visible = false;
            return CSAnimationStateAlive;
        }
        else {
            alive = true;
        }
    }
    _progress += delta;
    
    if (_progress < 0.0f) {
        _visible = false;
        return CSAnimationStateAlive;
    }
    if (_duration && _progress >= _duration) {
        alive = false;
    }
    updateFacing(camera);
    
    bool remaining = false;
    
    {
        float positionDuration = this->positionDuration(camera);
        if (positionDuration) {
            _positionLoop.getState(_progress / positionDuration, remaining, alive);
        }
    }
    if (_rotationDuration) {
        _rotationLoop.getState(_progress / _rotationDuration, remaining, alive);
    }
    if (_scaleDuration) {
        _scaleLoop.getState(_progress / _scaleDuration, remaining, alive);
    }
    if (_alphaTestDuration) {
        _alphaTestLoop.getState(_progress / _alphaTestDuration, remaining, alive);
    }
    if (_progress < delta) {
        delta = _progress;
    }
    if (_substance) {
        switch (_substance->update(delta, camera, alive || remaining)) {
            case CSAnimationStateStopped:
                alive = false;
                break;
            case CSAnimationStateAlive:
                remaining = true;
                break;
            case CSAnimationStateFinishing:
                alive = false;
                remaining = true;
                break;
        }
    }
    if (_derivation) {
        switch (_derivation->update(delta, camera, alive || remaining)) {
            case CSAnimationStateStopped:
                if (_derivationFinish) alive = false;
                break;
            case CSAnimationStateAlive:
                remaining = true;
                break;
            case CSAnimationStateFinishing:
                if (_derivationFinish) alive = false;
                remaining = true;
                break;
        }
    }
    _visible = alive || remaining;
    if (_visible && _soundIndices && _soundDelegate && _progress > _soundLatency) {
        int counter = _soundLoop ? (int)((_progress - _soundLatency) / _soundLoop) + 1 : 1;
        
        if (_soundCounter != counter) {
            if (_soundEnabled) {
                (*_soundDelegate)(this, _soundIndices->pointer(), _soundIndices->count(), _soundVolume, _soundEssential);
            }
            _soundCounter = counter;
        }
    }
    if (alive) return CSAnimationStateAlive;
    if (remaining) return CSAnimationStateFinishing;
    return CSAnimationStateStopped;
}

bool CSAnimation::isVisible(uint visibleKey) const {
	return _visible && (_key & visibleKey) == _key && (_substance || (_derivation && _derivation->isVisible(visibleKey)));
}

void CSAnimation::draw(CSGraphics* graphics, uint visibleKey, bool shadow) {
    if (isVisible(visibleKey)) {
        graphics->push();
        graphics->setUsingLight(!shadow && _usingLight);
        if (_usingDepth) {
            graphics->setDepthMode(_depthMode);
            graphics->setDepthBias(_depthBias);
        }
        graphics->setStencilMode(_usingStencil ? CSStencilInclusive : CSStencilNone);
        
        graphics->setUsingAlphaTest(_usingAlphaTest);
        
        if (_usingAlphaTest) {
            if (_alphaTest) {
                if (_alphaTestDuration) {
                    int alphaTestRandom0Seq = 0;
                    int alphaTestRandom1Seq = 0;
                    
                    float alphaTestProgress = _alphaTestLoop.getProgress(_progress / _alphaTestDuration, &alphaTestRandom0Seq, &alphaTestRandom1Seq);
                    
                    float ar = randomRate(_alphaTestRandom, alphaTestRandom0Seq, alphaTestRandom1Seq, alphaTestProgress);
                    
                    float a = CSMath::clamp(_alphaTest->value(alphaTestProgress, ar), 0.0f, 1.0f);
                    
                    graphics->setAlphaTest(a);
                }
                else {
                    float ar = randomRate(_alphaTestRandom, 1);
                    
                    float a = CSMath::clamp(_alphaTest->value(1.0f, ar), 0.0f, 1.0f);
                    
                    graphics->setAlphaTest(a);
                }
            }
            else {
                graphics->setAlphaTest(0.0f);
            }
        }
        if (_substance) {
            _substance->draw(graphics, shadow);
        }
        if (_derivation) {
            _derivation->draw(graphics, visibleKey, shadow);
        }
        graphics->pop();
    }
}

void CSAnimation::preload(CSResourceDelegate* delegate) const {
    if (_substance) {
        _substance->preload(delegate);
    }
    if (_derivation) {
        _derivation->preload(delegate);
    }
}
