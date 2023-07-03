#ifndef LocaleData_h
#define LocaleData_h

#include "Application.h"

struct LocaleData {
	const CSArray<CSString>* codes;
	const CSString* name;
	char digitGroupSeperator;

	LocaleData(CSBuffer* buffer);
	~LocaleData();

	inline LocaleData(const LocaleData& other) {
		CSAssert(false, "invalid operation");
	}
	inline LocaleData& operator=(const LocaleData&) {
		CSAssert(false, "invalid operation");
		return *this;
	}

	inline const CSString* code() const {
		return codes->objectAtIndex(0);
	}
};

#endif /* LocaleData_h */