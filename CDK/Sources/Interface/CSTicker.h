//
//  CSTicker.h
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSTicker_h
#define CSTicker_h

#include "CSLayer.h"

class CSTicker : public CSLayer {
private:
    CSArray<CSString>* _texts;
    const CSFont* _font;
    CSColor _textColor;
    float _scroll;
    float _scrollSpeed;
    float _width;
    uint _position;
    CSColor _strokeColor;
    byte _strokeWidth;
    bool _finished;
    bool _paused;
public:
    CSTicker();
private:
    ~CSTicker();
public:
    inline const CSArray<CSString>* texts() const {
        return _texts;
    }
private:
    void resetWidth();
public:
    void rewind();
    void addText(const char* text);
    void clearText();
    
    inline bool isFinished() const {
        return _finished;
    }
    
    inline const CSFont* font() const {
        return _font;
    }
    void setFont(const CSFont* font);

    inline const CSColor& textColor() const {
        return _textColor;
    }
    void setTextColor(const CSColor& textColor);

    void setStrokeWidth(int width);
    inline int strokeWidth() const {
        return _strokeWidth;
    }

    void setStrokeColor(const CSColor& color);
    inline const CSColor& strokeColor() const {
        return _strokeColor;
    }

    inline void setScroll(float scroll) {
        _scroll = scroll;
    }
    inline float scroll() const {
        return _scroll;
    }
    inline void setScrollSpeed(float scrollSpeed) {
        _scrollSpeed = scrollSpeed;
    }
    inline float scrollSpeed() const {
        return _scrollSpeed;
    }
    inline void pause() {
        _paused = true;
    }
    inline void resume() {
        _paused = false;
    }
private:
    void onTimeout() override;
    void onDraw(CSGraphics* graphics) override;
};

#endif /* CSTicker_h */
