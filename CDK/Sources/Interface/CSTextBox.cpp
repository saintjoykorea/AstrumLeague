//
//  CSTextBox.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 21..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTextBox.h"

CSTextBox::CSTextBox() : scroll(this) {
    scroll.OnScroll.add([this](CSScrollParent* parent) {
        refresh();
    });
    _text = new CSString();
    _font = retain(CSGraphics::defaultFont());
    _textColor = CSColor::White;
	_strokeColor = CSColor::Black;
    _touchCarry = false;
}

CSTextBox::~CSTextBox() {
    _text->release();
    _font->release();
}

void CSTextBox::updateTextHeight() {
    _textHeight = CSGraphics::stringSize(_text, _font, _frame.size.width).height;
}

void CSTextBox::setFont(const CSFont* font) {
    if (retain(_font, font)) {
        updateTextHeight();
        refresh();
    }
}

void CSTextBox::autoScroll() {
    if (_textHeight > _frame.size.height && scroll.position().y < _textHeight - _frame.size.height) {
        scroll.set(CSVector2(0, _textHeight - _frame.size.height));
    }
}

void CSTextBox::setText(const char* str) {
    _text->set(str);
    updateTextHeight();
    refresh();
}

void CSTextBox::setTextColor(const CSColor& color) {
    _textColor = color;
    refresh();
}

void CSTextBox::setStrokeWidth(int width) {
	_strokeWidth = width;
	refresh();
}

void CSTextBox::setStrokeColor(const CSColor& color) {
	_strokeColor = color;
	refresh();
}

bool CSTextBox::shrinkText() {
    if (_maxHeight) {
        CSSize size = CSGraphics::stringSize(_text, _font, _frame.size.width);
        
        if (size.height > _maxHeight) {
            const CSString* str = CSGraphics::shrinkString(_text, _font, size.height - _maxHeight, _frame.size.width);
            _text->set(*str);
            return true;
        }
    }
    return false;
}

void CSTextBox::setMaxHeight(float maxHeight) {
    _maxHeight = maxHeight;
    if (shrinkText()) {
        updateTextHeight();
        
        refresh();
    }
}

void CSTextBox::clearText() {
    _text->set("");
    _textHeight = 0;
    refresh();
}

void CSTextBox::appendText(const char* str) {
    _text->append(str);
    _text->append("\n");
    shrinkText();
    updateTextHeight();
    
    if (!scroll.isScrolling() && scroll.position().y < _textHeight - height()) {
        scroll.set(CSVector2(0, _textHeight - height()));
    }
    refresh();
}

void CSTextBox::onTimeout() {
    scroll.timeout(timeoutInterval(), isTouching());
    
    CSLayer::onTimeout();
}

void CSTextBox::onDraw(CSGraphics* graphics) {
    CSLayer::onDraw(graphics);

    graphics->reset();

    clip(graphics);

    graphics->setColor(_textColor);
    graphics->setFont(_font);
	graphics->setStrokeWidth(_strokeWidth);
	graphics->setStrokeColor(_strokeColor);
    graphics->drawStringScrolled(_text, bounds(), scroll.position().y);
    
    graphics->setColor(CSColor::White);
	graphics->setStrokeWidth(0);
    scroll.draw(graphics);
}

void CSTextBox::onTouchesMoved() {
    if (!_lockScroll) {
        const CSTouch* touch = _touches->objectAtIndex(0);
        CSVector2 p0 = touch->prevPoint(this);
        CSVector2 p1 = touch->point(this);
        
        scroll.drag(p0 - p1);
    }
    CSLayer::onTouchesMoved();
}

