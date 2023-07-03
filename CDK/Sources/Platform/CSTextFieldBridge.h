//
//  CSTextFieldBridge.h
//  CDK
//
//  Created by chan on 13. 4. 16..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifdef CDK_IMPL

#ifndef __CDK__CSTextFieldBridge__
#define __CDK__CSTextFieldBridge__

#include "CSTextField.h"

#include "CSFont.h"

class CSTextFieldBridge {
public:
    static void* createHandle(CSTextField* textField);
    static void destroyHandle(void* handle);
    static void addToView(void* handle);
    static void removeFromView(void* handle);
    static void setFrame(void* handle, const CSRect& frame);
    static const CSString* text(void* handle);
    static void setText(void* handle, const char* text);
    static void clearText(void* handle);
    static CSColor textColor(void* handle);
    static void setTextColor(void* handle, const CSColor& textColor);
    static void setFont(void* handle, const CSFont* font);
    static const CSString* placeholder(void* handle);
    static void setPlaceholder(void* handle, const char* placeholder);
    static CSTextAlignment textAlignment(void* handle);
    static void setTextAlignment(void* handle, CSTextAlignment textAlignment);
    static CSReturnKeyType returnKeyType(void* handle);
    static void setReturnKeyType(void* handle, CSReturnKeyType returnKeyType);
    static CSKeyboardType keyboardType(void* handle);
    static void setKeyboardType(void* handle, CSKeyboardType keyboardType);
    static bool isSecureText(void* handle);
    static void setSecureText(void* handle, bool secureText);
    static uint maxLength(void* handle);
    static void setMaxLength(void* handle, uint maxLength);
	static uint maxLine(void* handle);
	static void setMaxLine(void* handle, uint maxLine);
	static bool isFocused(void* handle);
    static void setFocus(void* handle, bool focused);
};

#endif

#endif
