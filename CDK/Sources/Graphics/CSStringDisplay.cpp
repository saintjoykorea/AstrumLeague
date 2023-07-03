#define CDK_IMPL

#include "CSStringDisplay.h"

#include "CSGraphicsImpl.h"

#include "icu_compat.h"

#include <unicode/ushape.h>

#define FormatStringFuncPrefix          '#'
#define FormatStringFuncParamCount      4
#define FormatStringFuncNameLength      16
#define FormatStringFuncParamLength     16

CSStringParagraph::~CSStringParagraph() {
	if (type == CSStringParagraphTypeFont) {
		if (attr.font.name) free((char*)attr.font.name);
	}
}

CSStringParagraph::CSStringParagraph(const CSStringParagraph& other) {
	memcpy(this, &other, sizeof(CSStringParagraph));
	if (type == CSStringParagraphTypeFont) {
		if (attr.font.name) attr.font.name = strdup(attr.font.name);
	}
}

CSStringParagraph& CSStringParagraph::operator=(const CSStringParagraph& other) {
	if (type == CSStringParagraphTypeFont) {
		if (attr.font.name) free((char*)attr.font.name);
	}
	memcpy(this, &other, sizeof(CSStringParagraph));
	if (type == CSStringParagraphTypeFont) {
		if (attr.font.name) attr.font.name = strdup(attr.font.name);
	}
	return *this;
}

CSStringDisplay::CSStringDisplay(const char* str) : CSStringDisplay(CSString::ucstring(str)) {

}

CSStringDisplay::CSStringDisplay(const uchar* str) {
	int len = u_strlen(str);

	_content = (uchar*)calloc((len + 1), sizeof(uchar));

	UErrorCode err = U_ZERO_ERROR;
	ucompat_shapeArabic(str, len, _content, len + 1, U_SHAPE_LETTERS_SHAPE, &err);
	if (err > U_ZERO_ERROR) u_strcpy(_content, str);
	    
	_characters = new CSCharSequence(UBRK_CHARACTER, _content);

	_paragraphs = new CSArray<CSStringParagraph>();

	_rtl = false;

	CSCharacters* cs = CSGraphicsImpl::sharedImpl()->characters();

	CSCharSequence words(UBRK_WORD, _content);

	uint pci = 0;
	uint ci = 0;
	uint wi = 0;

	CSStringParagraphType dirtype = CSStringParagraphTypeNeutral;

	while (ci < _characters->count()) {
		uchar* cc = _content + _characters->from(ci);

		switch (cc[0]) {
			case '\r':
				if (_characters->length(ci) >= 2 && cc[1] == '\n') {
					CSStringParagraph& p = _paragraphs->addObject();
					p.type = CSStringParagraphTypeLinebreak;
					p.attr.text.start = ci;
					p.attr.text.end = ci + 1;
				}
				ci++;
				pci = ci;
				while (wi < words.count() && _characters->from(ci) >= words.to(wi)) wi++;
				continue;
			case '\n':
				{
					CSStringParagraph& p = _paragraphs->addObject();
					p.type = CSStringParagraphTypeLinebreak;
					p.attr.text.start = ci;
					p.attr.text.end = ci + 1;
				}
				ci++;
				pci = ci;
				while (wi < words.count() && _characters->from(ci) >= words.to(wi)) wi++;
				continue;
			case FormatStringFuncPrefix:
				if (readTag(ci)) {
					pci = ci;
					while (wi < words.count() && _characters->from(ci) >= words.to(wi)) wi++;
					continue;
				}
				break;
		}

		switch (cs->direction(cc, _characters->length(ci))) {
			case UBIDI_LTR:
				if (dirtype == CSStringParagraphTypeRTL) {
					CSStringParagraph& p = _paragraphs->addObject();
					p.type = CSStringParagraphTypeRTL;
					p.attr.text.start = pci;
					p.attr.text.end = ci;
					pci = ci;
				}
				dirtype = CSStringParagraphTypeLTR;
				break;
			case UBIDI_RTL:
				if (dirtype == CSStringParagraphTypeLTR) {
					CSStringParagraph& p = _paragraphs->addObject();
					p.type = CSStringParagraphTypeLTR;
					p.attr.text.start = pci;
					p.attr.text.end = ci;
					pci = ci;
				}
				dirtype = CSStringParagraphTypeRTL;
				_rtl = true;
				break;
		}

		if (wi < words.count() && _characters->to(ci) >= words.to(wi)) {
			if (dirtype == CSStringParagraphTypeNeutral) {
				uint ei = _characters->to(ci);
				uint i = _characters->from(pci);
				dirtype = CSStringParagraphTypeSpace;
				do {
					uint cp = CSString::toCodePoint(_content, i);
					if (!u_isspace(cp)) {
						dirtype = CSStringParagraphTypeNeutral;
						break;
					}
				} while (i < ei);
			}

			CSStringParagraph& p = _paragraphs->addObject();
			p.type = dirtype;
			p.attr.text.start = pci;
			p.attr.text.end = ci + 1;
			pci = ci + 1;
			dirtype = CSStringParagraphTypeNeutral;
			wi++;
		}
		ci++;
	}
	if (_rtl) {
		uint pi = 0;
		do {
			CSStringParagraph& pa = _paragraphs->objectAtIndex(pi);
			switch (pa.type) {
				case CSStringParagraphTypeNeutral:
				case CSStringParagraphTypeLTR:
					{
						uint npi = pi + 1;
						uint epi = pi;
						while (npi < _paragraphs->count()) {
							const CSStringParagraph& npa = _paragraphs->objectAtIndex(npi);

							switch (npa.type) {
								case CSStringParagraphTypeNeutral:
								case CSStringParagraphTypeLTR:
									epi = npi;
								case CSStringParagraphTypeSpace:
									npi++;
									break;
								default:
									goto fin;
							}
						}
					fin:
						if (epi > pi) {
							const CSStringParagraph& epa = _paragraphs->objectAtIndex(epi);
							pa.attr.text.end = epa.attr.text.end;
							for (uint i = pi; i < epi; i++) _paragraphs->removeObjectAtIndex(pi + 1);
						}
						else if (pa.attr.text.end - pa.attr.text.start == 1 && _characters->length(pa.attr.text.start) == 1) {
							uchar& cc = _content[_characters->from(pa.attr.text.start)];
							switch (cc) {
								case '(':
									cc = ')';
									break;
								case '<':
									cc = '>';
									break;
								case '[':
									cc = ']';
									break;
								case ')':
									cc = '(';
									break;
								case ']':
									cc = '[';
									break;
								case '>':
									cc = '<';
									break;
							}
						}
					}
					break;
			}
		} while (++pi < _paragraphs->count());
	}
	//============================================================================================
	/*
	foreach(const CSStringParagraph&, pa, _paragraphs) {
		uint from = _characters->from(pa.attr.text.start);
		uint to = _characters->from(pa.attr.text.end);
		CSLog("paragraph:%d~%d type:%d text:%s", from, to, pa.type, CSString::cstring(_content + from, to - from));
	}
	 */
	//============================================================================================
}

CSStringDisplay::~CSStringDisplay() {
	free(_content);
	delete _characters;
	_paragraphs->release();
}

static uint parseTag(const uchar* str, uint i, std::function<bool(const char* func, const char** params, uint paramCount)>&& cb) {
	uint prev = i;

	const char* func = NULL;
	const char* params[FormatStringFuncParamCount] = {};
	uint paramCount = 0;

	bool flag = true;
	do {
		switch (str[i]) {
			case '\0':
				return 0;
			case '(':
				if (prev == i) return 0;
				func = CSString::cstring(str + prev, i - prev);
				i++;
				prev = i;
				flag = false;
				break;
			default:
				if (++i - prev > FormatStringFuncNameLength) {
					return 0;
				}
				break;
		}
	} while (flag);

	flag = true;

	do {
		switch (str[i]) {
			case '\0':
				return 0;
			case ')':
				flag = false;
				if (i == prev) {
					i++;
					prev = i;
					break;
				}
			case ',':
				if (paramCount == FormatStringFuncParamCount) {
					return 0;
				}
				while (str[prev] == ' ') prev++;
				if (i > prev) {
					uint end = i;
					while (str[end - 1] == ' ') end--;
					params[paramCount] = CSString::cstring(str + prev, end - prev);
				}
				paramCount++;
				i++;
				prev = i;
				break;
			default:
				if (++i - prev > FormatStringFuncParamLength) {
					return 0;
				}
				break;
		}
	} while (flag);

	return cb(func, params, paramCount) ? i : 0;
}

bool CSStringDisplay::readTag(uint& ci) {
	uint next = parseTag(_content, _characters->from(ci) + 1, [this](const char* func, const char** params, uint paramCount) {
		if (strcasecmp(func, "color") == 0) {
			CSStringParagraph& p = _paragraphs->addObject();
			p.type = CSStringParagraphTypeColor;
			p.attr.color = params[0] ? CSColor(CSString::hexIntValue(params[0])) : CSColor::Transparent;
			return true;
		}
		else if (strcasecmp(func, "stroke") == 0) {
			CSStringParagraph& p = _paragraphs->addObject();
			p.type = CSStringParagraphTypeStroke;
			p.attr.stroke.color = params[0] ? CSColor(CSString::hexIntValue(params[0])) : CSColor::Transparent;
			p.attr.stroke.width = params[1] ? CSString::intValue(params[1]) : 0;
			return true;
		}
		else if (strcasecmp(func, "gradient") == 0) {
			CSStringParagraph& p = _paragraphs->addObject();
			if (params[0] && params[1]) {
				p.type = CSStringParagraphTypeGradient;
				p.attr.gradient.color[0] = CSColor(CSString::hexIntValue(params[0]));
				p.attr.gradient.color[1] = (CSString::hexIntValue(params[1]));
				p.attr.gradient.horizontal = params[2] && strcasecmp(params[2], "horizontal") == 0;
			}
			else {
				p.type = CSStringParagraphTypeGradientReset;
			}
			return true;
		}
		else if (strcasecmp(func, "align") == 0) {
			if (params[0]) {
				CSStringParagraph& p = _paragraphs->addObject();
				p.type = CSStringParagraphTypeAlign;
				if (strcasecmp(params[0], "center") == 0) p.attr.align = 1;
				else if (strcasecmp(params[0], "right") == 0) p.attr.align = 2;
				else p.attr.align = 0;

				return true;
			}
		}
		else if (strcasecmp(func, "font") == 0) {
			CSStringParagraph& p = _paragraphs->addObject();
			p.type = CSStringParagraphTypeFont;
			p.attr.font.name = params[0] ? strdup(params[0]) : NULL;
			p.attr.font.size = params[1] ? CSString::floatValue(params[1]) : 0;
			p.attr.font.style = (CSFontStyle)-1;
			if (params[2]) {
				if (strcasecmp(params[2], "normal") == 0) {
					p.attr.font.style = CSFontStyleNormal;
				}
				else if (strcasecmp(params[2], "bold") == 0) {
					p.attr.font.style = CSFontStyleBold;
				}
				else if (strcasecmp(params[2], "italic") == 0) {
					p.attr.font.style = CSFontStyleItalic;
				}
				else if (strcasecmp(params[2], "medium") == 0) {
					p.attr.font.style = CSFontStyleMedium;
				}
				else if (strcasecmp(params[2], "semibold") == 0) {
					p.attr.font.style = CSFontStyleSemiBold;
				}
			}
			return true;
		}
		return false;
	});

	if (next) {
		while (_characters->from(ci) < next) ci++;
		return true;
	}
	return false;
}


CSString* CSStringDisplay::customString(const CSString* str, char prefix, std::function<bool(CSString* dest, const char* func, const char** params, uint paramCount)>&& cb) {
	if (!str) return NULL;

	CSString* dest = CSString::string();

	uint prev = 0;
	uint i = 0;

	const CSStringDisplay* display = str->display();

	const uchar* content = display->content();

	while (content[i]) {
		if (content[i] == prefix) {
			if (i > prev) {
				dest->append(CSString::cstring(content + prev, i - prev));
			}
			uint next = parseTag(content, i + 1, [&](const char* func, const char** params, uint paramCount) {
				return cb(dest, func, params, paramCount);
			});
			if (next) {
				prev = next;
				i = next;
			}
			else {
				prev = i;
				i++;
			}
		}
		else {
			i++;
		}
	}
	if (prev < i) {
		dest->append(CSString::cstring(content + prev, i - prev));
	}
	return dest;
}
