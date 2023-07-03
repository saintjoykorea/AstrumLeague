//
//  CSString.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 1. 4..
//  Copyright 짤 2019??brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSString.h"

#include "CSEncoder.h"
#include "CSBuffer.h"
#include "CSFile.h"
#include "CSArray.h"
#include "CSURLConnection.h"
#include "CSCrypto.h"
#include "CSLocaleString.h"
#include "CSStringDisplay.h"

CSString::CSString(const char* str) : _cstring(str), _hashditry(true) {
    
}

CSString::CSString(const char* str, uint clength) : _hashditry(true) {
    _cstring.append(str, clength);
}

CSString::~CSString() {
	if (_characters) delete _characters;
	if (_display) delete _display;
}

CSString* CSString::createWithFormatAndArguments(const char* format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    uint length = vsnprintf(NULL, 0, format, copy) + 1;
    va_end(copy);

	char* buf = (char*)alloca(length);
    vsprintf(buf, format, args);
    CSString* str = new CSString(buf);
    return str;
}

CSString* CSString::createWithFormat(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    CSString* str = createWithFormatAndArguments(format, ap);
    va_end(ap);
    return str;
}

CSString* CSString::createWithBytes(const void* data, uint len, CSStringEncoding encoding) {
    return CSEncoder::createStringWithBytes(data, len, encoding);
}

CSString* CSString::createWithBuffer(CSBuffer* buffer, CSStringEncoding encoding) {
    int len = buffer->readLength();
    
    CSString* str = NULL;
    
    if (len) {
        str = CSString::createWithBytes(buffer->bytes(), len, encoding);
        buffer->skip(len);
    }
    return str;
}

CSString* CSString::createWithContentOfFile(const char* path, CSStringEncoding encoding) {
    uint length = 0;
    void* data = CSFile::createRawWithContentOfFile(path, 0, length, false);
    if (data) {
        CSString* str = CSEncoder::createStringWithBytes(data, length, encoding);
        free(data);
        return str;
    }
    return NULL;
}

CSString* CSString::createWithContentOfURL(const CSURLRequest* request, CSStringEncoding encoding) {
    const CSData* data = CSURLConnection::sendSynchronousRequest(request);
    if (data) {
        CSString* str = CSEncoder::createStringWithBytes(data->bytes(), data->length(), encoding);
        return str;
    }
    return NULL;
}

CSString* CSString::stringWithFormat(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    CSString* str = createWithFormatAndArguments(format, ap);
    va_end(ap);
    str->autorelease();
    return str;
}

CSString* CSString::stringWithBuffer(CSBuffer* buffer) {
    return autorelease(createWithBuffer(buffer));
}

CSString* CSString::stringWithBuffer(CSBuffer* buffer, CSStringEncoding encoding) {
    return autorelease(createWithBuffer(buffer, encoding));
}

int CSString::intValue() const {
    return intValue(_cstring.c_str());
}

int64 CSString::longValue() const {
    return longValue(_cstring.c_str());
}

uint CSString::hexIntValue() const {
    return hexIntValue(_cstring.c_str());
}

float CSString::floatValue() const {
    return floatValue(_cstring.c_str());
}

double CSString::doubleValue() const {
    return doubleValue(_cstring.c_str());
}

bool CSString::boolValue() const {
    return boolValue(_cstring.c_str());
}

int CSString::intValue(const char* str) {
    return atoi(str);
}

int64 CSString::longValue(const char* str) {
    return atoll(str);
}

uint CSString::hexIntValue(const char* str) {
    char* p;
    return strtoul(str, &p, 16);
}

float CSString::floatValue(const char* str) {
    char* p;
    return strtof(str, &p);
}

double CSString::doubleValue(const char* str) {
    char* p;
    return strtod(str, &p);
}

bool CSString::boolValue(const char* str) {
    return strcmp(str, "0") && strcasecmp(str, "false");
}

const CSCharSequence* CSString::characters() const {
	if (!_characters) {
		_characters = new CSCharSequence(UBRK_CHARACTER, _cstring.c_str());
	}
	return _characters;
}

const CSStringDisplay* CSString::display() const {
    if (!_display) {
        _display = new CSStringDisplay(_cstring.c_str());
    }
    return _display;
}

const char* CSString::characterAtIndex(uint index) const {
	characters();

	int from = _characters->from(index);
	int to = _characters->to(index);

	char* dest = (char*)fmalloc(to - from + 1);
	const char* src = _cstring.c_str();
	strncpy(dest, src + from, to - from);
	dest[to - from] = 0;
	autofree(dest);

	return dest;
}

uint CSString::length() const {
	characters();
    
	return _characters->count();
}

bool CSString::startsWith(const char* str) const {
    return _cstring.find(str) == 0;
}

bool CSString::endsWith(const char* str) const {
    return _cstring.find(str) == _cstring.length() - strlen(str);
}

int CSString::indexOf(const char* str) const {
    return indexOf(str, 0);
}

int CSString::indexOf(const char* str, uint from) const {
	characters();

	uint cfrom = _characters->from(from);

	const char* s = _cstring.c_str();
	const char* p = strstr(s + cfrom, str);

	if (!p) return CSNotFound;

	uint ci = (uint)(p - s);

	for (uint i = from; i < _characters->count(); i++) {
		if (_characters->to(i) >= ci) return i;
	}

	return CSNotFound;
}

CSString* CSString::substringWithRange(uint index, uint length) const {
	characters();

	uint cfrom = _characters->from(index);
	uint cto = _characters->to(index + length - 1);

	return CSString::string(_cstring.c_str() + cfrom, cto - cfrom);
}

CSString* CSString::substringFromIndex(uint index) const {
	characters();

	return CSString::string(_cstring.c_str() + _characters->from(index));
}

CSString* CSString::trimmedString() const {
    CSString* str = CSString::string(_cstring.c_str());
    str->trim();
    return str;
}

int CSString::compareTo(const char* str, bool caseSensitive) const {
    return caseSensitive ? strcmp(_cstring.c_str(), str) : strcasecmp(_cstring.c_str(), str);
}

CSArray<CSString>* CSString::split(const char* delimiter) const {
    char* str = strdup(_cstring.c_str());
    char* tok = strtok(str, delimiter);
    
    CSArray<CSString>* tokens = CSArray<CSString>::array();
    while (tok) {
        tokens->addObject(CSString::string(tok));
        tok = strtok(NULL, delimiter);
    }
    free(str);
    
    return tokens;
}

CSData* CSString::data(CSStringEncoding encoding) const {
    uint length = 0;
    void* bytes = CSEncoder::createRawWithString(_cstring.c_str(), length, encoding);
    return bytes ? CSData::dataWithBytes(bytes, length, false) : NULL;
}

bool CSString::isEqualToString(const char* str) const {
    return str && strcmp(_cstring.c_str(), str) == 0;
}

void CSString::replace(const char *from, const char *to) {
    int pos = 0;
    uint fromlen = strlen(from);
    uint tolen = strlen(from);
    while ((pos = _cstring.find(from, pos)) != std::string::npos) {
        _cstring.replace(pos, fromlen, to);
        pos += tolen;
    }
	dirty();
}

void CSString::clear() {
    _cstring.clear();
    
    dirty();
}

void CSString::append(const char* str) {
    _cstring.append(str);
    
    dirty();
}

void CSString::appendFormatAndArguments(const char* format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    uint length = vsnprintf(NULL, 0, format, copy) + 1;
    va_end(copy);

	char* buf = (char*)alloca(length);
    vsprintf(buf, format, args);
    append(buf);
}

void CSString::appendFormat(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    appendFormatAndArguments(format, ap);
    va_end(ap);
}

void CSString::insert(uint i, const char* str) {
	characters();

    _cstring.insert(_characters->from(i), str);
    
    dirty();
}

void CSString::insertFormatAndArguments(uint i, const char* format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    uint length = vsnprintf(NULL, 0, format, copy) + 1;
    va_end(copy);

	char* buf = (char*)alloca(length);
    vsprintf(buf, format, args);
    insert(i, buf);
}

void CSString::insertFormat(uint i, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    insertFormatAndArguments(i, format, ap);
    va_end(ap);
}

void CSString::removeWithRange(uint index, uint length) {
	characters();

	uint cfrom = _characters->from(index);
	uint cto = _characters->to(index + length - 1);

	_cstring.erase(cfrom, cto - cfrom);

	dirty();
}

void CSString::removeAtIndex(uint i) {
	removeWithRange(i, 1);
}

void CSString::set(const char* str) {
    _cstring = str;
    
    dirty();
}

void CSString::trim() {
	static const char* whitespace = " \t\r\v\n";

	_cstring.erase(_cstring.find_last_not_of(whitespace) + 1);
	_cstring.erase(0, _cstring.find_first_not_of(whitespace));

    dirty();
}

bool CSString::AESEncrypt(const char* key) {
    CSData* data = CSData::dataWithBytes(_cstring.c_str(), _cstring.length());
    
    if (data->AESEncrypt(key, false)) {
        const CSString* enc = data->base64EncodedString();
        
        if (enc) {
            _cstring = *enc;
 
            dirty();
        }
        return true;
    }
    return false;
}

bool CSString::AESDecrypt(const char* key) {
    CSData* data = CSData::dataWithBase64EncodedString(_cstring.c_str());
    
    if (data && data->AESDecrypt(key, false)) {
        _cstring.clear();
        _cstring.append((const char*)data->bytes(), data->length());
        dirty();
        return true;
    }
    return false;
}

CSString* CSString::AESEncrypt(const char* str, const char* key) {
    CSData* data = CSData::dataWithBytes(str, strlen(str));
    
    return data->AESEncrypt(key, false) ? data->base64EncodedString() : NULL;
}

CSString* CSString::AESDecrypt(const char* str, const char* key) {
    CSData* data = CSData::dataWithBase64EncodedString(str);
    
    return data && data->AESDecrypt(key, false) ? CSString::stringWithBytes(data->bytes(), data->length()) : NULL;
}

char* CSString::cstring(const uchar* str, bool free) {
	return cstring(str, u_strlen(str), free);
}

char* CSString::cstring(const uchar* str, uint ulength, bool free) {
	uint capacity = ulength * 4 + 1;
	char* rtn = (char*)calloc(sizeof(char), capacity);
	UErrorCode err = U_ZERO_ERROR;
	u_strToUTF8WithSub(rtn, capacity, NULL, str, ulength, 0x0FFFD, NULL, &err);
	CSAssert(!err, "ucstring error:%d", err);
	if (free) autofree(rtn);
	return rtn;
}

uchar* CSString::ucstring(const char* str, bool free) {
	return ucstring(str, strlen(str), free);
}

uchar* CSString::ucstring(const char* str, uint clength, bool free) {
	uint capacity = clength + 1;
	uchar* rtn = (uchar*)calloc(sizeof(uchar), capacity);
	UErrorCode err = U_ZERO_ERROR;
	u_strFromUTF8WithSub(rtn, capacity, NULL, str, clength, 0x0FFFD, NULL, &err);
	CSAssert(!err, "ucstring error:%d", err);
	if (free) autofree(rtn);
	return rtn;
}

char* CSString::cstring(const char* str, uint clength) {
	char* buf = (char*)fmalloc(clength + 1);
	strncpy(buf, str, clength);
	buf[clength] = 0;
	autofree(buf);
	return buf;
}

char* CSString::cstringWithFormatAndArguments(const char* format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    uint length = vsnprintf(NULL, 0, format, copy) + 1;
    va_end(copy);
    
    char* buf = (char*)fmalloc(length);
    vsprintf(buf, format, args);
    autofree(buf);
    return buf;
}

char* CSString::cstringWithFormat(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    char* rtn = cstringWithFormatAndArguments(format, ap);
    va_end(ap);
    return rtn;
}

#define CSTRING_WITH_FORMAT(value, comma, format, outlen, buflen);	\
	char* out = (char*)fmalloc(outlen);	\
	char dgsp = CSLocaleString::digitGroupSeperator();	\
	if (comma && dgsp) {	\
		char buf[buflen];	\
		sprintf(buf, format, value);	\
		char* outp = out;	\
		int c = 2 - strlen(buf) % 3;	\
		for (char* bufp = buf; *bufp != 0; bufp++) {	\
			*outp++ = *bufp;	\
			if (c == 1) {	\
				*outp++ = dgsp;	\
			}	\
			c = (c + 1) % 3;	\
		}	\
		*--outp = 0;	\
	}	\
	else {	\
		sprintf(out, format, value);	\
	}	\
	autofree(out);	\
	return out;


char* CSString::cstringWithNumber(int value, bool comma) {
	CSTRING_WITH_FORMAT(value, comma, "%d", 15, 12);
}

char* CSString::cstringWithNumber(uint value, bool comma) {
	CSTRING_WITH_FORMAT(value, comma, "%u", 14, 11);
}

char* CSString::cstringWithNumber(int64 value, bool comma) {
	CSTRING_WITH_FORMAT(value, comma, "%" PRId64, 28, 22);
}

char* CSString::cstringWithNumber(uint64 value, bool comma) {
	CSTRING_WITH_FORMAT(value, comma, "%" PRIu64, 27, 21);
}

const char* CSString::cstring(const CSString* str) {
    return str ? str->_cstring.c_str() : NULL;
}

uchar* CSString::ucstring(const CSString* str) {
    return str ? ucstring(str->_cstring.c_str()) : NULL;
}

uint CSString::toCodePoint(const uchar* cc, uint& i) {
    int c1 = cc[i++];
    if (c1 >= 0xd800 && c1 < 0xdc00) {
        int c2 = cc[i++];
        return ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
    }
    return c1;
}

uint CSString::toCodePoint(const char* cc, uint& i) {
    byte u0 = cc[i++];
    if (u0 >= 0 && u0 <= 127) return u0;
    byte u1 = cc[i++];
    if (u0 >= 192 && u0 <= 223) return (u0 - 192) * 64 + (u1 - 128);
    if (u0 == 0xed && (u1 & 0xa0) == 0xa0) return -1;
    byte u2 = cc[i++];
    if (u0 >= 224 && u0 <= 239) return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);
    byte u3 = cc[i++];
    if (u0 >= 240 && u0 <= 247) return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
    return -1;
}

CSString* CSString::sha1() const {
    return CSCrypto::sha1(_cstring.c_str(), _cstring.length());
}

CSString* CSString::sha1(const char* str) {
    return CSCrypto::sha1(str, strlen(str));
}

void CSString::writeTo(CSBuffer* buffer, const CSString* str) {
    if (!str) {
        buffer->writeLength(0);
    }
    else {
        buffer->writeLength(str->_cstring.length());
        buffer->write(str->_cstring.c_str(), str->_cstring.length());
    }
}

void CSString::writeTo(CSBuffer* buffer, const CSString* str, CSStringEncoding encoding) {
    if (!str) {
        buffer->writeLength(0);
    }
    else if (encoding == CSStringEncodingUTF8) {
        buffer->writeLength(str->_cstring.length());
        buffer->write(str->_cstring.c_str(), str->_cstring.length());
    }
    else {
        const CSData* data = str->data(encoding);
        buffer->writeLength(data->length());
        buffer->write(data->bytes(), data->length());
    }
}

void CSString::writeTo(CSBuffer* buffer, const char* str) {
    if (str) {
        uint len = strlen(str);
        buffer->writeLength(len);
        buffer->write(str, len);
    }
    else {
        buffer->writeLength(0);
    }
}

void CSString::writeTo(CSBuffer* buffer) const {
    writeTo(buffer, this);
}

void CSString::writeTo(CSBuffer* buffer, CSStringEncoding encoding) const {
    writeTo(buffer, this, encoding);
}

void CSString::writeToFile(const char* path, const char* str, bool compression) {
    CSFile::writeRawToFile(path, str, strlen(str) + 1, compression);
}

void CSString::writeToFile(const char* path, bool compression) const {
    CSFile::writeRawToFile(path, _cstring.c_str(), _cstring.length() + 1, compression);
}

void CSString::writeToFile(const char* path, CSStringEncoding encoding, bool compression) const {
    if (encoding == CSStringEncodingUTF8) {
        CSFile::writeRawToFile(path, _cstring.c_str(), _cstring.length() + 1, compression);
    }
    else {
        const CSData* data = this->data(encoding);
        
        CSFile::writeRawToFile(path, data->bytes(), data->length(), compression);
    }
}

uint CSString::hash() const {
    if (_hashditry) {
        _hash = std::hash<std::string>()(_cstring);
        _hashditry = false;
    }
    return _hash;
}

CSObject* CSString::copy() const {
    return new CSString(_cstring.c_str());
}

bool CSString::isEqual(const CSObject* object) const {
    if (object == this) return true;
    
    const CSString* str = dynamic_cast<const CSString*>(object);
    
    return isEqualToString(*str);
}

int CSString::compareTo(const CSObject *object) const {
    const CSString* str = dynamic_cast<const CSString*>(object);
    
    return str ? _cstring.compare(str->_cstring) : 0;
}

void CSString::dirty() {
	if (_characters) {
		delete _characters;
		_characters = NULL;
	}
	if (_display) {
		delete _display;
        _display = NULL;
	}
    _hashditry = true;
}

void CSString::hide() {
    for (int i = _cstring.length() - 1; i >= 0 ; i--) {
        _cstring[i] = 0;
    }
    dirty();
}

bool CSString::isEqual(const char* str1, const char* str2) {
    return str1 ? (str2 && strcmp(str1, str2) == 0) : str2 == NULL;
}
