//
//  CSTicker.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTicker.h"

#define DefaultScrollSpeed  120

CSTicker::CSTicker() {
    _texts = new CSArray<CSString>();
    _font = retain(CSGraphics::defaultFont());
    _textColor = CSColor::White;
    _strokeColor = CSColor::Black;
    _scrollSpeed = DefaultScrollSpeed;
    _finished = true;
}

CSTicker::~CSTicker() {
    _texts->release();
    _font->release();
}

void CSTicker::resetWidth() {
    _width = CSGraphics::stringSize(_texts->objectAtIndex(_position), _font).width;
}

void CSTicker::rewind() {
    _position = 0;
    _scroll = 0;
    if (_texts->count()) {
        resetWidth();
    }
    refresh();
}

void CSTicker::addText(const char* text) {
    CSString* str = new CSString(text);
    str->replace("\n", " ");
    _texts->addObject(str);
    
    _finished = false;
    if (_texts->count() == 1) {
        _scroll = 0;
        resetWidth();
    }
    str->release();
    
    refresh();
}

void CSTicker::clearText() {
    _texts->removeAllObjects();
    _position = 0;
    _scroll = 0;
    _width = 0;
    refresh();
}

void CSTicker::setFont(const CSFont* font) {
    if (retain(_font, font)) {
        if (_texts->count()) {
            resetWidth();
        }
        refresh();
    }
}

void CSTicker::setTextColor(const CSColor& textColor) {
    _textColor = textColor;
    refresh();
}


void CSTicker::setStrokeWidth(int width) {
    _strokeWidth = width;
    refresh();
}

void CSTicker::setStrokeColor(const CSColor& color) {
    _strokeColor = color;
    refresh();
}

void CSTicker::onTimeout() {
    CSLayer::onTimeout();
    
    if (!_paused && _texts->count()) {
        _scroll += _scrollSpeed * _timeoutInterval;
        if (_scroll > _width + _frame.size.width) {
            _scroll = 0;
            if (++_position >= _texts->count()) {
                _finished = true;
                _position = 0;
            }
            resetWidth();
        }
        refresh();
    }
}

void CSTicker::onDraw(CSGraphics* graphics) {
    CSLayer::onDraw(graphics);
    
    if (_texts->count()) {
        graphics->reset();
        
        const CSString* str = _texts->objectAtIndex(_position);

        clip(graphics);
        
        graphics->setColor(_textColor);
        graphics->setFont(_font);
        graphics->setStrokeWidth(_strokeWidth);
        graphics->setStrokeColor(_strokeColor);
        graphics->drawString(str, CSRect(_frame.size.width - _scroll, 0, CSStringWidthUnlimited, height()), CSAlignMiddle);
    }
}
