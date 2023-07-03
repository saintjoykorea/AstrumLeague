//
//  CSScroll.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 2. 2..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSScroll.h"

#include "CSMath.h"
#include "CSTime.h"

#define ScrollBarDuration           0.5f
#define ScrollBounceBackBoundary    0.5f
#define ScrollDragDecreaseRate      0.75f
#define ScrollReleaseDecreaseRate   0.95f
#define ScrollBounceBackRate        0.8f

CSScroll::CSScroll(CSScrollParent* parent) :
    _parent(parent),
    _current(CSVector2::Zero),
    _delta(CSVector2::Zero),
    _barRemaining(0),
    _barThickness(10),
    _barColor(CSColor::TranslucentBlack),
    _barHidden(true),
    _auto(NULL),
    _horizontalBounceBack(CSScrollBounceBackIfNeeded),
    _verticalBounceBack(CSScrollBounceBackIfNeeded)
{
    
}
CSScroll::~CSScroll() {
    if (_auto) {
        delete _auto;
        _auto = NULL;
    }
}

static bool adjust(float& current, float& delta, float content, float clip, float interval, bool dragging, CSScrollBounceBack bounceBack) {
    bool flag;
    if (bounceBack == CSScrollBounceBackNone) {
        flag = false;
    }
    else if (bounceBack == CSScrollBounceBackIfNeeded) {
        flag = content > clip;
    }
    else {
        flag = true;
    }
    
    float value = current;
    
    if (flag) {
        if (delta) {
            float boundary = content * ScrollBounceBackBoundary;
            
            float next = value + delta;
            
            if (next < 0) {
                value = delta < 0 ? (next < -boundary ? -boundary : value + delta * (1 + value / boundary)) : next;
                
                if (!dragging) {
                    delta = 0;
                }
            }
            else {
                float max = CSMath::max(content - clip, 0.0f);
                
                if (next > max) {
                    value = delta > 0 ? (next > max + boundary ? max + boundary : value + delta * ((max + boundary) - value) / boundary) : next;
                    
                    if (!dragging) {
                        delta = 0;
                    }
                }
                else {
                    value = next;
                }
            }
        }
        else if (!dragging) {
            if (value < 0) {
                value *= CSMath::pow(ScrollBounceBackRate, interval * 60);
                if (value > -1) {
                    value = 0;
                }
            }
            else {
                float max = CSMath::max(content - clip, 0.0f);
                
                if (value > max) {
                    value -= (value - max) * (1 - CSMath::pow(ScrollBounceBackRate, interval * 60));
                    if (value < max + 1) {
                        value = max;
                    }
                }
            }
        }
    }
    else {
        if (content <= clip) {
            value = 0;
            delta = 0;
        }
        else {
            float next = value + delta;
            
            if (next < 0) {
                value = 0;
                delta = 0;
            }
            else {
                float max = CSMath::max(content - clip, 0.0f);
                
                if (next > max) {
                    value = max;
                    delta = 0;
                }
                else {
                    value = next;
                }
            }
        }
    }
    if (current != value) {
        current = value;
        return true;
    }
    return false;
}

static bool jump(float& current, float value, float content, float clip, CSScrollBounceBack bounceBack) {
    if (value < 0 || content <= clip) {
        value = 0;
    }
    else if (value + clip > content) {
        value = content - clip;
    }
    if (current != value) {
        current = value;
        return true;
    }
    return false;
}

bool CSScroll::adjust(CSVector2& delta, float interval, bool dragging) {
    CSSize content = _parent->scrollContentSize();
    CSSize clip = _parent->scrollClipSize();
    
    bool refresh = false;
    if (::adjust(_current.x, delta.x, content.width, clip.width, interval, dragging, _horizontalBounceBack)) {
        refresh = true;
    }
    if (::adjust(_current.y, delta.y, content.height, clip.height, interval, dragging, _verticalBounceBack)) {
        refresh = true;
    }
    return refresh;
}

bool CSScroll::jump(const CSVector2& position) {
    CSSize content = _parent->scrollContentSize();
    CSSize clip = _parent->scrollClipSize();
    
    bool refresh = false;
    if (::jump(_current.x, position.x, content.width, clip.width, _horizontalBounceBack)) {
        refresh = true;
    }
    if (::jump(_current.y, position.y, content.height, clip.height, _verticalBounceBack)) {
        refresh = true;
    }
    return refresh;
}

void CSScroll::stop() {
    _delta = CSVector2::Zero;
    if (_auto) {
        delete _auto;
        _auto = NULL;
    }
}

void CSScroll::set(const CSVector2& position) {
    stop();

    if (jump(position)) {
        OnScroll(_parent);
    }
}

void CSScroll::set(const CSVector2& position, float duration) {
    const CSVector2& current = _auto ? _auto->next : _current;
    
    _delta = CSVector2::Zero;
    
    if (current != position) {
        if (!_auto) {
            _auto = new Auto();
        }
        _auto->prev = _current;
        _auto->next = position;
        _auto->duration = duration;
        _auto->progress = 0;
    }
    else {
        if (_auto) {
            delete _auto;
            _auto = NULL;
        }
    }
}

void CSScroll::drag(const CSVector2& delta) {
    if (!_auto) {
        if (delta) {
            _delta += delta;
            _barRemaining = ScrollBarDuration;
        }
        CSVector2 d = delta;
        if (adjust(d, 0, true)) {
            OnScroll(_parent);
        }
    }
}

void CSScroll::timeout(float interval, bool dragging) {
    bool refresh = false;
    
    if (_auto) {
        _auto->progress += interval;
        
        float progress = _auto->progress / _auto->duration;
        
        if (progress < 1) {
            CSVector2 value = CSVector2::lerp(_auto->prev, _auto->next, progress);
            
            if (jump(value)) {
                refresh = true;
            }
        }
        else {
            if (jump(_auto->next)) {
                refresh = true;
            }
            delete _auto;
            _auto = NULL;
        }
    }
    else{
        float deltaDecreaseRate;
        if (dragging) {
            deltaDecreaseRate = CSMath::pow(ScrollDragDecreaseRate, interval * 60);
        }
        else {
            if (adjust(_delta, interval, false)) {
                refresh = true;
            }
            deltaDecreaseRate = CSMath::pow(ScrollReleaseDecreaseRate, interval * 60);
        }
        _delta.x = (int)_delta.x ? _delta.x * deltaDecreaseRate : 0;
        _delta.y = (int)_delta.y ? _delta.y * deltaDecreaseRate : 0;
    }
    if (_barRemaining) {
        _barRemaining -= interval;
        if (_barRemaining > 0) {
            if (_barHidden) refresh = true;
        }
        else {
            _barRemaining = 0;
        }
    }
    if (refresh) {
        OnScroll(_parent);
    }
}

void CSScroll::draw(CSGraphics* graphics) {
    if (_barThickness && (_barRemaining || !_barHidden)) {
        CSSize content = _parent->scrollContentSize();
        CSSize clip = _parent->scrollClipSize();
        
        graphics->pushColor();
        if (_barHidden) {
            graphics->setColor(CSColor(_barColor.r, _barColor.g, _barColor.b, _barColor.a * _barRemaining / ScrollBarDuration));
        }
        else {
            graphics->setColor(_barColor);
        }
        if (content.width > clip.width) {
            float barWidth = clip.width * clip.width / content.width;
            float barX = _current.x * (clip.width - barWidth) / (content.width - clip.width);
            
            if (barX < 0) {
                barWidth += barX;
                barX = 0;
            }
            else if (barX + barWidth > clip.width) {
                barWidth = clip.width - barX;
            }
                
            graphics->drawRoundRect(CSRect(barX, clip.height - _barThickness, barWidth, _barThickness), _barThickness * 0.3f, true);
        }
        
        if (content.height > clip.height) {
            float barHeight = clip.height * clip.height / content.height;
            float barY = _current.y * (clip.height - barHeight) / (content.height - clip.height);
            
            if (barY < 0) {
                barHeight += barY;
                barY = 0;
            }
            else if (barY + barHeight > clip.height) {
                barHeight = clip.height - barY;
            }
            
            graphics->drawRoundRect(CSRect(clip.width - _barThickness, barY, _barThickness, barHeight), _barThickness * 0.3f, true);
        }
        graphics->popColor();
    }
}
