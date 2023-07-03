#ifndef __CDK__CSCharSequence__
#define __CDK__CSCharSequence__

#include "CSTypes.h"
#include "CSLog.h"

#include <unicode/ubrk.h>

class CSCharSequence {
private:
	uint* _seq;
	uint _count;
public:
	CSCharSequence(UBreakIteratorType type, const char* str);
	CSCharSequence(UBreakIteratorType type, const uchar* str);
	~CSCharSequence();
private:
	void init(UBreakIterator* iterator);
public:
	inline uint count() const {
		return _count;
	}

	inline uint from(uint i) const {
		CSAssert(i <= _count, "invalid operation");
		return i ? _seq[i - 1] : 0;
	}

	inline uint to(uint i) const {
		CSAssert(i < _count, "invalid operation");
		return _seq[i];
	}

	inline uint length(uint i) const {
		return to(i) - from(i);
	}
private:
	CSCharSequence(const CSCharSequence&);
	CSCharSequence& operator=(const CSCharSequence&);

	void wordMerge(const char* str);
	void wordMerge(const uchar* str);
};

#endif /* __CDK__CSCharSequence__ */
