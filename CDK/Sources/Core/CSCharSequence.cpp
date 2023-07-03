#define CDK_IMPL

#include "CSCharSequence.h"

#include "CSMemory.h"

#ifndef CDK_IOS     //iOS는 ubrk 관련 라이브러리 사용불가

CSCharSequence::CSCharSequence(UBreakIteratorType type, const char* str) : _seq(NULL), _count(0) {
	UErrorCode err = U_ZERO_ERROR;
	UBreakIterator* iterator = NULL;
	UText* text = NULL;

	iterator = ubrk_open(type, NULL, NULL, 0, &err);
	if (err > U_ZERO_ERROR) goto exit;
	text = utext_openUTF8(NULL, str, -1, &err);
	if (err > U_ZERO_ERROR) goto exit;
	ubrk_setUText(iterator, text, &err);
	if (err > U_ZERO_ERROR) goto exit;

	init(iterator);

	if (type == UBRK_WORD) wordMerge(str);
exit:
	if (text) utext_close(text);
	if (iterator) ubrk_close(iterator);
}

CSCharSequence::CSCharSequence(UBreakIteratorType type, const uchar* str) : _seq(NULL), _count(0) {
	UErrorCode err = U_ZERO_ERROR;
	UBreakIterator* iterator = NULL;

	iterator = ubrk_open(type, NULL, str, -1, &err);
	if (err > U_ZERO_ERROR) return;

	init(iterator);

	if (type == UBRK_WORD) wordMerge(str);

	ubrk_close(iterator);
}

void CSCharSequence::init(UBreakIterator* iterator) {
	uint cursor;

	ubrk_first(iterator);
	while ((cursor = ubrk_next(iterator)) != UBRK_DONE) {
		_count++;
	}
	if (!_count) return;

	_seq = (uint*)fmalloc(_count * sizeof(uint));

	ubrk_first(iterator);
	{
		uint i = 0;
		while ((cursor = ubrk_next(iterator)) != UBRK_DONE) {
			_seq[i++] = cursor;
		}
	}
}

#endif

CSCharSequence::~CSCharSequence() {
	if (_seq) free(_seq);
}

template <typename T>
static inline void wordMerge(const T* str, uint* seq, uint& count) {
	uint i = 0;

	while (i < count) {
		switch (str[seq[i]]) {
			case '.':
			case ',':
			case '?':
			case '!':
			case ':':
			case ';':
				count--;
				memcpy(seq + i, seq + i + 1, (count - i) * sizeof(uint));
				break;
			default:
				i++;
				break;
		}
	}
}

void CSCharSequence::wordMerge(const char* str) {
	::wordMerge(str, _seq, _count);
}


void CSCharSequence::wordMerge(const uchar* str) {
	::wordMerge(str, _seq, _count);
}
