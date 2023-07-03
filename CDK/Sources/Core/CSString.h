//
//  CSString.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 1. 4..
//  Copyright 짤 2019??brgames. All rights reserved.
//

#ifndef CSString_h
#define CSString_h

#include "CSResource.h"
#include "CSCharSequence.h"

#include <string>

template <typename V, int dimension, bool readonly>
class CSArray;

class CSData;
class CSBuffer;
class CSURLRequest;
class CSStringDisplay;

enum CSStringEncoding {
    CSStringEncodingUTF8,
    CSStringEncodingUTF16,
    CSStringEncodingUTF16LE,
    CSStringEncodingUTF16BE
};

#ifdef CDK_WINDOWS
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

class CSString : public CSResource {
private:
    std::string _cstring;
	mutable const CSCharSequence* _characters;
	mutable const CSStringDisplay* _display;
    mutable bool _hashditry;
    mutable uint _hash;
public:
    inline CSString() {
        
    }
    CSString(const char* str);
    CSString(const char* str, uint length);
private:
    ~CSString();
public:
    static CSString* createWithFormatAndArguments(const char* format, va_list args);
    static CSString* createWithFormat(const char* format, ...);
    static CSString* createWithBytes(const void* data, uint length, CSStringEncoding encoding = CSStringEncodingUTF8);
    static CSString* createWithBuffer(CSBuffer* buffer, CSStringEncoding encoding = CSStringEncodingUTF8);
    static CSString* createWithContentOfFile(const char* path, CSStringEncoding encoding = CSStringEncodingUTF8);
    static CSString* createWithContentOfURL(const CSURLRequest* request, CSStringEncoding encoding = CSStringEncodingUTF8);
    
    static inline CSString* string() {
        return autorelease(new CSString());
    }
    static inline CSString* string(const char* str) {
        return autorelease(new CSString(str));
    }
    static inline CSString* string(const char* str, uint length) {
        return autorelease(new CSString(str, length));
    }
    static inline CSString* stringWithFormatAndArguments(const char* format, va_list args) {
        return autorelease(createWithFormatAndArguments(format, args));
    }
    static CSString* stringWithFormat(const char* format, ...);
    static inline CSString* stringWithBytes(const void* data, uint length, CSStringEncoding encoding = CSStringEncodingUTF8) {
        return autorelease(createWithBytes(data, length, encoding));
    }
    
    static CSString* stringWithBuffer(CSBuffer* buffer);
    static CSString* stringWithBuffer(CSBuffer* buffer, CSStringEncoding encoding);
    
    static inline CSString* stringWithContentOfFile(const char* path, CSStringEncoding encoding = CSStringEncodingUTF8) {
        return autorelease(createWithContentOfFile(path, encoding));
    }
    static inline CSString* stringWithContentOfURL(const CSURLRequest* request, CSStringEncoding encoding = CSStringEncodingUTF8) {
        return autorelease(createWithContentOfURL(request, encoding));
    }

public:
    inline CSResourceType resourceType() const override {
        return CSResourceTypeString;
    }
    
    int intValue() const;
    uint hexIntValue() const;
    int64 longValue() const;
    float floatValue() const;
    double doubleValue() const;
    bool boolValue() const;
    
    static int intValue(const char* str);
    static uint hexIntValue(const char* str);
    static int64 longValue(const char* str);
    static float floatValue(const char* str);
    static double doubleValue(const char* str);
    static bool boolValue(const char* str);
    
    const CSCharSequence* characters() const;
    const CSStringDisplay* display() const;

	const char* characterAtIndex(uint index) const;
    uint length() const;
    bool startsWith(const char* str) const;
    bool endsWith(const char* str) const;
    int indexOf(const char* str) const;
    int indexOf(const char* str, uint from) const;
    CSString* substringWithRange(uint index, uint length) const;
    CSString* substringFromIndex(uint index) const;
    CSString* trimmedString() const;
    
    int compareTo(const char* str, bool caseSensitive = false) const;
    CSArray<CSString, 1, true>* split(const char* delimiter) const;
    CSData* data(CSStringEncoding encoding = CSStringEncodingUTF8) const;
    bool isEqualToString(const char* str) const;
    inline bool isEqualToString(const CSString* str) const {
        return str == this || isEqualToString(*str);
    }
    void replace(const char* from, const char* to);
    void clear();
    void append(const char* str);
    void appendFormatAndArguments(const char* format, va_list args);
    void appendFormat(const char* format, ...);
    void insert(uint i, const char* str);
    void insertFormatAndArguments(uint i, const char* format, va_list args);
    void insertFormat(uint i, const char* format, ...);
	void removeWithRange(uint index, uint length);
	void removeAtIndex(uint i);
    void set(const char* str);
    void trim();
    
    bool AESEncrypt(const char* key);
    bool AESDecrypt(const char* key);
    static CSString* AESEncrypt(const char* str, const char* key);
    static CSString* AESDecrypt(const char* str, const char* key);
    
	static char* cstring(const uchar* str, bool free = true);
	static char* cstring(const uchar* str, uint ulength, bool free = true);
	static uchar* ucstring(const char* str, bool free = true);
	static uchar* ucstring(const char* str, uint clength, bool free = true);

	static char* cstring(const char* str, uint clength);
    static char* cstringWithFormatAndArguments(const char* format, va_list args);
    static char* cstringWithFormat(const char* format, ...);

	static char* cstringWithNumber(int value, bool comma = false);
	static char* cstringWithNumber(uint value, bool comma = false);
	static char* cstringWithNumber(int64 value, bool comma = false);
	static char* cstringWithNumber(uint64 value, bool comma = false);
private:
    static const char* cstring(const CSString* str);
	static uchar* ucstring(const CSString* str);
public:
    inline operator const char*() const {
        const CSString* str = this;

		return cstring(str);
    }
	inline operator uchar*() const {
        const CSString* str = this;

		return ucstring(str);
	}

    static uint toCodePoint(const uchar* cc, uint& i);
    static uint toCodePoint(const char* cc, uint& i);
    static inline uint toCodePoint(const uchar* cc) {
        uint i = 0;
        return toCodePoint(cc, i);
    }
    static inline uint toCodePoint(const char* cc) {
        uint i = 0;
        return toCodePoint(cc, i);
    }

	CSString* sha1() const;
    static CSString* sha1(const char* str);
    
    static void writeTo(CSBuffer* buffer, const CSString* str);
    static void writeTo(CSBuffer* buffer, const CSString* str, CSStringEncoding encoding);
    static void writeTo(CSBuffer* buffer, const char* str);
    
    void writeTo(CSBuffer* buffer) const;
    void writeTo(CSBuffer* buffer, CSStringEncoding encoding) const;
    
    static void writeToFile(const char* path, const char* str, bool compression = false);
    
    void writeToFile(const char* path, bool compression = false) const;
    void writeToFile(const char* path, CSStringEncoding encoding, bool compression = false) const;
    
    uint hash() const override;
    CSObject* copy() const override;
    bool isEqual(const CSObject* object) const override;
    int compareTo(const CSObject* object) const override;
    
    static bool isEqual(const char* str1, const char* str2);
    
    static inline bool isEqual(const CSString* str1, const CSString* str2) {
        return str1 == str2 || isEqual(*str1, *str2);
    }
    
private:
    void dirty();
public:
    void hide();
};

#endif /* CSString_h */
