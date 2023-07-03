//
//  CSScrollPane.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 20..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSScrollPane.h"

#include "CSView.h"

#include "CSTime.h"

CSScrollPane::CSScrollPane() : scroll(this), _focusScroll(true) {
    scroll.OnScroll.add([this](CSScrollParent* parent) {
        refresh();
    });
    _touchInherit = true;
    _touchCarry = false;
    _touchOut = true;
}

void CSScrollPane::focusLayer(CSLayer* layer, float duration) {
    if (_layers->containsObject(layer)) {
        const CSRect& frame = layer->nextFrame();
        
        bool flag = false;
        
        CSVector2 position = scroll.position();
        
        if (position.x > frame.origin.x) {
            position.x = frame.origin.x;
            flag = true;
        }
        else if (position.x < frame.origin.x && position.x + _frame.size.width < frame.origin.x + frame.size.width) {
            position.x = CSMath::min(frame.origin.x, frame.origin.x + frame.size.width - _frame.size.width);
            flag = true;
        }
        if (position.y > frame.origin.y) {
            position.y = frame.origin.y;
            flag = true;
        }
        else if (position.y < frame.origin.y && position.y + _frame.size.height < frame.origin.y + frame.size.height) {
            position.y = CSMath::min(frame.origin.y, frame.origin.y + frame.size.height - _frame.size.height);
            flag = true;
        }
        if (flag) {
            if (duration) scroll.set(position, duration);
            else scroll.set(position);
        }
    }
}

void CSScrollPane::onTimeout() {
    scroll.timeout(timeoutInterval(), isTouching());
    
    CSLayer::onTimeout();
}

void CSScrollPane::onDrawContent(CSGraphics* graphics) {
	OnDrawContent(this, graphics);
}

void CSScrollPane::draw(CSGraphics* graphics) {
    graphics->push();
    graphics->translate(_frame.origin);
    
    if (transitionForward(graphics)) {
		graphics->push();

		commitDraw(graphics);
		graphics->reset();
		
		clip(graphics);
		const CSVector2& position = scroll.position();
		graphics->translate(-position);

		graphics->push();

		onDrawContent(graphics);
		graphics->reset();

		if (_layers->count()) {
			int i;

			for (i = _layers->count() - 1; i > 0 && !_layers->objectAtIndex(i)->covered(); i--);

			if (i > 0 && _layers->objectAtIndex(i)->transitionBackward(graphics)) {
				int j;
				for (j = i; j > 0 && !_layers->objectAtIndex(j)->covered(); j--);

				for (; j < i; j++) {
					CSLayer* layer = _layers->objectAtIndex(j);
					CSRect frame = layer->frame();
					frame.origin -= position;

					if (frame.right() > 0 && frame.left() < _frame.size.width && frame.bottom() > 0 && frame.top() < _frame.size.height) {
						layer->draw(graphics);
					}
				}
				graphics->reset();
			}

			for (; i < _layers->count(); i++) {
				CSLayer* layer = _layers->objectAtIndex(i);
				CSRect frame = layer->frame();
				frame.origin -= position;

				if (frame.right() > 0 && frame.left() < _frame.size.width && frame.bottom() > 0 && frame.top() < _frame.size.height) {
					layer->draw(graphics);
				}
			}
		}

        graphics->pop();

		graphics->reset();
            
        scroll.draw(graphics);
        
		commitDrawCover(graphics);

		graphics->pop();

#ifdef CS_DISPLAY_LAYER_FRAME
        if (view()->displayLayerFrame()) {
            graphics->setColor(CSColor::Magenta);
            graphics->drawRect(bounds(), false);
        }
#endif
    }
    graphics->pop();
}

CSLayerTouchReturn CSScrollPane::layersOnTouch(const CSTouch* touch, CSArray<CSLayer>* targetLayers, bool began) {
    if (_state == CSLayerStateFocus && _enabled) {
        CSVector2 point = touch->point(_parent);
        
        if (_frame.contains(point)) {
            bool touching = false;
            
            for (int i = _layers->count() - 1; i >= 0; i--) {
                CSLayer* layer = _layers->objectAtIndex(i);
                
                CSLayerTouchReturn rtn = layer->layersOnTouch(touch, targetLayers, began);
                
                switch (rtn) {
                    case CSLayerTouchReturnCarry:
                        if (_touchInherit) {
                            touching = true;
                            goto occur;
                        }
                    case CSLayerTouchReturnCatch:
                        return rtn;
                }
            }
		occur:
            if (began || (_touchIn && !_touches->containsObject(touch))) {
                _touches->addObject(touch);
                
                targetLayers->addObject(this);
                
                touching = true;
            }
            if (touching) {
                return _touchCarry ? CSLayerTouchReturnCarry : CSLayerTouchReturnCatch;
            }
        }
    }
    return CSLayerTouchReturnNone;
}

bool CSScrollPane::touchContains(const CSVector2& p) const {
	return _frame.contains(p);
}

void CSScrollPane::convertToLocalSpace(CSVector2* point) const {
    CSLayer::convertToLocalSpace(point);
    (*point) += scroll.position();
}

void CSScrollPane::convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const {
    if (this != parent) {
        (*point) += _frame.origin - scroll.position();
        _parent->convertToParentSpace(point, parent);
    }
}

void CSScrollPane::convertToParentSpace(CSVector2* point, int key, bool masking) const {
    if ((masking ? (_key & key) : _key) != key) {
        (*point) += _frame.origin - scroll.position();
        _parent->convertToParentSpace(point, key);
    }
}

void CSScrollPane::convertToViewSpace(CSVector2* point) const {
    (*point) += _frame.origin - scroll.position();
    _parent->convertToViewSpace(point);
}

void CSScrollPane::onTouchesMoved() {
    if (!_lockScroll) {
        const CSTouch* touch = _touches->objectAtIndex(0);
        CSVector2 p0 = touch->prevPoint(this);
        CSVector2 p1 = touch->point(this);
        
        scroll.drag(p0 - p1);
        
        if (_focusScroll) {
            cancelChildrenTouches();
        }
    }
    CSLayer::onTouchesMoved();
}

