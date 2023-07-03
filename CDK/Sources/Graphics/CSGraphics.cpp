//
//  CSGraphics.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

#include "CSGraphicsImpl.h"

#include "CSThread.h"

#include "CSGraphics+StrokeAlpha.h"

#define GeometryInstanceCapacity	64
#define VertexCapacity				32
#define IndexCapacity				48
#define AccumVertexCapacity			1024

CSGraphics::Geometry::Geometry() {
    memset(&var, 0, sizeof(var));
}

CSGraphics::Geometry::Geometry(const Geometry& other) :
    camera(other.camera),
    convolution(other.convolution),
    var(other.var)
{

}

typename CSGraphics::Geometry& CSGraphics::Geometry::operator =(const Geometry& other) {
    camera = other.camera;
    convolution = other.convolution;
    var = other.var;
    return *this;
}

bool CSGraphics::Geometry::operator ==(const Geometry& other) const {
    return memcmp(&var, &other.var, sizeof(var)) == 0 && camera == other.camera && convolution == other.convolution;
}

bool CSGraphics::Geometry::usingStroke() const {
    return (var.blendMode == CSBlendNormal || var.blendMode == CSBlendNone) && var.sub.strokeWidth != 0 && var.sub.strokeColor.a != 0;
}

CSGraphics::DynamicGeometrySource::DynamicGeometrySource(uint mode) :
	indices(new CSArray<ushort>(IndexCapacity)), 
	indexOffset(0),
	mode(mode) 
{

}

CSGraphics::DynamicGeometrySource::~DynamicGeometrySource() {
	indices->release();
}

CSGraphics::StaticGeometrySource::StaticGeometrySource(const CSVertexArray* arr, uint indexOffset, uint indexCount, uint mode, const CSMatrix& world, const CSColor& color) :
    arr(CSObject::retain(arr)),
    indexOffset(indexOffset),
    indexCount(indexCount),
    mode(mode),
    world(world),
    color(color)
{

}

CSGraphics::StaticGeometrySource::~StaticGeometrySource() {
    arr->release();
}

CSGraphics::GeometryInstance::GeometryInstance(const Geometry& geometry, const CSRootImage* image) :
    geometry(geometry),
    image(CSObject::retain(image)),
    renderBounds(NULL),
    dynamicSources(NULL),
    staticSources(NULL)
{

}

CSGraphics::GeometryInstance::~GeometryInstance() {
    CSObject::release(image);
    CSObject::release(renderBounds);
    CSObject::release(dynamicSources);
    CSObject::release(staticSources);
}

CSGraphics::State::State(CSRenderTarget* target, State* prev) :
	worldReserved(NULL),
	colorReserved(NULL),
    font(NULL),
    stencilBounds(CSRenderBounds::None),
    usingStencil(false),
    mark(false),
    prev(prev)
{
    reset(target);
}
CSGraphics::State::~State() {
    CSAssert(!usingStencil, "invalid operation");
	release(worldReserved);
	release(colorReserved);
    font->release();
}
void CSGraphics::State::reset(CSRenderTarget* target) {
    CSAssert(!usingStencil, "invalid operation");
    
	release(worldReserved);
	release(colorReserved);
	
	if (prev) {
        world = prev->world;
        color = prev->color;
        retain(font, prev->font);
        memcpy(fontColors, prev->fontColors, sizeof(fontColors));
        usingRenderOrder = prev->usingRenderOrder;
        geometry = prev->geometry;
    }
    else {
        world = CSMatrix::Identity;
        color = CSColor::White;
        retain(font, CSGraphics::defaultFont());
        fontColors[0] = fontColors[1] = fontColors[2] = fontColors[3] = CSColor::White;
        usingRenderOrder = true;
        geometry.camera = CSCamera(0, target->width(), target->height(), 10, 10000);
        geometry.convolution = CSConvolution::None;
        geometry.var.lightness = 0.0f;
        geometry.var.brightness = 1.0f;
        geometry.var.contrast = 1.0f;
        geometry.var.saturation = 1.0f;
        geometry.var.blendMode = CSBlendNormal;
        geometry.var.maskingMode = CSMaskingNone;
        geometry.var.sub.strokeColor = CSColor::Black;
        geometry.var.sub.strokeMode = CSStrokeNormal;
        geometry.var.sub.strokeWidth = 0;
        geometry.var.cullMode = CSCullNone;
        geometry.var.depthMode = CSDepthNone;
        geometry.var.sub.depthBias = 0.0f;
        geometry.var.stencilMode = CSStencilNone;
        geometry.var.stencilDepth = 0;
        geometry.var.usingAlphaTest = false;
        geometry.var.sub.alphaTest = 0.0f;
        geometry.var.usingScissor = false;
        geometry.var.sub.scissor = CSRect::Zero;
        geometry.var.usingLight = false;
        geometry.var.usingShadow = false;
        geometry.var.sub.light = CSLight::Default;
        geometry.var.sub.material = CSMaterial::Default;
        geometry.var.lineWidth = 1.0f;
        geometry.var.layer = 0;
    }
}

static CSGraphics* __currentGraphics = NULL;

CSGraphics::CSGraphics(CSRenderTarget* target) :
    _target(retain(target)),
    _state(new State(target, NULL)),
    _instances(new CSArray<GeometryInstance>(GeometryInstanceCapacity)),
    _vertices(new CSArray<CSVertex>(VertexCapacity)),
	_accumVertices(new CSArray<CSVertex>(AccumVertexCapacity)),
    _indices(new CSArray<ushort>(IndexCapacity)),
    _dirtyOffscreen(true),
    _lastLineWidth(1.0f),
    _validation(CSGraphicsValidationAll)
{
	glGenVertexArraysCS(1, &_vao);
	glBindVertexArrayCS(_vao);

	glGenBuffersCS(2, &_vbo);

	glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
	foreach(const CSVertexLayout, layout, CSVertexLayouts) {
		glVertexAttribPointerCS(layout.attrib, layout.size, layout.type, layout.normalized, sizeof(CSVertex), (const GLvoid*)(int64)layout.offset);
	}
	glEnableVertexAttribArrayCS(CSVertexAttribPosition);
	glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glBindVertexArrayCS(0);
	glBindBufferCS(GL_ARRAY_BUFFER, 0);
	glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, 0);
}

CSGraphics::~CSGraphics() {
    _target->release();
    
    State* current = _state;
    while (current) {
        State* prev = current->prev;
        delete current;
        current = prev;
    }
    _instances->release();
    _vertices->release();
	_accumVertices->release();
    _indices->release();
    
	glDeleteBuffersCS(2, &_vbo);
	glDeleteVertexArraysCS(1, &_vao);

	if (__currentGraphics == this) {
        __currentGraphics = NULL;
    }
}

void CSGraphics::invalidate(uint validation) {
    if (__currentGraphics) {
        __currentGraphics->_validation |= validation;
    }
}

void CSGraphics::invalidate(const CSRenderTarget *target, uint validation) {
    if (__currentGraphics && __currentGraphics->target() != target) {
        __currentGraphics->_validation |= validation;
    }
}

void CSGraphics::validate() {
    if (this != __currentGraphics) {
        if (__currentGraphics) {
            __currentGraphics->_validation = CSGraphicsValidationAll;
        }
        __currentGraphics = this;
    }
    if (_validation) {
        if (_validation & CSGraphicsValidationTarget) {
            _target->validate();
        }
        if (_validation & CSGraphicsValidationBlend) {
            applyBlendMode();
        }
        if (_validation & CSGraphicsValidationCull) {
            applyCullMode();
        }
        if (_validation & CSGraphicsValidationDepth) {
            applyDepthMode();
        }
        if (_validation & CSGraphicsValidationStencil) {
            applyStencilMode();
        }
        if (_validation & CSGraphicsValidationLineWidth) {
            glLineWidthCS(_lastLineWidth);
        }
        if (_validation & CSGraphicsValidationScissor) {
            applyScissor();
        }
        _validation = 0;
    }
}

//clear

void CSGraphics::clear(const CSColor& color) {
    commit();
    
    glDepthMaskCS(GL_TRUE);
    glColorMaskCS(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glClearDepthfCS(1);
    glClearStencilCS(0);
    glClearColorCS(color.r, color.g, color.b, color.a);
    
    glClearCS(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    bool depthMask = (_lastDepthMode & CSDepthWrite) != 0;
    bool colorMask = (_lastStencilMode == CSStencilNone);
    glDepthMaskCS(depthMask);
    glColorMaskCS(colorMask, colorMask, colorMask, colorMask);
}

void CSGraphics::clearDepth() {
    render();
    
    glDepthMaskCS(GL_TRUE);
    glClearDepthfCS(1);
    glClearCS(GL_DEPTH_BUFFER_BIT);
    
    bool depthMask = (_lastDepthMode & CSDepthWrite) != 0;
    glDepthMaskCS(depthMask);
}

void CSGraphics::clearStencil() {
    if (_state->usingStencil) {
        commit();
        
        if (_state->stencilBounds.onScreen()) {
            _state->stencilBounds.clip();

            bool clearByShader = _state->geometry.var.stencilDepth > 1;
            
            if (!clearByShader) {
                float size = (_state->stencilBounds.right - _state->stencilBounds.left) * (_state->stencilBounds.bottom - _state->stencilBounds.top);
                
                static const float clearByShaderMaxSize = 4 * 0.36f;        //벤치마크에서 전체화면의 36% 정도까지 쉐이더가 더 빠름 (ipad 4)
                
                if (size <= clearByShaderMaxSize) {
                    clearByShader = true;
                }
            }
            if (clearByShader) {
                glEnableCS(GL_STENCIL_TEST);
                glStencilFuncCS(GL_EQUAL, _state->geometry.var.stencilDepth, 0xff);
                glStencilOpCS(GL_KEEP, GL_KEEP, GL_DECR);
                glColorMaskCS(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                CSGraphicsImpl::sharedImpl()->fillShader()->draw(NULL, _state->stencilBounds);
                _validation |= CSGraphicsValidationStencil;
            }
            else {
                glClearStencilCS(0);
                glClearCS(GL_STENCIL_BUFFER_BIT);
            }
        }
        _state->geometry.var.stencilDepth--;
        _state->stencilBounds = CSRenderBounds::None;
        _state->usingStencil = false;
    }
}

void CSGraphics::commit() {
    render();
    if (_lastBlendMode != _state->geometry.var.blendMode) {
        _lastBlendMode = _state->geometry.var.blendMode;
        _validation |= CSGraphicsValidationBlend;
    }
    if (_lastCullMode != _state->geometry.var.cullMode) {
        _lastCullMode = _state->geometry.var.cullMode;
        _validation |= CSGraphicsValidationCull;
    }
    if (_lastDepthMode != _state->geometry.var.depthMode) {
        _lastDepthMode = _state->geometry.var.depthMode;
        _validation |= CSGraphicsValidationDepth;
    }
    if (_lastStencilMode != _state->geometry.var.stencilMode || _lastStencilDepth != _state->geometry.var.stencilDepth) {
        _lastStencilMode = _state->geometry.var.stencilMode;
        _lastStencilDepth = _state->geometry.var.stencilDepth;
        _validation |= CSGraphicsValidationStencil;
    }
    if (_lastLineWidth != _state->geometry.var.lineWidth) {
        _lastLineWidth = _state->geometry.var.lineWidth;
        _validation |= CSGraphicsValidationLineWidth;
    }
    if (_lastUsingScissor != _state->geometry.var.usingScissor || (_state->geometry.var.usingScissor && _lastScissor != _state->geometry.var.sub.scissor)) {
        _lastUsingScissor = _state->geometry.var.usingScissor;
        _lastScissor = _state->geometry.var.sub.scissor;
        _validation |= CSGraphicsValidationScissor;
    }
    validate();
}

void CSGraphics::setup(CSRenderProgram* program, const Geometry& geometry, const CSRootImage* image) {
    glUseProgramCS(program->object());
    program->setCamera(geometry.camera);
    program->setImage(image);
    program->setLightness(geometry.var.lightness);
    program->setBrightness(geometry.var.brightness);
    program->setContrast(geometry.var.contrast);
    program->setSaturation(geometry.var.saturation);
    program->setAlphaTest(geometry.var.sub.alphaTest);
    program->setDepthBias(geometry.var.sub.depthBias, geometry.camera);
    program->setConvolution(geometry.convolution);
    program->setLight(geometry.var.sub.light);
    program->setMaterial(geometry.var.sub.material);
}

void CSGraphics::render() {
    validate();
    
    if (_instances->count()) {
		if (_accumVertices->count() && _accumIndexCount) {
			//CSLog("render:%d, %d", _accumVertices->count(), _accumIndexCount);

			uint indexOffset = 0;

			ushort* indices = (ushort*)fmalloc(sizeof(ushort) * _accumIndexCount);
			foreach(GeometryInstance&, instance, _instances) {
				foreach(DynamicGeometrySource&, source, instance.dynamicSources) {
					memcpy(indices + indexOffset, source.indices->pointer(), sizeof(ushort) * source.indices->count());
					source.indexOffset = indexOffset;
					indexOffset += source.indices->count();
				}
			}
			glBindVertexArrayCS(_vao);
			glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
			glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, _ibo);
			glBufferDataCS(GL_ARRAY_BUFFER, sizeof(CSVertex) * _accumVertices->count(), _accumVertices->pointer(), GL_STREAM_DRAW);
			glBufferDataCS(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * _accumIndexCount, indices, GL_STREAM_DRAW);
			free(indices);

			glBindVertexArrayCS(0);
			glBindBufferCS(GL_ARRAY_BUFFER, 0);
			glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
    
        uint start = 0;
        
        while (start < _instances->count()) {
            const GeometryInstance& instance = _instances->objectAtIndex(start);
            
            uint end = start + 1;
            
            if (instance.geometry.var.layer) {
                while (end < _instances->count() && _instances->objectAtIndex(end).geometry.var.layer == instance.geometry.var.layer) {
                    end++;
                }
                if (start + 1 < end) {
                    for (uint i = start; i < end; i++) {
                        const GeometryInstance& currentInstance = _instances->objectAtIndex(i);
                        
                        if (currentInstance.geometry.usingStroke()) {
                            render(currentInstance, true, false);
                        }
                    }
                    for (uint i = start; i < end; i++) {
                        const GeometryInstance& currentInstance = _instances->objectAtIndex(i);
                        render(currentInstance, false, true);
                    }
                }
                else {
                    render(instance, true, true);
                }
            }
            else {
                render(instance, true, true);
            }
            start = end;
        }
		/*
		if (_accumVertexCount && _accumIndexCount) {		//TODO:TEST, INVALIDATE?
			glBindVertexArrayCS(_vao);
			glBindBufferCS(GL_ARRAY_BUFFER, _vbo);
			glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, _ibo);
			glBufferDataCS(GL_ARRAY_BUFFER, sizeof(CSVertex) * _accumVertexCount, NULL, GL_STREAM_DRAW);
			glBufferDataCS(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * _accumIndexCount, NULL, GL_STREAM_DRAW);
			glBindVertexArrayCS(0);
			glBindBufferCS(GL_ARRAY_BUFFER, 0);
			glBindBufferCS(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		*/
		_instances->removeAllObjects();
		_accumVertices->removeAllObjects();
		_accumIndexCount = 0;
		_dirtyOffscreen = true;
	}
}

void CSGraphics::render(const GeometryInstance& instance, bool background, bool foreground) {
    CSRenderState rs;
    
    if (_lastCullMode != instance.geometry.var.cullMode) {
        _lastCullMode = instance.geometry.var.cullMode;
        applyCullMode();
    }
    if (_lastDepthMode != instance.geometry.var.depthMode) {
        _lastDepthMode = instance.geometry.var.depthMode;
        applyDepthMode();
    }
    if (_lastStencilMode != instance.geometry.var.stencilMode || _lastStencilDepth != instance.geometry.var.stencilDepth) {
        _lastStencilMode = instance.geometry.var.stencilMode;
        _lastStencilDepth = instance.geometry.var.stencilDepth;
        applyStencilMode();
    }
    if (_lastLineWidth != instance.geometry.var.lineWidth) {
        _lastLineWidth = instance.geometry.var.lineWidth;
        glLineWidthCS(_lastLineWidth);
    }
    if (_lastUsingScissor != instance.geometry.var.usingScissor || (instance.geometry.var.usingScissor && _lastScissor != instance.geometry.var.sub.scissor)) {
        _lastUsingScissor = instance.geometry.var.usingScissor;
        _lastScissor = instance.geometry.var.sub.scissor;
        applyScissor();
    }
    rs.image = instance.image;
    rs.usingLightness = instance.geometry.var.lightness != 0.0f;
    rs.usingBrightness = instance.geometry.var.brightness != 1.0f;
    rs.usingContrast = instance.geometry.var.contrast != 1.0f;
    rs.usingSaturation = instance.geometry.var.saturation != 1.0f;
    rs.usingShadow = instance.geometry.var.usingShadow;
    rs.usingAlphaTest = instance.geometry.var.usingAlphaTest;
    
    if (instance.geometry.var.sub.depthBias) {
        if (instance.geometry.camera.fov()) {
            rs.depthBias = CSRenderStateDepthBiasPerspective;
        }
        else {
            rs.depthBias = CSRenderStateDepthBiasOrtho;
        }
    }
    else {
        rs.depthBias = CSRenderStateDepthBiasNone;
    }
    if (background && instance.geometry.usingStroke()) {
        CSColor strokeColor;
        switch (instance.geometry.var.sub.strokeMode) {
            case CSStrokeNormal:
                rs.blendMode = CSBlendNormal;
                strokeColor = instance.geometry.var.sub.strokeColor;
            
                if (strokeColor.a > 0.0f && strokeColor.a < 1.0f) {
                    float fa = strokeColor.a * 255;
                    int ia = fa;
                    strokeColor.a = CSMath::lerp(strokeAlphas[ia], strokeAlphas[ia + 1], fa - ia);
                }
                break;
            case CSStrokeLighten:
                rs.blendMode = CSBlendAdd;
                strokeColor.r = instance.geometry.var.sub.strokeColor.r * instance.geometry.var.sub.strokeColor.a / 3;
                strokeColor.g = instance.geometry.var.sub.strokeColor.g * instance.geometry.var.sub.strokeColor.a / 3;
                strokeColor.b = instance.geometry.var.sub.strokeColor.b * instance.geometry.var.sub.strokeColor.a / 3;
                strokeColor.a = 1.0f;
                break;
        }
        if (_lastBlendMode != rs.blendMode) {
            _lastBlendMode = rs.blendMode;
            applyBlendMode();
        }
		rs.maskingMode = instance.geometry.var.maskingMode == CSMaskingNone ? CSMaskingAlpha : instance.geometry.var.maskingMode;
		rs.usingOffset = true;
		rs.usingLight = false;
		rs.convolutionWidth = 0;
		rs.color = CSRenderStateColorUniformOnly;

        CSVector2 screenOffset(2.0f / instance.geometry.camera.width(), 2.0f / instance.geometry.camera.height());
        
        if (instance.dynamicSources) {
            rs.usingWorld = false;
            CSRenderProgram* program = CSGraphicsImpl::sharedImpl()->renderer()->program(rs);
            setup(program, instance.geometry, instance.image);
            program->setColor(strokeColor);
            
			glBindVertexArrayCS(_vao);
			glDisableVertexAttribArrayCS(CSVertexAttribColor);
            if (instance.image) {
                glEnableVertexAttribArrayCS(CSVertexAttribTextureCoord);
            }
			else {
				glDisableVertexAttribArrayCS(CSVertexAttribTextureCoord);
			}
			glDisableVertexAttribArrayCS(CSVertexAttribNormal);
            
            foreach(const DynamicGeometrySource&, source, instance.dynamicSources) {
                for (int y = -instance.geometry.var.sub.strokeWidth; y <= instance.geometry.var.sub.strokeWidth; y += instance.geometry.var.sub.strokeWidth) {
                    for (int x = -instance.geometry.var.sub.strokeWidth; x <= instance.geometry.var.sub.strokeWidth; x += instance.geometry.var.sub.strokeWidth) {
                        program->setOffset(CSVector2(x, y) * screenOffset);

						glDrawElementsCS(source.mode, source.indices->count(), GL_UNSIGNED_SHORT, (const GLvoid*)(sizeof(ushort) * source.indexOffset));
                    }
                }
            }
			glBindVertexArrayCS(0);
        }
        if (instance.staticSources) {
            rs.usingWorld = true;
            CSRenderProgram* program = CSGraphicsImpl::sharedImpl()->renderer()->program(rs);
            setup(program, instance.geometry, instance.image);
            program->setColor(strokeColor);
            
            foreach(const StaticGeometrySource&, source, instance.staticSources) {
                program->setWorld(source.world);
                source.arr->setLayoutEnabled(CSVertexAttribPosition, true);
                source.arr->setLayoutEnabled(CSVertexAttribColor, false);
                source.arr->setLayoutEnabled(CSVertexAttribTextureCoord, instance.image != NULL);
                source.arr->setLayoutEnabled(CSVertexAttribNormal, false);
                for (int y = -instance.geometry.var.sub.strokeWidth; y <= instance.geometry.var.sub.strokeWidth; y += instance.geometry.var.sub.strokeWidth) {
                    for (int x = -instance.geometry.var.sub.strokeWidth; x <= instance.geometry.var.sub.strokeWidth; x += instance.geometry.var.sub.strokeWidth) {
                        program->setOffset(CSVector2(x, y) * screenOffset);
                        source.arr->render(source.indexOffset, source.indexCount, source.mode);
                    }
                }
            }
        }
    }
    if (foreground) {
        rs.blendMode = instance.geometry.var.blendMode;
        if (_lastBlendMode != rs.blendMode) {
            _lastBlendMode = rs.blendMode;
            applyBlendMode();
        }
        rs.maskingMode = instance.geometry.var.maskingMode;
        rs.usingOffset = false;
        rs.usingLight = instance.geometry.var.usingLight;
        rs.convolutionWidth = instance.geometry.convolution;
        
        if (instance.dynamicSources) {
            rs.usingWorld = false;
            rs.color = CSRenderStateColorVertexOnly;
            CSRenderProgram* program = CSGraphicsImpl::sharedImpl()->renderer()->program(rs);
            setup(program, instance.geometry, instance.image);
            
			glBindVertexArrayCS(_vao);

            glEnableVertexAttribArrayCS(CSVertexAttribColor);
            if (instance.image) {
				glEnableVertexAttribArrayCS(CSVertexAttribTextureCoord);
			}
			else {
				glDisableVertexAttribArrayCS(CSVertexAttribTextureCoord);
			}
			if (instance.geometry.var.usingLight) {
				glEnableVertexAttribArrayCS(CSVertexAttribNormal);
			}
			else {
				glDisableVertexAttribArrayCS(CSVertexAttribNormal);
			}

			foreach(const DynamicGeometrySource&, source, instance.dynamicSources) {
				glDrawElementsCS(source.mode, source.indices->count(), GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(ushort) * source.indexOffset));
			}

			glBindVertexArrayCS(0);
		}
		if (instance.staticSources) {
			rs.usingWorld = true;
			rs.color = CSRenderStateColorAll;
			CSRenderProgram* program = CSGraphicsImpl::sharedImpl()->renderer()->program(rs);
			setup(program, instance.geometry, instance.image);

			foreach(const StaticGeometrySource&, source, instance.staticSources) {
				program->setWorld(source.world);
				program->setColor(source.color);
				source.arr->setLayoutEnabled(CSVertexAttribPosition, true);
				source.arr->setLayoutEnabled(CSVertexAttribColor, true);
				source.arr->setLayoutEnabled(CSVertexAttribTextureCoord, instance.image != NULL);
				source.arr->setLayoutEnabled(CSVertexAttribNormal, instance.geometry.var.usingLight);
				source.arr->render(source.indexOffset, source.indexCount, source.mode);
			}
		}
	}
}

void CSGraphics::flush() {
	render();

	_target->flush();
}

void CSGraphics::reset() {
	clearStencil();

	_state->reset(_target);
}

void CSGraphics::push(bool mark) {
	_state->mark = mark;
	_state = new State(_target, _state);
}

void CSGraphics::pop(bool mark) {
	if (mark) {
		while (_state->prev && !_state->prev->mark) {
			clearStencil();

			State* prev = _state->prev;
			delete _state;
			_state = prev;
		}
	}
	if (_state->prev) {
		if (_state->prev->mark == mark) {
			clearStencil();

			State* prev = _state->prev;
			delete _state;
			_state = prev;
		}
	}
	else {
		clearStencil();

		_state->reset(_target);
	}
}

uint CSGraphics::pushCount() const {
	uint count = 0;
	State* current = _state;
	while (current->prev) {
		count++;
		current = current->prev;
	}
	return count;
}

void CSGraphics::pushTransform() {
	if (!_state->worldReserved) _state->worldReserved = new CSArray<CSMatrix>(1);
	_state->worldReserved->addObject(_state->world);
}

void CSGraphics::popTransform() {
	if (_state->worldReserved && _state->worldReserved->count()) {
		_state->world = _state->worldReserved->lastObject();
		_state->worldReserved->removeLastObject();
	}
	else if (_state->prev) {
		_state->world = _state->prev->world;
	}
	else {
		_state->world = CSMatrix::Identity;
	}
}

//change attribute
void CSGraphics::setColor(const CSColor& color, bool inherit) {
    _state->color = color;
    
    if (inherit) {
		if (_state->colorReserved && _state->colorReserved->count()) _state->color *= _state->colorReserved->lastObject();
		else if (_state->prev) _state->color *= _state->prev->color;
    }
}

void CSGraphics::pushColor() {
	if (!_state->colorReserved) _state->colorReserved = new CSArray<CSColor>(1);
	_state->colorReserved->addObject(_state->color);
}

void CSGraphics::popColor() {
	if (_state->colorReserved && _state->colorReserved->count()) {
		_state->color = _state->colorReserved->lastObject();
		_state->colorReserved->removeLastObject();
	}
	else if (_state->prev) {
		_state->color = _state->prev->color;
	}
	else {
		_state->color = CSColor::White;
	}
}

void CSGraphics::setLightness(float lightness, bool inherit) {
    if (_state->prev && inherit) {
        lightness += _state->prev->geometry.var.lightness;
    }
    _state->geometry.var.lightness = lightness;
}

void CSGraphics::setBrightness(float brightness, bool inherit) {
    if (_state->prev && inherit) {
        brightness *= _state->prev->geometry.var.brightness;
    }
    _state->geometry.var.brightness = brightness;
}

void CSGraphics::setContrast(float contrast, bool inherit) {
    if (_state->prev && inherit) {
        contrast *= _state->prev->geometry.var.contrast;
    }
    _state->geometry.var.contrast = contrast;
}

void CSGraphics::setSaturation(float saturation, bool inherit) {
    if (_state->prev && inherit) {
        saturation *= _state->prev->geometry.var.saturation;
    }
    _state->geometry.var.saturation = saturation;
}

void CSGraphics::applyBlendMode() {
    switch (_lastBlendMode) {
        case CSBlendAdd:
            glEnableCS(GL_BLEND);
            glBlendFuncSeparateCS(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationCS(GL_FUNC_ADD);
            break;
        case CSBlendSubstract:
            glEnableCS(GL_BLEND);
            glBlendFuncSeparateCS(GL_ONE, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationSeparateCS(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
            break;
        case CSBlendMultiply:
            glEnableCS(GL_BLEND);
            glBlendFuncSeparateCS(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationCS(GL_FUNC_ADD);
            break;
        case CSBlendScreen:
            glEnableCS(GL_BLEND);
            glBlendFuncSeparateCS(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationCS(GL_FUNC_ADD);
            break;
        case CSBlendNone:
            glDisableCS(GL_BLEND);
            break;
        default:
            glEnableCS(GL_BLEND);
            glBlendFuncCS(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquationCS(GL_FUNC_ADD);
            break;
    }
}

void CSGraphics::setConvolution(const CSConvolution& convolution, bool inherit) {
    if (_state->prev && inherit) {
        _state->geometry.convolution = _state->prev->geometry.convolution;
        _state->geometry.convolution *= convolution;
    }
    else {
        _state->geometry.convolution = convolution;
    }
}

void CSGraphics::setStrokeColor(const CSColor &strokeColor) {
    _state->geometry.var.sub.strokeColor = strokeColor;
}

void CSGraphics::setDefaultFont(const CSFont* font) {
    CSGraphicsImpl::sharedImpl()->setDefaultFont(font);
}

const CSFont* CSGraphics::defaultFont() {
    return CSGraphicsImpl::sharedImpl()->defaultFont();
}

void CSGraphics::applyCullMode() {
    switch (_lastCullMode) {
        case CSCullNone:
            glDisableCS(GL_CULL_FACE);
            break;
        case CSCullBack:
            glEnableCS(GL_CULL_FACE);
            glFrontFaceCS(GL_CW);
            break;
        case CSCullFront:
            glEnableCS(GL_CULL_FACE);
            glFrontFaceCS(GL_CCW);
            break;
    }
}

void CSGraphics::applyDepthMode() {
    if (_lastDepthMode) {
        glEnableCS(GL_DEPTH_TEST);
        if (_lastDepthMode & CSDepthRead) {
            glDepthFuncCS(GL_LEQUAL);
        }
        else {
            glDepthFuncCS(GL_ALWAYS);
        }
        if (_lastDepthMode & CSDepthWrite) {
            glDepthMaskCS(GL_TRUE);
        }
        else {
            glDepthMaskCS(GL_FALSE);
        }
    }
    else {
        glDisableCS(GL_DEPTH_TEST);
        glDepthMaskCS(GL_FALSE);
    }
}

void CSGraphics::setDepthMode(CSDepthMode mode) {
    if (_target->usingDepthStencil() && _state->geometry.var.depthMode != mode) {
        _state->geometry.var.depthMode = mode;
        
        if (mode & CSDepthWrite) {
            _target->useDepthStencil();
        }
    }
}

void CSGraphics::applyStencilMode() {
    glStencilMaskCS(-1);
    
    switch (_lastStencilMode) {
        case CSStencilNone:
            if (_lastStencilDepth) {
                glEnableCS(GL_STENCIL_TEST);
                glStencilFuncCS(GL_EQUAL, _lastStencilDepth, 0xff);
            }
            else {
                glDisableCS(GL_STENCIL_TEST);
            }
            glStencilOpCS(GL_KEEP, GL_KEEP, GL_KEEP);
            glColorMaskCS(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case CSStencilInclusive:
            glEnableCS(GL_STENCIL_TEST);
            glStencilFuncCS(GL_EQUAL, _lastStencilDepth - 1, 0xff);
            glStencilOpCS(GL_KEEP, GL_KEEP, GL_INCR);
            glColorMaskCS(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            break;
        case CSStencilExclusive:
            if (_lastStencilDepth) {
                glEnableCS(GL_STENCIL_TEST);
                glStencilFuncCS(GL_EQUAL, _lastStencilDepth, 0xff);
                glStencilOpCS(GL_KEEP, GL_KEEP, GL_DECR);
            }
            else {
                glDisableCS(GL_STENCIL_TEST);
                glStencilOpCS(GL_KEEP, GL_KEEP, GL_KEEP);
            }
            glColorMaskCS(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            break;
    }
}

void CSGraphics::setStencilMode(CSStencilMode mode) {
    _state->geometry.var.stencilMode = mode;
    if (mode == CSStencilInclusive && !_state->usingStencil && _target->usingDepthStencil()) {
        CSAssert(_state->geometry.var.stencilDepth < 255, "stencil::too many stencil steps");
        _state->geometry.var.stencilDepth++;
        _state->usingStencil = true;
        
        _target->useDepthStencil();
    }
}

void CSGraphics::applyScissor() {
    if (_lastUsingScissor) {
        glEnableCS(GL_SCISSOR_TEST);
        glScissorCS(_lastScissor.origin.x, _lastScissor.origin.y, _lastScissor.size.width, _lastScissor.size.height);
    }
    else {
        glDisableCS(GL_SCISSOR_TEST);
    }
}
void CSGraphics::setScissor(const CSRect& scissor) {
    if (!_state->geometry.var.usingScissor || _state->geometry.var.sub.scissor != scissor) {
        render();
        
        CSRect tscissor = scissor;
        float xrate = _target->width() / _state->geometry.camera.width();
        float yrate = _target->height() / _state->geometry.camera.height();
        tscissor.origin.x *= xrate;
        tscissor.origin.y *= yrate;
        tscissor.size.width *= xrate;
        tscissor.size.height *= yrate;
        
        tscissor.origin.y = _target->height() - tscissor.size.height - tscissor.origin.y;
        
        if (_state->prev->geometry.var.usingScissor) {
            tscissor = _state->prev->geometry.var.sub.scissor.intersectedRect(tscissor);
        }
        _state->geometry.var.usingScissor = true;
        _state->geometry.var.sub.scissor = tscissor;
    }
}
void CSGraphics::clearScissor() {
    if (_state->geometry.var.usingScissor) {
        render();
        
        _state->geometry.var.usingScissor = false;
    }
}

typename CSGraphics::GeometryInstance* CSGraphics::ready(const CSRootImage* image, const CSRenderBounds* renderBounds) {
    GeometryInstance* instance = NULL;
    
    Geometry::Var::Sub sub = _state->geometry.var.sub;

    if (!_state->geometry.usingStroke()) {
        _state->geometry.var.sub.strokeWidth = 0;
        _state->geometry.var.sub.strokeColor = CSColor::Transparent;
        _state->geometry.var.sub.strokeMode = CSStrokeNormal;
    }
    else if (_state->prev) {
        _state->geometry.var.sub.strokeColor *= _state->prev->color;
    }
    if (!_state->geometry.var.usingAlphaTest) {
        _state->geometry.var.sub.alphaTest = 0;
    }
    if (!_state->geometry.var.usingScissor) {
        _state->geometry.var.sub.scissor = CSRect::Zero;
    }
    if (!_state->geometry.var.usingLight) {
        if (!_state->geometry.var.usingShadow) _state->geometry.var.sub.light = CSLight::Default;
        _state->geometry.var.sub.material = CSMaterial::Default;
    }
    if (_state->geometry.var.depthMode == CSDepthNone) {
        _state->geometry.var.sub.depthBias = 0;
    }

    for (int i = _instances->count() - 1; i >= 0; i--) {
        GeometryInstance& otherInstance = _instances->objectAtIndex(i);
        
        if (otherInstance.geometry == _state->geometry && otherInstance.image == image) {
            instance = &otherInstance;
        }
        if (_state->usingRenderOrder && (_state->geometry.var.depthMode != CSDepthReadWrite || !(otherInstance.geometry.var.depthMode & CSDepthRead))) {
            if (renderBounds && otherInstance.renderBounds) {
                foreach(const CSRenderBounds&, otherRenderBounds, otherInstance.renderBounds) {
                    if (renderBounds->contains(otherRenderBounds)) {
                        goto fin;
                    }
                }
            }
            else {
                goto fin;
            }
        }
    }
fin:
#ifdef CS_DIAGNOSTICS
	CSThread::mainThread()->diagnostics()->addGeometry(instance != NULL);
#endif
	if (instance) {
        if (renderBounds) {
            if (instance->renderBounds) {
                instance->renderBounds->addObject(*renderBounds);
            }
        }
        else {
            release(instance->renderBounds);
        }
    }
    else {
		instance = new (&_instances->addObject()) GeometryInstance(_state->geometry, image);
        if (renderBounds) {
            instance->renderBounds = new CSArray<CSRenderBounds>(8);
            instance->renderBounds->addObject(*renderBounds);
        }
    }
    _state->geometry.var.sub = sub;
    
    return instance;
}

void CSGraphics::begin() {
    CSAssert(!_began, "invalid state");
    _began = true;
}

void CSGraphics::end(const CSRootImage* image, uint mode) {
    CSAssert(_began, "invalid state");
    
    int width = _state->geometry.var.sub.strokeWidth;

    if (width) {
        CSVector2 offset(width * 2.0f / _state->geometry.camera.width(), width * 2.0f / _state->geometry.camera.height());
        _renderBounds.left -= offset.x;
        _renderBounds.right += offset.x;
        _renderBounds.top -= offset.y;
        _renderBounds.bottom += offset.y;
    }
    if (_renderBounds.onScreen()) {
		if (_accumVertices->count() + _vertices->count() > 65536) {
			render();
		}

        GeometryInstance* instance = ready(image, &_renderBounds);
        
        if (!instance->dynamicSources) {
            instance->dynamicSources = new CSArray<DynamicGeometrySource>();
        }
        bool append = true;
        if (instance->dynamicSources->count()) {
            const DynamicGeometrySource& lastSource = instance->dynamicSources->lastObject();
            if (lastSource.mode == mode) {
                switch (mode) {
                    case GL_TRIANGLES:
                    case GL_LINES:
                    case GL_POINTS:
                        append = false;
                        break;
                }
            }
        }
        if (append) {
            new (&instance->dynamicSources->addObject()) DynamicGeometrySource(mode);
        }
        DynamicGeometrySource& source = instance->dynamicSources->lastObject();
        foreach(ushort, index, _indices) {
            source.indices->addObject(index + _accumVertices->count());
        }
        _accumVertices->addObjectsFromArray(_vertices);
		_accumIndexCount += _indices->count();

        if (_target->usingDepthStencil() && _state->geometry.var.stencilMode != CSStencilNone) {
            State* state = _state;
            while (!state->usingStencil) {
                state = state->prev;
                break;
            }
            state->stencilBounds.append(_renderBounds);
        }
#ifdef CS_DIAGNOSTICS
		CSThread::mainThread()->diagnostics()->addDraw(_indices->count());
#endif
    }
	_renderBounds = CSRenderBounds::None;
	_vertices->removeAllObjects();
	_indices->removeAllObjects();
    _began = false;
}

void CSGraphics::addVertex(const CSVertex& vertex) {
	CSVertex& v = _vertices->addObject() = vertex;
	v.position.transformCoordinate(_state->world);
    
    const CSMatrix& viewProjection = _state->geometry.camera.viewProjection();
	float x = (v.position.x * viewProjection.m11) + (v.position.y * viewProjection.m21) + (v.position.z * viewProjection.m31) + viewProjection.m41;
	float y = (v.position.x * viewProjection.m12) + (v.position.y * viewProjection.m22) + (v.position.z * viewProjection.m32) + viewProjection.m42;
	float w = 1.0f / ((v.position.x * viewProjection.m14) + (v.position.y * viewProjection.m24) + (v.position.z * viewProjection.m34) + viewProjection.m44);
	if (w > 0)
	{
		x *= w;
		y *= w;
	}
	else
	{
		if (x < 0) x = -2;
		else if (x > 0) x = 2;
		if (y < 0) y = -2;
		else if (y > 0) y = 2;
	}
    
    _renderBounds.append(x, y);
    
	v.color *= _state->color;
	if (_state->geometry.var.usingLight) {
		v.normal.transformNormal(_state->world);
	}
}

void CSGraphics::addIndex(uint index) {
	CSAssert(index < 65536, "invalid operation");
    _indices->addObject(index);
}

void CSGraphics::applyAlign(CSVector3& point, const CSSize& size, CSAlign align) {
    if (align & CSAlignCenter) {
        point.x -= size.width * 0.5f;
    }
    else if (align & CSAlignRight) {
        point.x -= size.width;
    }
    if (align & CSAlignMiddle) {
        point.y -= size.height * 0.5f;
    }
    else if (align & CSAlignBottom) {
        point.y -= size.height;
    }
}

void CSGraphics::applyAlign(CSVector2& point, const CSSize& size, CSAlign align) {
    if (align & CSAlignCenter) {
        point.x -= size.width * 0.5f;
    }
    else if (align & CSAlignRight) {
        point.x -= size.width;
    }
    if (align & CSAlignMiddle) {
        point.y -= size.height * 0.5f;
    }
    else if (align & CSAlignBottom) {
        point.y -= size.height;
    }
}

//mesh

void CSGraphics::drawVertices(const CSRootImage* image, const CSVertexArray* arr, uint indexOffset, uint indexCount, uint mode) {
    GeometryInstance* instance = ready(image, NULL);
    if (!instance->staticSources) {
        instance->staticSources = new CSArray<StaticGeometrySource>();
    }
    new (&instance->staticSources->addObject()) StaticGeometrySource(arr, indexOffset, indexCount, mode, _state->world, _state->color);
    
    if (_target->usingDepthStencil() && _state->geometry.var.stencilMode != CSStencilNone) {
        State* state = _state;
        while (!state->usingStencil) {
            state = state->prev;
            break;
        }
        state->stencilBounds = CSRenderBounds::Full;
    }
#ifdef CS_DIAGNOSTICS
	CSThread::mainThread()->diagnostics()->addDraw(indexCount);
#endif
}

//grab

CSRootImage* CSGraphics::grab() {
    render();
    
    int width = _target->width();
    int height = _target->height();
    
    CSRootImage* image = CSRootImage::imageWithFormat(CSImageFormatRawRGBA8888, width, height);
    
    if (image) {
        glBindTextureCS(GL_TEXTURE_2D, image->texture0());
        glCopyTexImage2DCS(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
    }
    
    return image;
}

CSRootImage* CSGraphics::grab(const CSRect& rect) {
    render();
    
    int x = rect.origin.x;
    int y = rect.origin.y;
    int width = CSMath::ceil(rect.size.width);
    int height = CSMath::ceil(rect.size.height);
    
    CSRootImage* image = CSRootImage::imageWithFormat(CSImageFormatRawRGBA8888, width, height);
    
    if (image) {
        glBindTextureCS(GL_TEXTURE_2D, image->texture0());
        glCopyTexSubImage2DCS(GL_TEXTURE_2D, 0, 0, 0, x, y, width, height);
    }
    
    return image;
}



