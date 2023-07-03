//
//  CSStringDisplay.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 12. 9..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef __CDK__CSStringDisplay__
#define __CDK__CSStringDisplay__

#include "CSArray.h"
#include "CSColor.h"
#include "CSFont.h"

enum CSStringParagraphType {
	CSStringParagraphTypeNeutral,
	CSStringParagraphTypeLTR,
	CSStringParagraphTypeRTL,
	CSStringParagraphTypeSpace,
	CSStringParagraphTypeLinebreak,
	CSStringParagraphTypeColor,
	CSStringParagraphTypeStroke,
	CSStringParagraphTypeGradient,
	CSStringParagraphTypeGradientReset,
	CSStringParagraphTypeAlign,
	CSStringParagraphTypeFont
};

struct CSStringParagraph {
public:
	CSStringParagraphType type;

	union _attr {
		struct {
			uint start;
			uint end;
		} text;
		CSColor color;
		struct {
			CSColor color;
			byte width;
		} stroke;
		struct {
			CSColor color[2];
			bool horizontal;
		} gradient;
		byte align;
		struct {
			const char* name;
			float size;
			CSFontStyle style;
		} font;

		inline _attr() {}
	} attr;

	inline CSStringParagraph() {}
	~CSStringParagraph();

	CSStringParagraph(const CSStringParagraph& other);
	CSStringParagraph& operator=(const CSStringParagraph& other);
};

class CSString;

class CSStringDisplay {
private:
	uchar* _content;
	CSCharSequence* _characters;
	CSArray<CSStringParagraph>* _paragraphs;
	bool _rtl;
public:
	CSStringDisplay(const char* str);
	CSStringDisplay(const uchar* str);
	~CSStringDisplay();

	inline const uchar* content() const {
		return _content;
	}
	inline const CSCharSequence* characters() const {
		return _characters;
	}
	inline const CSArray<CSStringParagraph>* paragraphs() const {
		return _paragraphs;
	}
	inline bool isRTL() const {
		return _rtl;
	}
private:
	CSStringDisplay(const CSStringDisplay&);
	CSStringDisplay& operator=(const CSStringDisplay&);

	bool readTag(uint& ci);
public:
	static CSString* customString(const CSString* str, char prefix, std::function<bool(CSString* dest, const char* func, const char** params, uint paramCount)>&& cb);
};

#endif /* __CDK__CSStringDisplay__ */
