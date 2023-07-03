//
//  CSLocaleString.h
//  CDK
//
//  Created by 김찬 on 16. 8. 10..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSLocaleString__
#define __CDK__CSLocaleString__

#include "CSString.h"
#include "CSArray.h"

extern const CSString* CSLocaleDefault;

class CSBuffer;

class CSLocaleString : public CSObject {
private:
    CSString* _currentLocaleValue;
    mutable uint _currentLocaleMark;
    
    struct LocaleValue {
        const CSString* locale;
        CSString* value;
        
        ~LocaleValue();
    };
    CSArray<LocaleValue>* _localeValues;
    
    static uint _localeMark;
    static const CSString* _locale;
	static char _digitGroupSeperator;
public:
    CSLocaleString();
    CSLocaleString(CSBuffer* buffer, CSStringEncoding encoding = CSStringEncodingUTF8);
    CSLocaleString(const byte*& data, CSStringEncoding encoding = CSStringEncodingUTF8);
private:
    ~CSLocaleString();
public:
    static inline CSLocaleString* string() {
        return autorelease(new CSLocaleString());
    }
    static CSLocaleString* stringWithBuffer(CSBuffer* buffer);    
    static CSLocaleString* stringWithBuffer(CSBuffer* buffer, CSStringEncoding encoding);
    static inline CSLocaleString* stringWithBytes(const byte*& data, CSStringEncoding encoding = CSStringEncodingUTF8) {
        return autorelease(new CSLocaleString(data, encoding));
    }
    
    CSString* value();
    inline const CSString* value() const {
        return const_cast<CSLocaleString*>(this)->value();
    }
    inline operator const char*() const {
        return *value();
    }
    
    CSString* localeValue(const char* locale, bool useDefault = true);
    inline const CSString* localeValue(const char* locale, bool useDefault = true) const {
        return const_cast<CSLocaleString*>(this)->localeValue(locale, useDefault);
    }
    
    void setLocaleValue(const char* locale, const char* value);
    
    static inline uint localeMark() {
        return _localeMark;
    }
    static inline const CSString* locale() {
        return _locale;
    }
    static void setLocale(const char* locale);

	static inline char digitGroupSeperator() {
		return _digitGroupSeperator;
	}
	static inline void setDigitGroupSeperator(char seperator) {
		_digitGroupSeperator = seperator;
	}
};

#endif /* defined(__CDK__CSLocaleString__) */
