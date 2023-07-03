#ifdef CDK_WINDOWS

#ifndef __CDK__CSTextFieldImpl__
#define __CDK__CSTextFieldImpl__

#include "CSGraphics.h"

#include "CSTextField.h"

class CSTextFieldHandle {
private:
	CSTextField* _textField;
	CSColor _textColor;
	CSString* _placeholder;
	uint _maxLine;
	uint _maxLength;
	bool _secureText;
	CSTextAlignment _textAlignment;
	CSString* _text;
	uint _cursor;
	float _scroll;
	bool _attach;
	bool _cursorVisible;
public:
	CSTextFieldHandle(CSTextField* textField);
	~CSTextFieldHandle();

	inline CSTextField* textField() {
		return _textField;
	}
	inline const CSTextField* textField() const {
		return _textField;
	}

	inline bool isAttached() const {
		return _attach;
	}
	void attach();
	void detach();

	inline uint maxLine() const {
		return _maxLine;
	}
	inline void setMaxLine(uint maxLine) {
		_maxLine = maxLine;
	}

	inline const CSString* text() {
		return _text;
	}
	void setText(const char* text);
	void clearText();

	inline uint maxLength() const {
		return _maxLength;
	}
	void setMaxLength(uint maxLength);

	inline const CSColor& textColor() const {
		return _textColor;
	}
	inline void setTextColor(const CSColor& textColor) {
		_textColor = textColor;
	}
	inline const CSString* placeholder() const {
		return _placeholder;
	}
	void setPlaceholder(const char* placeholder);

	inline CSTextAlignment textAlignment() const {
		return _textAlignment;
	}
	inline void setTextAlignment(CSTextAlignment textAlignment) {
		_textAlignment = textAlignment;
	}
	inline bool isSecureText() const {
		return _secureText;
	}
	inline void setSecureText(bool secureText) {
		_secureText = secureText;
	}
	bool isFocused() const;
	void setFocus(bool focused);

	void inputCharacter(uint codepoint);
	void moveLeftCursor(bool deleting);
	void moveRightCursor(bool deleting);
	void moveStartCursor();
	void moveEndCursor();
	void moveCursor(CSVector2 p);
	void complete();
	void draw(CSGraphics* graphics);
	void timeout();
};

class CSTextFieldHandleManager {
private:
	CSArray<CSTextFieldHandle*>* _handles;
	CSTextFieldHandle* _focusedHandle;

	static CSTextFieldHandleManager* __sharedManager;

	CSTextFieldHandleManager();
	~CSTextFieldHandleManager();
public:
	static void initialize();
	static void finalize();

	static inline CSTextFieldHandleManager* sharedManager() {
		return __sharedManager;
	}

	CSTextFieldHandle* focusedHandle() {
		return _focusedHandle;
	}

	void addHandle(CSTextFieldHandle* handle);
	void removeHandle(CSTextFieldHandle* handle);
	bool touchHandle(const CSVector2& p);
	void focusHandle(CSTextFieldHandle* handle);
	void unfocusHandle(CSTextFieldHandle* handle);
	void draw(CSGraphics* graphics);
};

#endif
#endif
