//
//  CSParticle.cpp
//  CDK
//
//  Created by 김찬 on 14. 3. 24..
//  Copyright (c) 2014년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSParticle.h"

#include "CSBuffer.h"

#include "CSOpenGL.h"
#include "CSGraphicsImpl.h"

#include "CSRootImage.h"

#include "CSAnimation.h"

#include "CSDiagnostics.h"

CSParticle::CSParticle(const CSParticle* other, bool capture) : CSAnimationSubstance(other) {
    _shape = retain(other->_shape);
    _shapeShell = other->_shapeShell;
    _life = other->_life;
    _lifeVar = other->_lifeVar;
    _emissionRate = other->_emissionRate;
    _emissionMax = other->_emissionMax;
    _duration = other->_duration;
    _latency = other->_latency;
    _prewarm = other->_prewarm;
    _descent = other->_descent;
    _finishAfterEmitting = other->_finishAfterEmitting;
    _clearWithParent = other->_clearWithParent;
    _blendMode = other->_blendMode;
    
    _imageIndices = retain(other->_imageIndices);
    _imageSequence = other->_imageSequence;
    
    _material = other->_material;
    
    _colorRed = retain(other->_colorRed);
    _colorGreen = retain(other->_colorGreen);
    _colorBlue = retain(other->_colorBlue);
    _colorAlpha = retain(other->_colorAlpha);
    _colorFixedComponent = other->_colorFixedComponent;
    
    _radial = retain(other->_radial);
    _x = retain(other->_x);
    _y = retain(other->_y);
    _z = retain(other->_z);
    _billboardX = retain(other->_billboardX);
    _billboardY = retain(other->_billboardY);
    _billboardZ = retain(other->_billboardZ);

    _rotationX = retain(other->_rotationX);
    _rotationY = retain(other->_rotationY);
    _rotationZ = retain(other->_rotationZ);
    _rotationFixedComponent = other->_rotationFixedComponent;
    
    _scaleX = retain(other->_scaleX);
    _scaleY = retain(other->_scaleY);
    _scaleZ = retain(other->_scaleZ);
    _scaleFixedComponent = other->_scaleFixedComponent;
    
    _view = other->_view;
    _stretchRate = other->_stretchRate;
    _localSpace = other->_localSpace;
    
    if (capture) {
        _instances = new CSArray<Instance>(other->_instances, true);
        
        _progress = other->_progress;
        _counter = other->_counter;
        _remaining = other->_remaining;
        _emitting = other->_emitting;
    }
    else {
        _instances = new CSArray<Instance>();
        
        _progress = -_latency;
        if (_prewarm && _emissionRate > 0.0f) {
            _remaining = _emissionMax / _emissionRate;
        }
        _emitting = true;
    }
}
CSParticle::CSParticle(CSBuffer* buffer) {
    _shape = CSParticleShape::createWithBuffer(buffer);
    _shapeShell = buffer->readBoolean();
    _life = buffer->readFloat();
    _lifeVar = buffer->readFloat();
    _emissionRate = buffer->readFloat();
    _emissionMax = buffer->readInt();
    _duration = buffer->readFloat();
    _latency = buffer->readFloat();
    _prewarm = buffer->readBoolean();
    _descent = buffer->readBoolean();
    _finishAfterEmitting = buffer->readBoolean();
    _clearWithParent = buffer->readBoolean();
    
    _blendMode = (CSBlendMode)buffer->readByte();
    
    _imageIndices = retain(buffer->readArray<ushort, 2>());
    _imageSequence = buffer->readByte();
    
    _material = CSMaterial(buffer);
    
    _colorRed = CSMovement::createWithBuffer(buffer);
    _colorGreen = CSMovement::createWithBuffer(buffer);
    _colorBlue = CSMovement::createWithBuffer(buffer);
    _colorAlpha = CSMovement::createWithBuffer(buffer);
    _colorFixedComponent = buffer->readBoolean();
    
    _radial = CSMovement::createWithBuffer(buffer);
    _x = CSMovement::createWithBuffer(buffer);
    _y = CSMovement::createWithBuffer(buffer);
    _z = CSMovement::createWithBuffer(buffer);
    _billboardX = CSMovement::createWithBuffer(buffer);
    _billboardY = CSMovement::createWithBuffer(buffer);
    _billboardZ = CSMovement::createWithBuffer(buffer);

    _rotationX = CSMovement::createWithBuffer(buffer);
    _rotationY = CSMovement::createWithBuffer(buffer);
    _rotationZ = CSMovement::createWithBuffer(buffer);
    _rotationFixedComponent = buffer->readBoolean();
    
    _scaleX = CSMovement::createWithBuffer(buffer);
    _scaleY = CSMovement::createWithBuffer(buffer);
    _scaleZ = CSMovement::createWithBuffer(buffer);
    _scaleFixedComponent = buffer->readBoolean();
    
    _view = (CSParticleView)buffer->readByte();
    if (_view == CSParticleViewStretchBillboard) {
        _stretchRate = buffer->readFloat();
    }
    _localSpace = buffer->readBoolean();
    
    _instances = new CSArray<Instance>();
    
    _progress = -_latency;
    if (_prewarm && _emissionRate > 0.0f) {
        _remaining = _emissionMax / _emissionRate;
    }
    _emitting = true;
}

CSParticle::~CSParticle() {
    release(_shape);
    release(_imageIndices);
    release(_colorRed);
    release(_colorGreen);
    release(_colorBlue);
    release(_colorAlpha);
    release(_radial);
    release(_x);
    release(_y);
    release(_z);
    release(_billboardX);
    release(_billboardY);
    release(_billboardZ);
    release(_rotationX);
    release(_rotationY);
    release(_rotationZ);
    release(_scaleX);
    release(_scaleY);
    release(_scaleZ);
    
    _instances->release();
}
void CSParticle::setColor(const CSMovement* red, const CSMovement* green, const CSMovement* blue, const CSMovement* alpha, bool fixedComponent) {
    retain(_colorRed, red);
    retain(_colorGreen, green);
    retain(_colorBlue, blue);
    retain(_colorAlpha, alpha);
    _colorFixedComponent = fixedComponent;
}
void CSParticle::setPosition(const CSMovement* x, const CSMovement* y, const CSMovement* z) {
    retain(_x, x);
    retain(_y, y);
    retain(_z, z);
}
void CSParticle::setBillboard(const CSMovement* billboardX, const CSMovement* billboardY, const CSMovement* billboardZ) {
    retain(_billboardX, billboardX);
    retain(_billboardY, billboardY);
    retain(_billboardZ, billboardZ);
}
void CSParticle::setRotation(const CSMovement* rotationX, const CSMovement* rotationY, const CSMovement* rotationZ, bool fixedComponent) {
    retain(_rotationX, rotationX);
    retain(_rotationY, rotationY);
    retain(_rotationZ, rotationZ);
    _rotationFixedComponent = fixedComponent;
}
void CSParticle::setScale(const CSMovement* scaleX, const CSMovement* scaleY, const CSMovement* scaleZ, bool fixedComponent) {
    retain(_scaleX, scaleX);
    retain(_scaleY, scaleY);
    retain(_scaleZ, scaleZ);
    _scaleFixedComponent = fixedComponent;
}
float CSParticle::duration(const CSCamera* camera, CSAnimationDuration type, float duration) const {
    if (_duration) {
        float d = _duration + _latency;
        
        if (!duration || duration > d) {
            duration = d;
        }
    }
    if (duration) {
        switch (type) {
            case CSAnimationDurationAvg:
                duration += (_life + _lifeVar) * 0.5f;
                break;
            case CSAnimationDurationMax:
                duration += _life + _lifeVar;
                break;
        }
    }
    return duration;
}

bool CSParticle::isFinite(bool finite) const {
	if (_duration && _finishAfterEmitting) {
		finite = true;
	}
	return finite;
}

void CSParticle::rewind() {
    _instances->removeAllObjects();
    _progress = -_latency;
	_remaining = _prewarm && _emissionRate > 0.0f ? _emissionMax / _emissionRate : 0.0f;
    _counter = 0.0f;
    _emitting = true;
}

CSParticle::Instance::Instance(const Instance& other) {
    memcpy(this, &other, sizeof(Instance));
    if (transform) {
        transform = new CSMatrix(*transform);
    }
}

CSParticle::Instance::~Instance() {
    if (transform) {
        delete transform;
    }
}

typename CSParticle::Instance& CSParticle::Instance::operator =(const Instance& other) {
    if (transform) {
        delete transform;
    }
    memcpy(this, &other, sizeof(Instance));
    if (transform) {
        transform = new CSMatrix(*transform);
    }
    return *this;
}

void CSParticle::addInstance(const CSMatrix* transform, float progress) {
    float life = _life + _lifeVar * randFloat(-1.0f, 1.0f);
    
    if (life <= progress) {
        return;
    }
    Instance& p = _descent ? _instances->insertObject(0) : _instances->addObject();
    
    p.transform = transform ? new CSMatrix(*transform) : NULL;
    p.imageIndex = _imageIndices ? (_imageSequence ? 0 : randInt(0, _imageIndices->count() - 1)) : 0;
    p.life = life;
    p.progress = 0.0f;
    
    if (_colorFixedComponent) {
        p.colorRandom.r = p.colorRandom.g = p.colorRandom.b = p.colorRandom.a = randFloat(0.0f, 1.0f);
    }
    else {
        p.colorRandom.r = randFloat(0.0f, 1.0f);
        p.colorRandom.g = randFloat(0.0f, 1.0f);
        p.colorRandom.b = randFloat(0.0f, 1.0f);
        p.colorRandom.a = randFloat(0.0f, 1.0f);
    }
    _shape->issue(p.pos, p.firstDir, _shapeShell);
    
    p.radialRandom = randFloat(0.0f, 1.0f);
    p.positionRandom.x = randFloat(0.0f, 1.0f);
    p.positionRandom.y = randFloat(0.0f, 1.0f);
    p.positionRandom.z = randFloat(0.0f, 1.0f);
    p.billboardRandom.x = randFloat(0.0f, 1.0f);
    p.billboardRandom.y = randFloat(0.0f, 1.0f);
    p.billboardRandom.z = randFloat(0.0f, 1.0f);
    p.billboard = CSVector3::Zero;
    
    if (_rotationFixedComponent) {
        p.rotationRandom.x = p.rotationRandom.y = p.rotationRandom.z = randFloat(0.0f, 1.0f);
    }
    else {
        p.rotationRandom.x = randFloat(0.0f, 1.0f);
        p.rotationRandom.y = randFloat(0.0f, 1.0f);
        p.rotationRandom.z = randFloat(0.0f, 1.0f);
    }
    if (_scaleFixedComponent) {
        p.scaleRandom.x = p.scaleRandom.y = p.scaleRandom.z = randFloat(0.0f, 1.0f);
    }
    else {
        p.scaleRandom.x = randFloat(0.0f, 1.0f);
        p.scaleRandom.y = randFloat(0.0f, 1.0f);
        p.scaleRandom.z = randFloat(0.0f, 1.0f);
    }
    updateInstance(p, progress);
}

bool CSParticle::updateInstance(Instance& p, float delta) {
    p.progress += delta;
    
    if (p.progress >= p.life) {
        return false;
    }
    float progress = p.progress / p.life;
    
    if (_imageSequence && _imageIndices) {
        p.imageIndex = (progress * _imageSequence - CSMath::floor(progress * _imageSequence)) * _imageIndices->count();
    }
	p.color.r = _colorRed ? CSMath::clamp(_colorRed->value(progress, p.colorRandom.r), 0.0f, 1.0f) : 1.0f;
	p.color.g = _colorGreen ? CSMath::clamp(_colorGreen->value(progress, p.colorRandom.g), 0.0f, 1.0f) : 1.0f;
	p.color.b = _colorBlue ? CSMath::clamp(_colorBlue->value(progress, p.colorRandom.b), 0.0f, 1.0f) : 1.0f;
	p.color.a = _colorAlpha ? CSMath::clamp(_colorAlpha->value(progress, p.colorRandom.a), 0.0f, 1.0f) : 1.0f;
    
    p.lastDir = CSVector3(_x ? _x->value(progress, p.positionRandom.x) : 0.0f, _y ? _y->value(progress, p.positionRandom.y) : 0.0f, _z ? _z->value(progress, p.positionRandom.z) : 0.0f);
    if (_radial) {
        p.lastDir += p.firstDir * _radial->value(progress, p.radialRandom);
    }
    p.pos += p.lastDir * delta;
    
    p.billboardDir = CSVector3(_billboardX ? _billboardX->value(progress, p.billboardRandom.x) : 0.0f,
                               _billboardY ? _billboardY->value(progress, p.billboardRandom.y) : 0.0f,
                               _billboardZ ? _billboardZ->value(progress, p.billboardRandom.z) : 0.0f);
    
    p.billboard += p.billboardDir * delta;
    
    p.rotation.x = _rotationX ? _rotationX->value(progress, p.rotationRandom.x) : 0.0f;
    p.rotation.y = _rotationY ? _rotationY->value(progress, p.rotationRandom.y) : 0.0f;
    p.rotation.z = _rotationZ ? _rotationZ->value(progress, p.rotationRandom.z) : 0.0f;

    p.scale.x = _scaleX ? _scaleX->value(progress, p.scaleRandom.x) : 1.0f;
    p.scale.y = _scaleY ? _scaleY->value(progress, p.scaleRandom.y) : 1.0f;
    p.scale.z = _scaleZ ? _scaleZ->value(progress, p.scaleRandom.z) : 1.0f;

    return true;
}

void CSParticle::updateInstances(const CSCamera* camera) {
	if (_remaining) {
		int i = 0;
		while (i < _instances->count()) {
			Instance& p = _instances->objectAtIndex(i);
			if (updateInstance(p, _remaining)) {
				i++;
			}
			else {
				_instances->removeObjectAtIndex(i);
			}
		}
        if (_emitting && _shape && _emissionRate > 0.0f) {
            float rate = 1.0f / _emissionRate;
            
            float lifeMax = _life + _lifeVar;
            
            _counter += _remaining;
            if (_counter > lifeMax) {
                _counter = CSMath::mod(_counter, rate);
                _counter += CSMath::ceil(lifeMax / rate) * rate;
            }
            
            while (_counter >= rate) {
                _counter -= rate;
                
                if (_counter < lifeMax && _instances->count() < _emissionMax) {
                    if (_parent && !_localSpace) {
                        CSMatrix capture;
                        if (_parent->capture(CSMath::max(_parent->progress() - _counter, 0.0f), camera, capture)) {
                            addInstance(&capture, _counter);
                        }
                    }
                    else {
                        addInstance(NULL, _counter);
                    }
                }
            }
        }
        _remaining = 0.0f;
    }
}

CSAnimationState CSParticle::update(float delta, const CSCamera* camera, bool alive) {
    _progress += delta;
    
    if (_progress < 0.0f) {
        return CSAnimationStateAlive;
    }
    else if (_progress < delta) {
        delta = _progress;
    }
    _remaining += delta;
    
    _emitting = alive && (!_duration || _progress < _duration);
    
    if (_emitting) {
        return CSAnimationStateAlive;
    }
    else {
        if (!alive && _clearWithParent) {
            _instances->removeAllObjects();
        }
        else {
            foreach(const Instance&, p, _instances) {
                if (p.progress + _remaining < p.life) {
                    return alive ? CSAnimationStateAlive : CSAnimationStateFinishing;
                }
            }
        }
        return _finishAfterEmitting ? CSAnimationStateStopped : CSAnimationStateNone;
    }
}

void CSParticle::draw(CSGraphics* graphics, bool shadow) {
    if (shadow || _progress < 0) return;
    
	const CSCamera& camera = graphics->camera();

    updateInstances(&camera);
    
    if (_instances->count()) {
		const CSMatrix& view = camera.view();

		CSMatrix worldOrigin;
		CSMatrix worldBillboard;
		if (_localSpace) {
			if (_parent) {
				if (!_parent->capture(_parent->progress(), &camera, worldOrigin)) {
					return;
				}
				worldOrigin *= graphics->world();
			}
			else {
				worldOrigin = graphics->world();
			}
			switch (_view) {
				case CSParticleViewBillboard:
					view.billboard(&worldOrigin, worldBillboard);
					break;
				case CSParticleViewHorizontalBillboard:
					view.horizontalBillboard(&worldOrigin, worldBillboard);
					break;
				case CSParticleViewVerticalBillboard:
					view.verticalBillboard(&worldOrigin, worldBillboard);
					break;
			}
		}
		else {
			worldOrigin = graphics->world();
		}

        CSArray<const CSImage*>* images = new CSArray<const CSImage*>();
        
        const CSRootImage* root = NULL;
        if (_resourceDelegate && _imageIndices) {
            foreach(const CSArray<ushort>*, indices, _imageIndices) {
                const CSImage* image = static_cast<const CSImage*>((*_resourceDelegate)(CSResourceTypeImage, indices->pointer(), indices->count()));
                
                if (image) {
                    if (!root) {
                        root = image->root();
                    }
                    else {
                        CSAssert(root == image->root(), "invalid image");
                    }
					images->addObject(image);
				}
				else {
					images->release();
					return;
				}
            }
        }
        else {
			root = CSGraphicsImpl::sharedImpl()->particleImage();

			if (!root) {
				images->release();
				return;
			}

            images->addObject(root);
        }

		graphics->push();
        
        graphics->setCullMode(CSCullNone);
        graphics->setBlendMode(_blendMode);
        graphics->setStrokeWidth(0);
        graphics->setMaterial(_material);

		graphics->begin();

        const CSSize& textureSize = root->textureSize();
		const CSVector3& right = camera.right();
		const CSVector3& up = camera.up();
		const CSVector3& forward = camera.forward();

		uint vi = 0;
		
		foreach (Instance&, p, _instances) {
            const CSImage* image = images->objectAtIndex(CSMath::min((int)p.imageIndex, (int)images->count() - 1));
            
            if (image) {
                CSMatrix worldPrev = worldOrigin;
				if (p.transform && !_localSpace) worldPrev = *p.transform * worldPrev;

				CSMatrix world;

				switch (_view) {
					case CSParticleViewBillboard:
						if (_localSpace) world = worldBillboard;
						else view.billboard(&worldPrev, world);
						break;
					case CSParticleViewHorizontalBillboard:
						if (_localSpace) world = worldBillboard;
						else view.horizontalBillboard(&worldPrev, world);
						break;
					case CSParticleViewVerticalBillboard:
						if (_localSpace) world = worldBillboard;
						else view.verticalBillboard(&worldPrev, world);
						break;
					case CSParticleViewStretchBillboard:
						{
							CSVector3 dir = p.lastDir;
							dir += right * p.billboardDir.x;
							dir -= up * p.billboardDir.y;
							dir += forward * p.billboardDir.z;
							if (!view.stretchBillboard(&worldPrev, dir, _stretchRate, world)) {
								continue;
							}
						}
						break;
					default:
						world = CSMatrix::Identity;
						break;
				}
                if (p.rotation != CSVector3::Zero) {
                    world = CSMatrix::rotationYawPitchRoll(p.rotation.x, p.rotation.y, p.rotation.z) * world;
                }
                if (p.scale.x != 1.0f) {
                    world.m11 *= p.scale.x;
                    world.m12 *= p.scale.x;
                    world.m13 *= p.scale.x;
                }
                if (p.scale.y != 1.0f) {
                    world.m21 *= p.scale.y;
                    world.m22 *= p.scale.y;
                    world.m23 *= p.scale.y;
                }
                if (p.scale.z != 1.0f) {
                    world.m31 *= p.scale.z;
                    world.m32 *= p.scale.z;
                    world.m33 *= p.scale.z;
                }
                world.m41 = p.pos.x;
                world.m42 = p.pos.y;
                world.m43 = p.pos.z;
                world *= worldPrev;
                
                if (p.billboard.x) {
                    CSVector3 offset = right * p.billboard.x * worldPrev.m11;
                    world.m41 += offset.x;
                    world.m42 += offset.y;
                    world.m43 += offset.z;
                }
                if (p.billboard.y) {
                    CSVector3 offset = -up * p.billboard.y * worldPrev.m22;
                    world.m41 += offset.x;
                    world.m42 += offset.y;
                    world.m43 += offset.z;
                }
                if (p.billboard.z) {
                    CSVector3 offset = forward * p.billboard.z * worldPrev.m33;
                    world.m41 += offset.x;
                    world.m42 += offset.y;
                    world.m43 += offset.z;
                }
                graphics->setWorld(world);
            
                const CSRect& frame = image->frame();
                
                float hw = frame.size.width * 0.5f;
                float hh = frame.size.height * 0.5f;
                
                CSVertex vertex[4];
                
                vertex[0].position = CSVector3(-hw, -hh, 0.0f);
                vertex[1].position = CSVector3(hw, -hh, 0.0f);
                vertex[2].position = CSVector3(-hw, hh, 0.0f);
                vertex[3].position = CSVector3(hw, hh, 0.0f);
                
                vertex[0].color = p.color;
                vertex[1].color = p.color;
                vertex[2].color = p.color;
                vertex[3].color = p.color;
                
                vertex[0].textureCoord.x = vertex[2].textureCoord.x = frame.left() / textureSize.width;
                vertex[0].textureCoord.y = vertex[1].textureCoord.y = frame.top() / textureSize.height;
                vertex[1].textureCoord.x = vertex[3].textureCoord.x = frame.right() / textureSize.width;
                vertex[2].textureCoord.y = vertex[3].textureCoord.y = frame.bottom() / textureSize.height;
                
				vertex[0].normal = vertex[1].normal = vertex[2].normal = vertex[3].normal = CSVector3::UnitZ;
                
                graphics->addIndex(vi + 0);
                graphics->addIndex(vi + 1);
                graphics->addIndex(vi + 2);
                graphics->addIndex(vi + 1);
                graphics->addIndex(vi + 3);
                graphics->addIndex(vi + 2);
                
                graphics->addVertex(vertex[0]);
                graphics->addVertex(vertex[1]);
                graphics->addVertex(vertex[2]);
                graphics->addVertex(vertex[3]);
                
                vi += 4;
            }
        }
        images->release();
        
        graphics->end(root, GL_TRIANGLES);
        
        graphics->pop();
    }
}
void CSParticle::preload(CSResourceDelegate* delegate) const {
    if (!delegate) delegate = _resourceDelegate;
    if (delegate) {
        foreach(const CSArray<ushort>*, imageIndices, _imageIndices) {
            (*delegate)(CSResourceTypeImage, imageIndices->pointer(), imageIndices->count());
        }
    }
}
