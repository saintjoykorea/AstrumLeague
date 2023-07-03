//
//  CSTextField.h
//  CDK
//
//  Created by chan on 13. 4. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSTextField__
#define __CDK__CSTextField__

#include "CSLayer.h"

enum CSTextAlignment {
    CSTextAlignmentLeft,
    CSTextAlignmentCenter,
    CSTextAlignmentRight
};

enum CSReturnKeyType {
    CSReturnKeyDefault,
    CSReturnKeyGo,
    CSReturnKeyJoin,
    CSReturnKeyNext,
    CSReturnKeySearch,
    CSReturnKeySend,
    CSReturnKeyDone
};

enum CSKeyboardType {
    CSKeyboardDefault,
    CSKeyboardURL,
    CSKeyboardNumberPad,
    CSKeyboardEmailAddress,
    CSKeyboardMultiLine,
};

class CSTextField : public CSLayer {
public:
	CSHandler<CSTextField*> OnTextChanged;
	CSHandler<CSTextField*> OnTextReturn;
private:
    void* _handle;
    bool _autoFocus;
    bool _autoClose;
    const CSFont* _font;
    float _keyboardPadding;
public:
    CSTextField();
protected:
    virtual ~CSTextField();
public:
    static inline CSTextField* textField() {
        return autorelease(new CSTextField());
    }
    
    inline void setKeyboardPadding(float keyboardPadding) {
        _keyboardPadding = keyboardPadding;
    }
    
    inline float keyboardPadding() const {
        return _keyboardPadding;
    }
    
    void setAutoFocus(bool autoFocus);

    inline bool autoFocus() const {
        return _autoFocus;
    }
    
    inline void setAutoClose(bool autoClose) {
        _autoClose = autoClose;
    }
    
    inline bool autoClose() const {
        return _autoClose;
    }
    
    const CSString* text() const;
    void setText(const char* text);
    void clearText();
    CSColor textColor() const;
    void setTextColor(CSColor textColor);
    inline const CSFont* font() const {
        return _font;
    }
    
    void setFont(const CSFont* font);
    const CSString* placeholder() const;
    void setPlaceholder(const char* placeholder);
    CSTextAlignment textAlignment() const;
    void setTextAlignment(CSTextAlignment align);
    CSReturnKeyType returnKeyType() const;
    void setReturnKeyType(CSReturnKeyType returnKeyType);
    CSKeyboardType keyboardType() const;
    void setKeyboardType(CSKeyboardType keyboardType);
    bool isSecureText() const;
    void setSecureText(bool secureTextEntry);
    void setEnabled(bool enabled) override;
	uint maxLength() const;
    void setMaxLength(uint length);
	uint maxLine() const;
	void setMaxLine(uint maxLine);
    
    bool isFocused() const;
    void setFocus(bool focused);
    
    virtual void onTextReturn();
    virtual void onTextChanged();
private:
    void applyFrame();
    void applyFont();
    
    float keyboardScrollDegree(float bottom) const override;
protected:
    virtual void onStateChanged() override;
    virtual void onFrameChanged() override;
    virtual void onProjectionChanged() override;
    virtual void onTouchesView(const CSArray<CSTouch>* touches, bool& interrupt) override;
};

#endif /* defined(__CDK__CSTextField__) */
