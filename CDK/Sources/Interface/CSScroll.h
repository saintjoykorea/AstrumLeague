//
//  CSScroll.h
//  CDK
//
//  Created by Kim Chan on 2018. 2. 2..
//  Copyright © 2018년 brgames. All rights reserved.
//
#ifndef __CDK__CSScroll__
#define __CDK__CSScroll__

#include "CSGraphics.h"

enum CSScrollBounceBack : byte {
    CSScrollBounceBackNone,
    CSScrollBounceBackIfNeeded,
    CSScrollBounceBackAlways
};

class CSScrollParent {
public:
    virtual CSSize scrollContentSize() const = 0;
    virtual CSSize scrollClipSize() const = 0;
};

class CSScroll {
public:
	CSHandler<CSScrollParent*> OnScroll;
private:
    CSScrollParent* _parent;
    CSVector2 _current;
    CSVector2 _delta;
    float _barRemaining;
    float _barThickness;
    CSColor _barColor;
    bool _barHidden;
    struct Auto {
        CSVector2 prev;
        CSVector2 next;
        float duration;
        float progress;
    };
    Auto* _auto;
    CSScrollBounceBack _horizontalBounceBack;
    CSScrollBounceBack _verticalBounceBack;
public:
    CSScroll(CSScrollParent* parent);
    ~CSScroll();
private:
    bool adjust(CSVector2& delta, float interval, bool dragging);
    bool jump(const CSVector2& position);
public:
    inline const CSVector2& position() const {
        return _current;
    }
    inline void setHorizontalBounceBack(CSScrollBounceBack bounceBack) {
        _horizontalBounceBack = bounceBack;
    }
    inline CSScrollBounceBack horizontalBounceBack() const {
        return _horizontalBounceBack;
    }
    inline void setVerticalBounceBack(CSScrollBounceBack bounceBack) {
        _verticalBounceBack = bounceBack;
    }
    inline CSScrollBounceBack verticalBounceBack() const {
        return _verticalBounceBack;
    }
    inline void setBarThickness(float thickness) {
        _barThickness = thickness;
    }
    inline float barThickness() const {
        return _barThickness;
    }
    inline void setBarColor(const CSColor& color) {
        _barColor = color;
    }
    inline const CSColor& barColor() const {
        return _barColor;
    }
    inline void setBarHidden(bool hidden) {
        _barHidden = hidden;
    }
    inline bool barHidden() const {
        return _barHidden;
    }
    inline bool isScrolling() const {
        return _delta != CSVector2::Zero;
    }

    void stop();
    void set(const CSVector2& position);
    void set(const CSVector2& position, float duration);
    
    void drag(const CSVector2& delta);
    void timeout(float interval, bool dragging);
    void draw(CSGraphics* graphics);
};

#endif /* CSScroll_h */

