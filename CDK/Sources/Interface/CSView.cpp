//
//  CSView.cpp
//  CDK
//
//  Created by chan on 13. 4. 10..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSView.h"

#include "CSViewBridge.h"

#include "CSTime.h"

#include "CSThread.h"

#include "CSOpenGL.h"

#include "CSPopupCoverLayer.h"

CSView::CSView(void* handle) {
    _handle = handle;
    _layers = new CSArray<CSLayer, 1, false>();
    _touches = new CSDictionary<uint, CSTouch>();
    _touchLayers = new CSDictionary<uint, CSArray<CSLayer>>();
    _projection.znear = 1;
    _projection.zfar = 10000;
    _popupDarkness = 0.5f;
    _enabled = true;
    
    _task = CSTask::task(CSDelegate0<void>::delegate(this, &CSView::timeout), 0, CSTaskActivityRepeat);
    CSThread::mainThread()->addTask(_task);
    
    CSApplication::sharedApplication()->OnPause.add(this, &CSView::notifyPause);
    CSApplication::sharedApplication()->OnResume.add(this, &CSView::notifyResume);
    
#ifdef CS_DISPLAY_COUNTER
    _lastFrameElapsed = CSTime::currentTime();
#endif
}

CSView::~CSView() {
    _layers->release();
    _touches->release();
    _touchLayers->release();
    
    _task->stop();    

    CSApplication::sharedApplication()->OnPause.remove(this);
    CSApplication::sharedApplication()->OnResume.remove(this);
}

void CSView::setProjection(float fov, float width, float height, float znear, float zfar) {
    _projection.fov = fov;
    _projection.width = width;
    _projection.height = height;
    _projection.znear = znear;
    _projection.zfar = zfar;

    foreach(CSLayer*, layer, _layers) {
        layer->notifyProjectionChanged();
    }
    refresh();
}

float CSView::projectionFov() const {
    return _projection.fov;
}
float CSView::projectionWidth() const {
    return _projection.width ? _projection.width : CSViewBridge::bufferWidth(_handle);
}
float CSView::projectionHeight() const {
    return _projection.height ? _projection.height : CSViewBridge::bufferHeight(_handle);
}
float CSView::projectionZNear() const {
    return _projection.znear;
}
float CSView::projectionZFar() const {
    return _projection.zfar;
}

void CSView::touchesBegan(const CSPlatformTouch* pts, uint count) {
    if (!_enabled) {
        return;
    }
	for (int i = 0; i < count; i++) {
		const CSPlatformTouch& pt = pts[i];
		CSTouch* t = CSTouch::touch(pt.key, pt.button, CSVector2(pt.x, pt.y));
		_touches->setObject(pt.key, t);
		if (!_touchLayers->containsKey(pt.key)) {
			CSArray<CSLayer>* touchLayers = new CSArray<CSLayer>(1);
			_touchLayers->setObject(pt.key, touchLayers);
			touchLayers->release();
		}
	}
	bool interrupt = false;
	{
		const CSArray<CSTouch>* touches = _touches->allObjects();
		foreach(CSLayer*, layer, _layers) {
			layer->notifyTouchesView(touches, interrupt);
			if (interrupt) return;
		}
	}
    foreach(CSArray<CSLayer>*, layers, _touchLayers->allObjects()) {
        foreach(CSLayer*, layer, layers) {
            if (!layer->touchMulti()) return;
        }
    }
    CSLayer* topLayer = NULL;
    for (int i = _layers->count() - 1; i >= 0; i--) {
        CSLayer* layer = _layers->objectAtIndex(i);
        if (layer->enabled()) {
            topLayer = layer;
            break;
        }
    }
    if (topLayer) {
		CSArray<CSLayer>* layers = new CSArray<CSLayer>();

		for (int i = 0; i < count && !interrupt; i++) {
			uint64 key = pts[i].key;

			CSTouch* t = _touches->objectForKey(key);

			CSArray<CSLayer>* touchLayers = _touchLayers->objectForKey(key);
                
            if (topLayer->layersOnTouch(t, touchLayers, true)) {
                foreach(CSLayer *, layer, touchLayers) {
                    if (!layers->containsObject(layer)) {
                        layers->addObject(layer);
                            
                        if (!layer->touchMulti()) {
							interrupt = true;
                        }
                    }
                }
            }
        }
        if (layers->count()) {
            foreach(CSLayer *, layer, layers) {
                layer->commitTouchesBegan();
            }
        }
        else if (topLayer->state() == CSLayerStateFocus && topLayer->enabled()) {
            cancelTouches(topLayer);
            topLayer->commitTouchesCancelled();
        }
        layers->release();
    }
#ifdef CS_DISPLAY_LAYER_FRAME
    CSVector2 p(touches[0].x, touches[0].y);
    convertToLocalSpace(&p);
    if (p.x < 100 && p.y < 100) {
        _displayLayerFrame = !_displayLayerFrame;
        refresh();
    }
#endif
}

void CSView::touchesMoved(const CSPlatformTouch* pts, uint count) {
    if (!_enabled) {
        return;
    }
    CSArray<CSLayer>* layers = new CSArray<CSLayer>();
    
    for (int i = 0; i < count; i++) {
        const CSPlatformTouch& pt = pts[i];
        
        CSVector2 p(pt.x, pt.y);
        
        CSTouch* t = _touches->objectForKey(pt.key);
        
        if (t) {
            if (t->state() != CSTouchStateMoved) {
                CSVector2 fcp = t->firstPoint();
				convertToLocalSpace(&fcp);
				CSVector2 ccp = p;
				convertToLocalSpace(&ccp);

                if (fcp.distanceSquared(ccp) <= _touchSensitivity * _touchSensitivity) {
                    continue;
                }
            }
            t->setPoint(p);
            
            foreach(CSLayer *, layer, _touchLayers->objectForKey(pt.key)) {
                if (!layers->containsObject(layer)) {
                    layers->addObject(layer);
                }
            }
        }
    }
    
    foreach(CSLayer *, layer, layers) {
        if (!layer->commitTouchesMoved()) {
            cancelTouches(layer);
            
            layer->commitTouchesCancelled();
        }
    }
    
    layers->removeAllObjects();
    
    CSLayer* topLayer = _layers->lastObject();
    
    if (topLayer) {
        CSArray<CSLayer>* layersOnTouch = new CSArray<CSLayer>();
        
        for (uint i = 0; i < count; i++) {
            const CSPlatformTouch& pt = pts[i];
            
            CSTouch* t = _touches->objectForKey(pt.key);
            
            if (t) {
                layersOnTouch->removeAllObjects();
                
                if (topLayer->layersOnTouch(t, layersOnTouch, false)) {
                    foreach(CSLayer *, layer, layersOnTouch) {
                        if (!layers->containsObject(layer)) {
                            layers->addObject(layer);
                        }
                    }
                    
                    CSArray<CSLayer>* touchLayers = _touchLayers->objectForKey(pt.key);
                    
                    for (uint j = 0; j < layersOnTouch->count(); j++) {
                        touchLayers->insertObject(j, layersOnTouch->objectAtIndex(j));
                        j++;
                    }
                }
            }
        }
        
        layersOnTouch->release();
    }
    
    foreach(CSLayer *, layer, layers) {
        layer->commitTouchesBegan();
    }
    
    layers->release();
}

void CSView::touchesEnded(const CSPlatformTouch* pts, uint count) {
    if (!_enabled) {
        return;
    }
    CSArray<CSLayer>* layers = new CSArray<CSLayer>();
    
    for (uint i = 0; i < count; i++) {
        const CSPlatformTouch& pt = pts[i];
        
        CSTouch* t = _touches->objectForKey(pt.key);
        
        if (t) {
            t->end();
            
            foreach(CSLayer *, layer, _touchLayers->objectForKey(pt.key)) {
                if (!layers->containsObject(layer)) {
                    layers->addObject(layer);
                }
            }
            _touchLayers->removeObject(pt.key);
            _touches->removeObject(pt.key);
        }
    }
    
    foreach(CSLayer *, layer, layers) {
        layer->commitTouchesEnded();
    }
    layers->release();
}

void CSView::touchesCancelled(const CSPlatformTouch* touches, uint count) {
    if (!_enabled) {
        return;
    }
    CSArray<CSLayer>* layers = new CSArray<CSLayer>();
    
    foreach(CSArray<CSLayer>*, layersOnTouch, _touchLayers->allObjects()) {
        foreach(CSLayer *, layer, layersOnTouch) {
            if (!layers->containsObject(layer)) {
                layers->addObject(layer);
            }
        }
    }
    
    foreach(CSLayer *, layer, layers) {
        layer->commitTouchesCancelled();
    }
    
    layers->release();
    
    _touches->removeAllObjects();
    _touchLayers->removeAllObjects();
}

bool CSView::beginTouch(CSLayer* layer, const CSTouch* touch) {
    CSArray<CSLayer>* touchLayers = _touchLayers->objectForKey(touch->key());
    
    if (!touchLayers->containsObject(layer)) {
        touchLayers->addObject(layer);
        return true;
    }
    return false;
}

void CSView::cancelTouches(CSLayer* layer) {
    foreach(const CSTouch *, touch, layer->touches()) {
        CSArray<CSLayer>* layers = _touchLayers->objectForKey(touch->key());
        if (layers) {
            layers->removeObjectIdenticalTo(layer);
        }
    }
}

void CSView::wheel(float offset) {
	CSLayer* layer = _layers->lastObject();

	if (layer && layer->state() == CSLayerStateFocus) {
		layer->onWheel(offset);
	}
}

void CSView::keyDown(int keyCode) {
	CSLayer* layer = _layers->lastObject();

	if (layer && layer->state() == CSLayerStateFocus) {
		layer->onKeyDown(keyCode);
	}
}

void CSView::keyUp(int keyCode) {
	CSLayer* layer = _layers->lastObject();

	if (layer && layer->state() == CSLayerStateFocus) {
		layer->onKeyUp(keyCode);
	}
}

void CSView::timeout() {
#ifdef CS_DISPLAY_COUNTER
    double current = CSTime::currentTime();
    if (current - _lastFrameElapsed >= 1.0f) {
        _lastFrameElapsed = current;
        _lastFrameCount = _frameCount;
        _frameCount = 0;
        _refresh = true;
    }
    else {
        _frameCount++;
    }
#endif
    CSLayerVisible visible = CSLayerVisibleForward;
    
    int i = _layers->count() - 1;
    while (i >= 0) {
        CSLayer* layer = _layers->objectAtIndex(i);
        
        bool end = layer->timeout(visible);
        
        i = _layers->indexOfObjectIdenticalTo(layer);
        
        if (end) {
            _layers->removeObjectAtIndex(i);
        }
        else if (layer->covered()) {
            visible = CSLayerVisibleCovered;
        }
        else if (visible == CSLayerVisibleForward && layer->enabled()) {
            visible = CSLayerVisibleBackground;
        }
        i--;
    }

	CSViewBridge::render(_handle, _refresh);
    _refresh = false;
}

void CSView::backKey() {
    CSLayer* layer = _layers->lastObject();
    
    if (layer && layer->state() == CSLayerStateFocus) {
        layer->onBackKey();
    }
}

int CSView::layerCount() const {
    int layerCount = 0;
    foreach(CSLayer*, layer, _layers) {
        layerCount += layer->layerCount();
    }
    return layerCount;
}

void CSView::draw(CSGraphics* graphics) {
    graphics->reset();
    graphics->clear(CSColor::Black);
    graphics->camera().setFov(_projection.fov);
    if (_projection.width) graphics->camera().setWidth(_projection.width);
    if (_projection.height) graphics->camera().setHeight(_projection.height);
    graphics->camera().setZNear(_projection.znear);
    graphics->camera().setZFar(_projection.zfar);
    graphics->camera().setPosition(CSVector3(0, 0, graphics->camera().defaultDistance()));
    
    graphics->translate(CSVector3(-graphics->camera().width() * 0.5f, -graphics->camera().height() * 0.5f, 0.0f));
    
    graphics->push();
    int i;
    for (i = _layers->count() - 1; i > 0 && !_layers->objectAtIndex(i)->covered(); i--);
    if (i > 0) {
		if (_layers->objectAtIndex(i)->transitionBackward(graphics)) {
			int j;
			for (j = i - 1; j > 0 && !_layers->objectAtIndex(j)->covered(); j--);
			for (; j < i; j++) {
				_layers->objectAtIndex(j)->draw(graphics);
			}
		}
		graphics->reset();
	}
    for (; i < _layers->count(); i++) {
        _layers->objectAtIndex(i)->draw(graphics);
    }
    graphics->pop();
#ifdef CS_DISPLAY_COUNTER
    {
        const char* version = *CSApplication::sharedApplication()->version();
        
#ifdef DEBUG
        static const char* prefix = "DBG";
#else
        static const char* prefix = "REL";
#endif
        const char* msg = CSString::cstringWithFormat("%s.%s FPS.%d Draw.%d Layer:%d / %d", prefix, version, _lastFrameCount, glGetDrawCallCS(), _layers->count(), layerCount());
        graphics->setColor(CSColor::Black);
        graphics->drawString(msg, CSVector2(6, 6));
        graphics->setColor(CSColor::White);
        graphics->drawString(msg, CSVector2(5, 5));
    }
#endif

    CSAssert(graphics->pushCount() == 0, "invalid state");
}

CSRect CSView::frame() const {
    return CSViewBridge::frame(_handle);
}

CSRect CSView::bounds() const {
    return CSViewBridge::bounds(_handle);
}

CSEdgeInsets CSView::edgeInsets() const {
    return CSViewBridge::edgeInsets(_handle);
}

void CSView::notifyPause() {
    foreach(CSLayer *, layer, _layers) {
        layer->notifyPause();
    }
}
void CSView::notifyResume() {
    foreach(CSLayer *, layer, _layers) {
        layer->notifyResume();
    }
}

void CSView::clearLayers(bool transition) {
    foreach(CSLayer *, layer, _layers) {
        layer->removeFromParent(transition);
    }
}

bool CSView::addLayer(CSLayer* layer) {
    int i = _layers->count() - 1;
    while (i >= 0) {
        if (layer->order() < _layers->objectAtIndex(i)->order()) {
            i--;
        }
        else {
            break;
        }
    }
    return insertLayer(i + 1,  layer);
}

bool CSView::insertLayer(uint index, CSLayer* layer) {
    if (layer->attach(this)) {
        _layers->insertObject(index, layer);
        layer->onStateChanged();
        _refresh = true;
        
        layer->notifyLink();
        
        return true;
    }
    return false;
}

void CSView::clearLayers(int key, bool transition, bool inclusive, bool masking) {
    foreach(CSLayer *, layer, _layers) {
        if (((masking ? (layer->key() & key) : layer->key()) == key) == inclusive) {
            layer->removeFromParent(transition);
        }
        else {
            layer->clearLayers(key, transition, inclusive, masking);
        }
    }
}

const CSArray<CSLayer, 1, false>* CSView::findLayers(int key, bool masking) {
    CSArray<CSLayer, 1, false>* layers = CSArray<CSLayer, 1, false>::array();
    findLayers(key, layers, masking);
    return layers;
}

void CSView::findLayers(int key, CSArray<CSLayer, 1, false>* outArray, bool masking) {
    foreach(CSLayer *, layer, _layers) {
        if ((masking ? (layer->key() & key) : layer->key()) == key) {
            outArray->addObject(layer);
        }
        layer->findLayers(key, outArray, masking);
    }
}

CSLayer* CSView::findLayer(int key, bool masking) {
    foreach(CSLayer *, layer, _layers) {
        if ((masking ? (layer->key() & key) : layer->key()) == key) {
            return layer;
        }
        else {
            CSLayer* rtnLayer = layer->findLayer(key, masking);
            if (rtnLayer) {
                return rtnLayer;
            }
        }
    }
    return NULL;
}

CSLayer* CSView::findParent(int key, bool masking) {
    return NULL;
}

void CSView::insertLayerAsPopup(uint index, CSLayer* layer, float darkness, float blur) {
    if (!layer->parent()) {
        insertLayer(index, CSPopupCoverLayer::layer(layer, darkness, blur));
    }
}

void CSView::addLayerAsPopup(CSLayer* layer, float darkness, float blur) {
    if (!layer->parent()) {
        addLayer(CSPopupCoverLayer::layer(layer, darkness, blur));
    }
}

CSRect CSView::convertToUIFrame(CSRect frame) const {
    CSSize size = bounds().size;
    
    float xrate = size.width / projectionWidth();
    float yrate = size.height / projectionHeight();
    
    frame.origin.x *= xrate;
    frame.origin.y *= yrate;
    frame.size.width *= xrate;
    frame.size.height *= yrate;
    
    return frame;
}

void CSView::convertToLocalSpace(CSVector2* point) const {
    CSSize size = bounds().size;
    
    point->x *= projectionWidth() / size.width;
    point->y *= projectionHeight() / size.height;
}

void CSView::setEnabled(bool enabled) {
    _enabled = enabled;
    if (!enabled) {
        foreach(CSArray<CSLayer>*, layersOnTouch, _touchLayers->allObjects()) {
            foreach(CSLayer *, layer, layersOnTouch) {
                layer->cancelTouches();
            }
        }
    }
}

void CSView::setKeyboardHeight(float height) {
    _keyboardHeight = height * projectionHeight() / bounds().size.height;
}

bool CSView::customEvent(int op, void* param) {
    for (int i = _layers->count() - 1; i >= 0; i--) {
        CSLayer* layer = _layers->objectAtIndex(i);
        
        if (layer->customEvent(op, param)) {
            return true;
        }
    }
    return false;
}

bool CSView::screenshot(const char* path) const {
    return CSViewBridge::screenshot(_handle, path);
}
