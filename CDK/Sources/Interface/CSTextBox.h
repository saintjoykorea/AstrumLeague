//
//  CSTextBox.h
//  CDK
//
//  Created by 김찬 on 12. 8. 21..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSTextBox__
#define __CDK__CSTextBox__

#include "CSLayer.h"

#include "CSScroll.h"

class CSTextBox : public CSLayer, public CSScrollParent {
public:
    CSScroll scroll;
protected:
    CSString* _text;
    const CSFont* _font;
	CSColor _textColor;
	float _maxHeight;
    float _textHeight;
	CSColor _strokeColor;
	byte _strokeWidth;
	bool _lockScroll;
public:
    CSTextBox();
protected:
    virtual ~CSTextBox();
public:
    static inline CSTextBox* textBox() {
        return autorelease(new CSTextBox());
    }
    
    inline CSSize scrollContentSize() const override {
        return CSSize(width(), _textHeight);
    }
    inline CSSize scrollClipSize() const override {
        return _frame.size;
    }
    
    void setText(const char* str);
    inline const CSString* text() const {
        return _text;
    }
    
    void setFont(const CSFont* font);
    inline const CSFont* font() const {
        return _font;
    }
    
    void setTextColor(const CSColor& color);
    inline const CSColor& textColor() const {
        return _textColor;
    }

	void setStrokeWidth(int width);
	inline int strokeWidth() const {
		return _strokeWidth;
	}

	void setStrokeColor(const CSColor& color);
	inline const CSColor& strokeColor() const {
		return _strokeColor;
	}
    
    void setMaxHeight(float maxHeight);
    inline float maxHeight() const {
        return _maxHeight;
    }
    
    inline float textHeight() const {
        return _textHeight;
    }
    
    inline void setLockScroll(bool lockScroll) {
        _lockScroll = lockScroll;
    }
    inline bool lockScroll() const {
        return _lockScroll;
    }
    
    void clearText();
    void appendText(const char* str);

private:
    bool shrinkText();
    void updateTextHeight();
public:
    void autoScroll();
protected:
    virtual void onTimeout() override;
    virtual void onDraw(CSGraphics* graphics) override;
    virtual void onTouchesMoved() override;
};
#endif /* defined(__CDK__CSTextBox__) */
