//
//  CSLayer.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLayer.h"

#include "CSView.h"

#include "CSLayerTransition.h"

#include "CSTime.h"

const uint CSLayerTransitionMoveMask =  CSLayerTransitionLeft |
                                        CSLayerTransitionRight|
                                        CSLayerTransitionUp|
                                        CSLayerTransitionDown|
                                        CSLayerTransitionLeftBounce |
                                        CSLayerTransitionRightBounce|
                                        CSLayerTransitionUpBounce|
                                        CSLayerTransitionDownBounce|
                                        CSLayerTransitionHorizontalFlip|
                                        CSLayerTransitionVerticalFlip|
                                        CSLayerTransitionZoom|
                                        CSLayerTransitionFall|
                                        CSLayerTransitionPop;

void CSLayer::DrawHandler::operator()(CSLayer* layer, CSGraphics* graphics) {
    if (_delegates) {
		if (_delegates->count() == 1) {
			(*_delegates->objectAtIndex(0))(layer, graphics);
		}
		else if (_delegates->count() > 1) {
			for (int i = 0; i < _delegates->count(); i++) {
				(*_delegates->objectAtIndex(i))(layer, graphics);
				if (i + 1 < _delegates->count()) {
					graphics->reset();
				}
			}
		}
    }
}

CSLayer::CSLayer() {
    _touches = new CSArray<const CSTouch>();
    _layers = new CSArray<CSLayer, 1, false>();
    
    _transitionDuration = 0.25f;
    _transitionAccelation = 1.0f;
    _transitionWeight = 1.0f;

    _timeoutInterval = 1.0f / 60.0f;
    _timeoutFixedRate = true;

    _enabled = true;

    _touchCarry = true;
    
    _color = CSColor::White;
}

CSLayer::~CSLayer() {
    foreach(CSLayer*, sublayer, _layers) {
        sublayer->_parent = NULL;           //parent가 제거되면 하위 레이어의 parent가 쓰레기로 남을 수 있음
    }
    _layers->release();
    _touches->release();
    release(_tag);
	release(_subtag);
    if (_links) {
        foreach(CSLayer*, link, _links) {
            link->removeFromParent(false);
        }
        _links->release();
    }
}

void CSLayer::setFrame(const CSRect& frame) {
    _nextFrame = frame;
    _prevFrame = frame;
    _frameInterval = 0.0f;
	if (frame != _frame) {
		submitFrame(frame, false);
    }
}

void CSLayer::setFrame(const CSRect& frame, float interval) {
	if (interval > 0) {
		if (frame != _nextFrame) {
			_prevFrame = _frame;
			_nextFrame = frame;
			_frameUpdated = CSTime::currentTime();
			_frameInterval = interval;
		}
	}
	else setFrame(frame);
}

float CSLayer::contentWidth() const {
    float max = _contentWidth;
    
    foreach(CSLayer *, layer, _layers) {
        float right = layer->_frame.right();
        if (max < right) {
            max = right;
        }
    }
    return max;
}

float CSLayer::contentHeight() const {
    float max = _contentHeight;
    
    foreach(CSLayer *, layer, _layers) {
        float bottom = layer->_frame.bottom();
        if (max < bottom) {
            max = bottom;
        }
    }
    return max;
}

void CSLayer::submitFrame(const CSRect& frame, bool fromParent) {
    _frame = frame;
    notifyFrameChanged();
    refresh();
    
    submitLayout();
    if (!fromParent && _parent) {
        _parent->submitLayout();
    }
}

void CSLayer::onStateChanged() {
    OnStateChanged(this);
}

bool CSLayer::attach(CSLayerContainer* parent) {
    if (_parent == parent) {
        if (_state == CSLayerStateDetach || _state == CSLayerStateRemoved) {
            _state = CSLayerStateAttach;
            onStateChanged();
        }
    }
    else if (!_parent) {
        _transitionProgress = _transition ? 0.0f : 1.0f;
        _state = CSLayerStateAttach;
        _parent = parent;
        return true;
    }
    return false;
}

void CSLayer::detach(CSLayer* layer) {
}

void CSLayer::onTimeout() {
    OnTimeout(this);
}

void CSLayer::beginTouch(const CSTouch* touch, bool event) {
    CSView* view = this->view();
    
    if (view && view->beginTouch(this, touch)) {
        _touches->addObject(touch);
        if (event) commitTouchesBegan();
    }
}

void CSLayer::cancelTouches(bool event) {
    if (_touches->count()) {
        CSView* view = this->view();
        if (view) {
            view->cancelTouches(this);
        }
        if (event) commitTouchesCancelled();
        else _touches->removeAllObjects();
    }
}

void CSLayer::cancelChildrenTouches(bool event) {
    foreach(CSLayer *, layer, _layers) {
        layer->cancelTouches(event);
        layer->cancelChildrenTouches(event);
    }
}

bool CSLayer::timeout(CSLayerVisible visible) {
    double current = CSTime::currentTime();
    
    switch (_state) {
        case CSLayerStateHidden:
        case CSLayerStateFocus:
            if (visible == CSLayerVisibleForward) {
                if (_state == CSLayerStateHidden) {
                    _state = CSLayerStateFocus;
                    onStateChanged();
                }
            }
            else if (_state == CSLayerStateFocus) {
                cancelTouches();
                cancelChildrenTouches();
                _state = CSLayerStateHidden;
                onStateChanged();
            }
            
            if (visible) {
                while (_timeoutUpdated + _timeoutInterval <= current) {
                    _timeoutUpdated += _timeoutInterval;
                    
                    _timeoutSequence++;
                    
                    onTimeout();
                    
                    bool looping;
                    
                    switch (_state) {
                        case CSLayerStateDetach:
                            visible = CSLayerVisibleBackground;
                            looping = false;
                            break;
                        case CSLayerStateRemoved:
                            visible = CSLayerVisibleCovered;
                            looping = false;
                            break;
                        default:
                            if (CSTime::currentTime() - current >= _timeoutInterval) {
                                _timeoutUpdated = current;
                                looping = false;
                            }
                            else {
                                looping = _timeoutFixedRate;
                            }
                            break;
                    }
                    if (!looping) {
                        break;
                    }
                }
            }
            break;
        case CSLayerStateDetach:
            if (visible) {
                if (_timeoutUpdated + _transitionLatency + _transitionDuration <= current) {
                    _transitionProgress = 0.0f;
                    _state = CSLayerStateRemoved;
                    onStateChanged();
                    
                    //notifyUnlink();
                    
                    visible = CSLayerVisibleCovered;
                }
                else {
                    _transitionProgress = powf(fminf(1.0f - (current - _timeoutUpdated - _transitionLatency) / _transitionDuration, 1.0f), _transitionAccelation);
                    
                    visible = CSLayerVisibleBackground;
                }
                refresh();
            }
            else {
                _transitionProgress = 0.0f;
                _state = CSLayerStateRemoved;
                onStateChanged();
                
                //notifyUnlink();
            }
            break;
        case CSLayerStateAttach:
            if (visible) {
                if (_timeoutUpdated + _transitionLatency + _transitionDuration <= current) {
                    _transitionProgress = 1.0f;
                    _state = visible == CSLayerVisibleForward ? CSLayerStateFocus : CSLayerStateHidden;
                    onStateChanged();
                    
                    _timeoutUpdated = current;
                }
                else {
                    _transitionProgress = powf(fmaxf((current - _timeoutUpdated - _transitionLatency) / _transitionDuration, 0.0f), _transitionAccelation);

                    visible = CSLayerVisibleBackground;
                }
                refresh();
            }
            else {
                _transitionProgress = 1.0f;
                _state = CSLayerStateHidden;
                onStateChanged();
                
                _timeoutUpdated = current;
            }
            break;
        case CSLayerStateRemoved:
            visible = CSLayerVisibleCovered;
            break;
    }
    
    bool performLayout = false;
    
    int i = _layers->count() - 1;
    while (i >= 0) {
        CSLayer* layer = _layers->objectAtIndex(i);
        
        bool end = layer->timeout(visible);
        
        i = _layers->indexOfObjectIdenticalTo(layer);
        
        if (end) {
            detach(layer);
            
            _layers->removeObjectAtIndex(i);
            
            performLayout = true;
        }
        else if (layer->covered()) {
            visible = CSLayerVisibleCovered;
        }
        i--;
    }
    
    if (_frameInterval) {
        if (visible) {
            float progress = (float)(current - _frameUpdated) / _frameInterval;
            
            if (progress > 0.0f) {
                if (progress < 1.0f) {
                    
                    float x = CSMath::lerp(_prevFrame.origin.x, _nextFrame.origin.x, progress);
                    float y = CSMath::lerp(_prevFrame.origin.y, _nextFrame.origin.y, progress);
                    float width = CSMath::lerp(_prevFrame.size.width, _nextFrame.size.width, progress);
                    float height = CSMath::lerp(_prevFrame.size.height, _nextFrame.size.height, progress);
                    
                    submitFrame(CSRect(x, y, width, height), false);
                    performLayout = false;
                }
                else {
                    submitFrame(_nextFrame, false);
                    performLayout = false;
                    
                    _frameInterval = 0.0f;
                }
            }
        }
        else {
            submitFrame(_nextFrame, false);
            performLayout = false;
            
            _frameInterval = 0.0f;
        }
    }
    else if (_keyboardScroll) {
        CSView* view = this->view();
        
        float keyboardHeight = view->keyboardHeight();
        
        if (keyboardHeight != _keyboardHeight) {
			CSRect frame;

			if (keyboardHeight) {
				float degree = keyboardScrollDegree(view->projectionHeight() - keyboardHeight);

				frame = _frame;
				frame.origin.y -= degree;
			
				if (_keyboardScroll == CSLayerKeyboardScrollFit) {
					float miny = CSMath::min(_nextFrame.origin.y, 0.0f);

					if (frame.origin.y < miny) {
						frame.size.height -= miny - frame.origin.y;
						frame.origin.y = miny;
					}
				}
			}
			else {
				frame = _nextFrame;
			}
			if (frame != _frame) {
				submitFrame(frame, false);
				performLayout = false;
			}
			_keyboardHeight = keyboardHeight;
        }
    }
    if (performLayout) {
        submitLayout();
    }
    
    validateLinks();
    
    if (_state == CSLayerStateRemoved && clearLinks()) {
        notifyUnlink();
        _parent = NULL;
        return true;
    }
    return false;
}

bool CSLayer::transitionForward(CSGraphics* graphics) {
    switch (_state) {
        case CSLayerStateRemoved:
            return false;
        case CSLayerStateAttach:
        case CSLayerStateDetach:
            if (_transition) {
                for (int i = 0; i < LAYER_TRANSITION_COUNT; i++) {
                    if ((_transition >> i & 1) && !transitionMethods[i].forward(this, graphics)) {
                        return false;
                    }
                }
            }
            break;
    }
    return true;
}

bool CSLayer::transitionBackward(CSGraphics* graphics) {
    switch (_state) {
        case CSLayerStateAttach:
        case CSLayerStateDetach:
            if (_transition) {
                for (int i = 0; i < LAYER_TRANSITION_COUNT; i++) {
                    if ((_transition >> i & 1) && !transitionMethods[i].backward(this, graphics)) {
						return false;
                    }
                }
                return true;
            }
            break;
    }
    return false;
}

void CSLayer::onDraw(CSGraphics* graphics) {
    OnDraw(this, graphics);
}

void CSLayer::onDrawCover(CSGraphics* graphics) {
    OnDrawCover(this, graphics);
}

void CSLayer::clip(CSGraphics* graphics) {
    CSLayerContainer* current = this;
    bool transition = false;
    do {
        if ((current->transition() & CSLayerTransitionMoveMask) != 0) {
            float transitionProgress = current->transitionProgress();
            
            if (transitionProgress > 0 && transitionProgress < 1) {
                transition = true;
                break;
            }
        }
        current = current->parent();
    } while (current);
    
    if (transition) {
        graphics->setStencilMode(CSStencilInclusive);
        graphics->drawRect(bounds(), true);
        graphics->setStencilMode(CSStencilNone);
    }
    else {
        CSRect scissor = frame();
        _parent->convertToViewSpace(&scissor.origin);
        graphics->setScissor(scissor);
    }
}

void CSLayer::commitDraw(CSGraphics* graphics) {
	if (_color != CSColor::White) {
		graphics->setColor(_color);
		graphics->pushColor();
		onDraw(graphics);
		graphics->popColor();
	}
	else {
		onDraw(graphics);
	}
}

void CSLayer::commitDrawCover(CSGraphics* graphics) {
	if (_color != CSColor::White) {
		graphics->setColor(_color);
		graphics->pushColor();
		onDrawCover(graphics);
		graphics->popColor();
	}
	else {
		onDrawCover(graphics);
	}
}

void CSLayer::draw(CSGraphics* graphics) {
    graphics->push();
    graphics->translate(_frame.origin);

	if (transitionForward(graphics)) {
		graphics->push();

		commitDraw(graphics);
		
		graphics->reset();

		if (_layers->count()) {
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
		}

		commitDrawCover(graphics);

		graphics->pop();

#ifdef CS_DISPLAY_LAYER_FRAME
		if (view()->displayLayerFrame()) {
			graphics->setColor(CSColor::Magenta);
			switch (_touchArea) {
				case CSLayerTouchAreaFrame:
					graphics->drawRect(bounds(), false);
					break;
				case CSLayerTouchAreaRange:
					graphics->drawCircle(bounds(), false);
					break;
			}
        }
#endif
    }
    graphics->pop();
}

bool CSLayer::touchContains(const CSVector2& p) const {
	switch (_touchArea) {
		case CSLayerTouchAreaRange:
			{
				CSVector2 c = centerMiddle();
				CSVector2 d = _frame.origin + c - p;
				return (d.x * d.x) / (c.x * c.x) + (d.y * d.y) / (c.y * c.y) <= 1;
			}
	}
	return _frame.contains(p);
}

CSLayerTouchReturn CSLayer::layersOnTouch(const CSTouch* touch, CSArray<CSLayer>* targetLayers, bool began) {
    if (_state == CSLayerStateFocus && _enabled) {
        bool touching = false;
        
        for (int i = _layers->count() - 1; i >= 0; i--) {
            CSLayer* layer = _layers->objectAtIndex(i);
            
            CSLayerTouchReturn rtn = layer->layersOnTouch(touch, targetLayers, began);
            
            switch (rtn) {
                case CSLayerTouchReturnCarry:
                    if (_touchInherit) {
                        touching = true;
                        goto check;
                    }
                    if (!_touchCarry) {
                        rtn = CSLayerTouchReturnCatch;
                    }
                case CSLayerTouchReturnCatch:
                    return rtn;
            }
        }
        
check:
        if (touchContains(touch->point(_parent))) {
            if (began || (_touchIn && !_touches->containsObject(touch))) {
                _touches->addObject(touch);
                targetLayers->addObject(this);
                touching = true;
            }
        }
        if (touching) {
            return _touchCarry ? CSLayerTouchReturnCarry : CSLayerTouchReturnCatch;
        }
    }
    return CSLayerTouchReturnNone;
}

void CSLayer::onTouchesBegan() {
    OnTouchesBegan(this);
}

bool CSLayer::customEvent(int op, void* param) {
    for (int i = _layers->count() - 1; i >= 0; i--) {
        CSLayer* layer = _layers->objectAtIndex(i);
        
        if (layer->customEvent(op, param)) {
            return true;
        }
    }
    bool interrupt = false;
    onCustomEvent(op, param, interrupt);
    return interrupt;
}

void CSLayer::commitTouchesBegan() {
    if (_touches->count()) {
        onTouchesBegan();
        
        if (_touchRefresh) {
            refresh();
        }
    }
}

void CSLayer::onTouchesMoved() {
    OnTouchesMoved(this);
}

bool CSLayer::commitTouchesMoved() {
    if (_touches->count()) {
        if (!_touchOut) {
            foreach(const CSTouch *, touch, _touches) {
                if (!touchContains(touch->point(_parent))) {
                    return false;
                }
            }
        }
        onTouchesMoved();
    }
    
    return true;
}

void CSLayer::onTouchesEnded() {
    OnTouchesEnded(this);
}

void CSLayer::commitTouchesEnded() {
    if (_touches->count()) {
        onTouchesEnded();
        
        if (_touchRefresh) {
            refresh();
        }
        int i = 0;
        while (i < _touches->count()) {
            const CSTouch* touch = _touches->objectAtIndex(i);
            
            if (touch->state() == CSTouchStateEnded) {
                _touches->removeObjectAtIndex(i);
            }
            else {
                i++;
            }
        }
    }
}

void CSLayer::onTouchesCancelled() {
    OnTouchesCancelled(this);
}

void CSLayer::commitTouchesCancelled() {
    _touches->removeAllObjects();
    
    onTouchesCancelled();

    if (_touchRefresh) {
        refresh();
    }
}

void CSLayer::onWheel(float offset) {
	OnWheel(this, offset);
}

void CSLayer::onKeyDown(int keyCode) {
	OnKeyDown(this, keyCode);
}

void CSLayer::onKeyUp(int keyCode) {
	OnKeyUp(this, keyCode);
}

void CSLayer::onLink() {
    OnLink(this);
}

void CSLayer::onUnlink() {
    OnUnlink(this);
}

void CSLayer::onFrameChanged() {
    OnFrameChanged(this);
}

void CSLayer::onProjectionChanged() {
    OnProjectionChanged(this);
}

void CSLayer::onTouchesView(const CSArray<CSTouch>* touches, bool& interrupt) {
    OnTouchesView(this, touches, interrupt);
}

void CSLayer::onPause() {
    OnPause(this);
}

void CSLayer::onResume() {
    OnResume(this);
}

void CSLayer::onBackKey() {
    OnBackKey(this);
}

void CSLayer::onCustomEvent(int op, void* param, bool& interrupt) {
    OnCustomEvent(this, op, param, interrupt);
}

void CSLayer::validateLinks() {
    if (_links) {
        int i = 0;
        while (i < _links->count()) {
            CSLayer* link = _links->objectAtIndex(i);
            if (link->view()) {
                i++;
            }
            else {
                _links->removeObjectAtIndex(i);
            }
        }
    }
}

bool CSLayer::clearLinks() {
    bool clear = true;
    
    if (_links && _links->count()) {
        foreach(CSLayer*, link, _links) {
            link->removeFromParent(false);
        }
        clear = false;
    }
    foreach(CSLayer*, layer, _layers) {
        if (!layer->clearLinks()) {
            clear = false;
        }
    }
    return clear;
}

void CSLayer::notifyLink() {
    _timeoutUpdated = CSTime::currentTime();
    
    onLink();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyLink();
    }
}

void CSLayer::notifyUnlink() {
    _color = CSColor::White;
    
    onUnlink();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyUnlink();
    }
}

void CSLayer::notifyFrameChanged() {
    onFrameChanged();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyFrameChanged();
    }
}

void CSLayer::notifyProjectionChanged() {
    onProjectionChanged();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyProjectionChanged();
    }
}

void CSLayer::notifyTouchesView(const CSArray<CSTouch>* touches, bool& interrupt) {
    onTouchesView(touches, interrupt);
	if (interrupt) return;
    foreach(CSLayer*, layer, _layers) {
        layer->notifyTouchesView(touches, interrupt);
		if (interrupt) return;
    }
}

void CSLayer::notifyPause() {
    onPause();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyPause();
    }
}
void CSLayer::notifyResume() {
    _timeoutUpdated = CSTime::currentTime();
    
    onResume();
    
    foreach(CSLayer*, layer, _layers) {
        layer->notifyResume();
    }
}

float CSLayer::keyboardScrollDegree(float bottom) const {
    float degree = 0;
    foreach(CSLayer*, layer, _layers) {
        float d = layer->keyboardScrollDegree(bottom);
        if (d > degree) {
            degree = d;
        }
    }
    return degree;
}

void CSLayer::submitLayout() {
    switch (_layout) {
        case CSLayerLayoutHorizontal:
            {
                float x = 0.0f;
                foreach(CSLayer *, layer, _layers) {
                    if (layer->state() != CSLayerStateRemoved) {
                        layer->submitFrame(CSRect(x, (_frame.size.height - layer->height()) / 2, layer->width(), layer->height()), true);
                        x += layer->width() + _spacing;
                    }
                }
            }
            break;
        case CSLayerLayoutVertical:
            {
                float y = 0.0f;
                foreach(CSLayer *, layer, _layers) {
                    if (layer->state() != CSLayerStateRemoved) {
                        layer->submitFrame(CSRect((_frame.size.width - layer->width()) / 2, y, layer->width(), layer->height()), true);
                        y += layer->height() + _spacing;
                    }
                }
            }
            break;
        case CSLayerLayoutFlow:
            {
                float x = 0.0f;
                float y = 0.0f;
                foreach(CSLayer *, layer, _layers) {
                    if (layer->state() != CSLayerStateRemoved) {
                        layer->submitFrame(CSRect(x, y, layer->width(), layer->height()), true);
                        x += layer->width() + _spacing;
                        if (x >= width()) {
                            y += layer->height() + _spacing;
                            x = 0.0f;
                        }
                    }
                }
            }
            break;
    }
}

void CSLayer::removeFromParent(bool transition) {
    if (_state == CSLayerStateDetach) {
        if (!transition) {
            _transitionProgress = 0.0f;
            _state = CSLayerStateRemoved;
            onStateChanged();
        }
    }
    else if (_state != CSLayerStateRemoved) {
        if (transition && _transition) {
            _timeoutUpdated = CSTime::currentTime();
            _transitionProgress = 1.0f;
            _state = CSLayerStateDetach;
            onStateChanged();
        }
        else {
            _transitionProgress = 0.0f;
            _state = CSLayerStateDetach;
            onStateChanged();
            if (_state == CSLayerStateDetach) {
                _state = CSLayerStateRemoved;
                onStateChanged();
            }
        }
        cancelTouches();
        cancelChildrenTouches();
    }
}

void CSLayer::addToParent() {
    if (_parent && (_state == CSLayerStateDetach || _state == CSLayerStateRemoved)) {
        _state = CSLayerStateAttach;
        onStateChanged();
    }
}

int CSLayer::layerCount() const {
    int layerCount = 1;
    foreach (CSLayer*, layer, _layers) {
        layerCount += layer->layerCount();
    }
    return layerCount;
}

/////////////////////////////////////////////////////////////////
// CSLayerContainer
/////////////////////////////////////////////////////////////////
void CSLayer::clearLayers(bool transition) {
    foreach(CSLayer *, layer, _layers) {
        layer->removeFromParent(transition);
    }
}

bool CSLayer::addLayer(CSLayer* layer) {
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

bool CSLayer::insertLayer(uint index, CSLayer* layer) {
    if (layer->attach(this)) {
        _layers->insertObject(index, layer);
        layer->onStateChanged();
        
        submitLayout();
        
        CSView* view = this->view();
        if (view) {
            view->refresh();
            
            layer->notifyLink();
        }
        return true;
    }
    return false;
}

void CSLayer::clearLayers(int key, bool transition, bool inclusive, bool masking) {
    if (((masking ? (_key & key) : _key) == key) == inclusive) {
        removeFromParent(transition);
    }
    else {
        foreach(CSLayer *, layer, _layers) {
            layer->clearLayers(key, transition, inclusive, masking);
        }
    }
}

CSArray<CSLayer, 1, false>* CSLayer::findLayers(int key, bool masking) {
    CSArray<CSLayer, 1, false>* layers = CSArray<CSLayer, 1, false>::array();
    findLayers(key, layers, masking);
    return layers;
}

void CSLayer::findLayers(int key, CSArray<CSLayer, 1, false>* outArray, bool masking) {
    if ((masking ? (_key & key) : _key) == key) {
        outArray->addObject(this);
    }
    else {
        foreach(CSLayer *, layer, _layers) {
            layer->findLayers(key, outArray, masking);
        }
    }
}

CSLayer* CSLayer::findLayer(int key, bool masking) {
    if ((masking ? (_key & key) : _key) == key) {
        return this;
    }
    else {
        foreach(CSLayer *, layer, _layers) {
            CSLayer* rtnLayer = layer->findLayer(key, masking);
            if (rtnLayer) {
                return rtnLayer;
            }
        }
    }
    return NULL;
}

CSLayer* CSLayer::findParent(int key, bool masking) {
    if ((masking ? (_key & key) : _key) == key) {
        return const_cast<CSLayer*>(this);
    }
    return _parent ? _parent->findParent(key) : NULL;
}

void CSLayer::convertToLocalSpace(CSVector2* point) const {
    if (_parent) {
        _parent->convertToLocalSpace(point);
    }
    point->x -= _frame.origin.x;
    point->y -= _frame.origin.y;
}

void CSLayer::convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const {
    if (this != parent) {
        point->x += _frame.origin.x;
        point->y += _frame.origin.y;
        if (_parent) {
            _parent->convertToParentSpace(point, parent);
        }
    }
}

void CSLayer::convertToParentSpace(CSVector2* point, int key, bool masking) const {
    if ((masking ? (_key & key) : _key) != key) {
        point->x += _frame.origin.x;
        point->y += _frame.origin.y;
        if (_parent) {
            _parent->convertToParentSpace(point, key, masking);
        }
    }
}

void CSLayer::convertToViewSpace(CSVector2* point) const {
    point->x += _frame.origin.x;
    point->y += _frame.origin.y;
    if (_parent) {
        _parent->convertToViewSpace(point);
    }
}

void CSLayer::setOwner(CSLayer* owner) {
    if (!owner->_links) owner->_links = new CSArray<CSLayer>(1);
    owner->_links->addObject(this);
}
