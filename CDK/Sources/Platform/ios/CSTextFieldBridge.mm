//
//  CSTextFieldBridge.mm
//  CDK
//
//  Created by chan on 13. 4. 16..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSTextFieldBridge.h"

#import "CSView.h"

#import <UIKit/UIKit.h>

@interface CSTextFieldHandle : NSObject<UITextFieldDelegate, UITextViewDelegate> {
    UITextField* _textFieldComponent;
    UITextView* _textViewComponent;
	CSTextField* _textField;
	uint _maxLength;
    uint _maxLine;
}

@property (nonatomic, readonly) CSTextField* textField;
@property (nonatomic, readwrite) uint maxLength;
@property (nonatomic, readwrite) uint maxLine;
@property (nonatomic, readwrite, retain) NSString* text;
@property (nonatomic, readwrite, retain) UIColor* textColor;
@property (nonatomic, readwrite, retain) UIFont* font;
@property (nonatomic, readwrite) NSTextAlignment textAlignment;
@property (nonatomic, readwrite) UIReturnKeyType returnKeyType;
@property (nonatomic, readwrite) UIKeyboardType keyboardType;
@property (nonatomic, readwrite, getter=isSecureText) BOOL secureText;
@property (nonatomic, readwrite, getter=isFocused) BOOL focus;

-(id)initWithTextField:(CSTextField*)textField;

@end

@implementation CSTextFieldHandle

@synthesize textField = _textField, maxLength = _maxLength;

-(id)initWithTextField:(CSTextField*)textField {
	if (self = [super init]) {
        _textField = textField;
        _maxLength = 0;
        _maxLine = 1;
        
        _textFieldComponent = [[UITextField alloc] init];
        _textFieldComponent.delegate = self;
        _textFieldComponent.textColor = [UIColor blackColor];
        _textFieldComponent.backgroundColor = [UIColor clearColor];
        _textFieldComponent.autocorrectionType = UITextAutocorrectionTypeNo;
        _textFieldComponent.autocapitalizationType = UITextAutocapitalizationTypeNone;
        
        _textViewComponent = [[UITextView alloc] init];
        _textViewComponent.delegate = self;
        _textViewComponent.textColor = [UIColor blackColor];
        _textViewComponent.backgroundColor = [UIColor clearColor];
        _textViewComponent.autocorrectionType = UITextAutocorrectionTypeNo;
        _textViewComponent.autocapitalizationType = UITextAutocapitalizationTypeNone;
	}
	return self;
}

-(void)dealloc {
    [_textFieldComponent setDelegate:nil];
	[_textFieldComponent removeFromSuperview];
	[_textFieldComponent release];
    
    [_textViewComponent setDelegate:nil];
    [_textViewComponent removeFromSuperview];
    [_textViewComponent release];

	[super dealloc];
}

-(BOOL)textFieldShouldReturn:(UITextField*)textField {
	_textField->onTextReturn();
    return YES;
}

-(void)textChanged {
    _textField->onTextChanged();
}

-(void)textViewDidChange:(UITextView *)textView {
    int currentLine = (int)((textView.contentSize.height - textView.font.lineHeight) / textView.font.lineHeight);
    if (currentLine > _maxLine) {
        NSString* strNew = [textView.text substringToIndex:[textView.text length] - 1];
        textView.text = strNew;
    }
}

-(BOOL)textField:(UITextField*)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string {
	if (_maxLength && textField.text.length >= _maxLength && range.length == 0) {
		return NO;
	}
	else {
        [self performSelectorOnMainThread:@selector(textChanged) withObject:nil waitUntilDone:NO];
        return YES;
	}
}

-(void)setMaxLine:(uint)maxLine {
    _maxLine = maxLine;
    
    bool attach = _textFieldComponent.superview != nil || _textViewComponent.superview != nil;

    if (_maxLine > 1) {
        [_textFieldComponent removeFromSuperview];
        if (attach) [(UIView*)_textField->view()->handle() addSubview:_textViewComponent];
    }
    else {
        [_textViewComponent removeFromSuperview];
        if (attach) [(UIView*)_textField->view()->handle() addSubview:_textFieldComponent];
    }
}

-(void)addToView {
    if (_maxLine > 1) [(UIView*)_textField->view()->handle() addSubview:_textViewComponent];
    else [(UIView*)_textField->view()->handle() addSubview:_textFieldComponent];
}

-(void)removeFromView {
    [_textViewComponent removeFromSuperview];
    [_textFieldComponent removeFromSuperview];
}

-(void)setFrame:(CGRect)frame {
    [CATransaction begin];
    [CATransaction setValue:(id)kCFBooleanTrue forKey:kCATransactionDisableActions];
    [_textViewComponent setFrame:frame];
    [_textFieldComponent setFrame:frame];
    [CATransaction commit];
}

-(NSString*)text {
    return _maxLine > 1 ? _textViewComponent.text : _textFieldComponent.text;
}

-(void)setText:(NSString*)text {
    [_textFieldComponent setText:text];
    [_textViewComponent setText:text];
}

-(UIColor*)textColor {
    return _maxLine > 1 ? _textViewComponent.textColor : _textFieldComponent.textColor;
}

-(void)setTextColor:(UIColor*)color {
    [_textFieldComponent setTextColor:color];
    [_textViewComponent setTextColor:color];
}

-(UIFont*)font {
    return _maxLine > 1 ? _textViewComponent.font : _textFieldComponent.font;
}

-(void)setFont:(UIFont*)font {
    [_textFieldComponent setFont:font];
    [_textViewComponent setFont:font];
}

-(NSString*)placeholder {
    return _textFieldComponent.placeholder;
}

-(void)setPlaceholder:(NSString*)placeholder {
    [_textFieldComponent setPlaceholder:placeholder];
}

-(NSTextAlignment)textAlignment {
    return _maxLine > 1 ? _textViewComponent.textAlignment : _textFieldComponent.textAlignment;
}

-(void)setTextAlignment:(NSTextAlignment)textAlignment {
    [_textFieldComponent setTextAlignment:textAlignment];
    [_textViewComponent setTextAlignment:textAlignment];
}

-(UIReturnKeyType)returnKeyType {
    return _maxLine > 1 ? _textViewComponent.returnKeyType : _textFieldComponent.returnKeyType;
}

-(void)setReturnKeyType:(UIReturnKeyType)returnKeyType {
    [_textFieldComponent setReturnKeyType:returnKeyType];
    [_textViewComponent setReturnKeyType:returnKeyType];
}

-(UIKeyboardType)keyboardType {
    return _maxLine > 1 ? _textViewComponent.keyboardType : _textFieldComponent.keyboardType;
}

-(void)setKeyboardType:(UIKeyboardType)keyboardType {
    [_textFieldComponent setKeyboardType:keyboardType];
    [_textViewComponent setKeyboardType:keyboardType];
}

-(BOOL)isSecureText {
    return _maxLine > 1 ? _textViewComponent.isSecureTextEntry : _textFieldComponent.isSecureTextEntry;
}

-(void)setSecureText:(BOOL)secureText {
    [_textFieldComponent setSecureTextEntry:secureText];
    [_textViewComponent setSecureTextEntry:secureText];
}

-(BOOL)isFocused {
    return _maxLine > 1 ? _textViewComponent.isFirstResponder : _textFieldComponent.isFirstResponder;
}

-(void)setFocus:(BOOL)focus {
    if (focus) {
        [_maxLine > 1 ? _textViewComponent : _textFieldComponent becomeFirstResponder];
    }
    else {
        [_maxLine > 1 ? _textViewComponent : _textFieldComponent resignFirstResponder];
    }
}

@end

void* CSTextFieldBridge::createHandle(CSTextField* textField) {
	return [[CSTextFieldHandle alloc] initWithTextField:textField];
}

void CSTextFieldBridge::destroyHandle(void* handle) {
	[(CSTextFieldHandle*) handle release];
}

void CSTextFieldBridge::addToView(void* handle) {
    [(CSTextFieldHandle*) handle addToView];
}

void CSTextFieldBridge::removeFromView(void* handle) {
	[(CSTextFieldHandle*) handle removeFromView];
}

void CSTextFieldBridge::setFrame(void* handle, const CSRect& frame) {
	CGRect cgframe = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);

    [(CSTextFieldHandle*) handle setFrame:cgframe];
}

const CSString* CSTextFieldBridge::text(void* handle) {
	NSString* nstext = [(CSTextFieldHandle*) handle text];

	return CSString::string(nstext.UTF8String);
}

void CSTextFieldBridge::setText(void* handle, const char* text) {
	NSString* nstext = [NSString stringWithUTF8String:text];

	[(CSTextFieldHandle*) handle setText:nstext];
}

void CSTextFieldBridge::clearText(void* handle) {
    [(CSTextFieldHandle*) handle setText:nil];
}

CSColor CSTextFieldBridge::textColor(void* handle) {
	UIColor* uicolor = [(CSTextFieldHandle*) handle textColor];

	CGFloat r, g, b, a;

	[uicolor getRed:&r green:&g blue:&b alpha:&a];

	return CSColor((float)r, (float)g, (float)b, (float)a);
}

void CSTextFieldBridge::setTextColor(void* handle, const CSColor& textColor) {
	UIColor* uicolor = [UIColor colorWithRed:textColor.r green:textColor.g blue:textColor.b alpha:textColor.a];

	[(CSTextFieldHandle*) handle setTextColor:uicolor];
}

void CSTextFieldBridge::setFont(void* handle, const CSFont* font) {
	UIFont* uifont;
	switch (font->style()) {
        case CSFontStyleBold:
            uifont = [UIFont boldSystemFontOfSize:font->size()];
			break;
        case CSFontStyleItalic:
            uifont = [UIFont italicSystemFontOfSize:font->size()];
			break;
        default:
            uifont = [UIFont systemFontOfSize:font->size()];
			break;
    }
	[(CSTextFieldHandle*) handle setFont:uifont];		//multiple font face not supported
}

const CSString* CSTextFieldBridge::placeholder(void* handle) {
	NSString* nsplaceholder = [(CSTextFieldHandle*) handle  placeholder];

	return CSString::string(nsplaceholder.UTF8String);
}

void CSTextFieldBridge::setPlaceholder(void* handle, const char* placeholder) {
	NSString* nsplaceholder = [NSString stringWithUTF8String:placeholder];

	[(CSTextFieldHandle*) handle setPlaceholder:nsplaceholder];
}

CSTextAlignment CSTextFieldBridge::textAlignment(void* handle) {
	NSTextAlignment uialign = [(CSTextFieldHandle*) handle textAlignment];

	switch (uialign) {
		case NSTextAlignmentCenter:
			return CSTextAlignmentCenter;
		case NSTextAlignmentRight:
			return CSTextAlignmentRight;
		default:
			return CSTextAlignmentLeft;
	}
}

void CSTextFieldBridge::setTextAlignment(void* handle, CSTextAlignment textAlignment) {
	NSTextAlignment uiTextAlignment;

	switch (textAlignment) {
		case CSTextAlignmentCenter:
			uiTextAlignment = NSTextAlignmentCenter;
			break;
		case CSTextAlignmentRight:
			uiTextAlignment = NSTextAlignmentRight;
			break;
		default:
			uiTextAlignment = NSTextAlignmentLeft;
			break;
	}
	[(CSTextFieldHandle*) handle setTextAlignment:uiTextAlignment];
}

CSReturnKeyType CSTextFieldBridge::returnKeyType(void* handle) {
	UIReturnKeyType uirtn = [(CSTextFieldHandle*) handle returnKeyType];

	switch (uirtn) {
		case UIReturnKeyGo:
			return CSReturnKeyGo;
		case UIReturnKeyJoin:
			return CSReturnKeyJoin;
		case UIReturnKeyNext:
			return CSReturnKeyNext;
		case UIReturnKeySearch:
			return CSReturnKeySearch;
		case UIReturnKeySend:
			return CSReturnKeySend;
		case UIReturnKeyDone:
			return CSReturnKeyDone;
		default:
			return CSReturnKeyDefault;
	}
}

void CSTextFieldBridge::setReturnKeyType(void* handle, CSReturnKeyType returnKeyType) {
	UIReturnKeyType uiReturnKeyType;

	switch (returnKeyType) {
		case CSReturnKeyGo:
			uiReturnKeyType = UIReturnKeyGo;
			break;
		case CSReturnKeyJoin:
			uiReturnKeyType = UIReturnKeyJoin;
			break;
		case CSReturnKeyNext:
			uiReturnKeyType = UIReturnKeyNext;
			break;
		case CSReturnKeySearch:
			uiReturnKeyType = UIReturnKeySearch;
			break;
		case CSReturnKeySend:
			uiReturnKeyType = UIReturnKeySend;
			break;
		case CSReturnKeyDone:
			uiReturnKeyType = UIReturnKeyDone;
			break;
		default:
			uiReturnKeyType = UIReturnKeyDefault;
			break;
	}
	[(CSTextFieldHandle*) handle setReturnKeyType:uiReturnKeyType];
}

CSKeyboardType CSTextFieldBridge::keyboardType(void* handle) {
	UIKeyboardType uikeyboard = [(CSTextFieldHandle*) handle keyboardType];

	switch (uikeyboard) {
		case UIKeyboardTypeAlphabet:
		case UIKeyboardTypeDefault:
			return CSKeyboardDefault;
		case UIKeyboardTypeURL:
			return CSKeyboardURL;
		case UIKeyboardTypeNumberPad:
			return CSKeyboardNumberPad;
		case UIKeyboardTypeEmailAddress:
			return CSKeyboardEmailAddress;
		default:
			return CSKeyboardDefault;
	}
}

void CSTextFieldBridge::setKeyboardType(void* handle, CSKeyboardType keyboardType) {
	UIKeyboardType uiKeyboardType;

	switch (keyboardType) {
		case CSKeyboardURL:
			uiKeyboardType = UIKeyboardTypeURL;
			break;
		case CSKeyboardNumberPad:
			uiKeyboardType = UIKeyboardTypeNumberPad;
			break;
        case CSKeyboardEmailAddress:
			uiKeyboardType = UIKeyboardTypeEmailAddress;
			break;
		default:
			uiKeyboardType = UIKeyboardTypeDefault;
			break;
	}
	[(CSTextFieldHandle*) handle setKeyboardType:uiKeyboardType];
}

bool CSTextFieldBridge::isSecureText(void* handle) {
	return [(CSTextFieldHandle*) handle isSecureText];
}

void CSTextFieldBridge::setSecureText(void* handle, bool secureText) {
    [(CSTextFieldHandle*) handle setSecureText:secureText];
}

uint CSTextFieldBridge::maxLength(void* handle) {
	return [(CSTextFieldHandle*) handle maxLength];
}

void CSTextFieldBridge::setMaxLength(void* handle, uint maxLength) {
	[(CSTextFieldHandle*) handle setMaxLength:maxLength];
}

uint CSTextFieldBridge::maxLine(void* handle) {
    return [(CSTextFieldHandle*) handle maxLine];
}

void CSTextFieldBridge::setMaxLine(void* handle, uint maxLine) {
    [(CSTextFieldHandle*) handle setMaxLine:maxLine];
}

bool CSTextFieldBridge::isFocused(void* handle) {
    return [(CSTextFieldHandle*) handle isFocused];
}

void CSTextFieldBridge::setFocus(void* handle, bool focused) {
    [(CSTextFieldHandle*) handle setFocus:focused];
}

#endif
