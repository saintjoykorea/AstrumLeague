//
//  CSData.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 27..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSData.h"
#include "CSBytes.h"

#include "CSLog.h"

#include "CSMath.h"

#include "CSFile.h"

#include "CSURLConnection.h"

#include "CSCrypto.h"

CSData::CSData() : _capacity(256) {
    _bytes = fmalloc(_capacity);
}

CSData::CSData(uint capacity) : _capacity(capacity) {
    CSAssert(_capacity, "invalid capacity");
    _bytes = fmalloc(_capacity);
}

CSData::CSData(const void* bytes, uint length, bool copy) {
    CSAssert(bytes, "data is NULL");
    if (copy) {
        _bytes = fmalloc(length);
        memcpy(_bytes, bytes, length);
    }
    else {
        _bytes = const_cast<void*>(bytes);
    }
    _capacity = _length = length;
}

CSData* CSData::createWithContentOfFile(const char* path, bool compression) {
    uint length = 0;
    void* bytes = CSFile::createRawWithContentOfFile(path, 0, length, compression);
    return bytes ? new CSData(bytes, length, false) : NULL;
}

CSData* CSData::createWithContentOfFile(const char* path, uint offset, uint length, bool compression) {
    void* bytes = CSFile::createRawWithContentOfFile(path, offset, length, compression);
    return bytes ? new CSData(bytes, length, false) : NULL;
}

CSData* CSData::createWithContentOfURL(const CSURLRequest* request) {
    return retain(const_cast<CSData*>(CSURLConnection::sendSynchronousRequest(request)));
}

CSData::CSData(const CSData* other) {
    _capacity = _length = other->_length;
    _bytes = fmalloc(_capacity);
    memcpy(_bytes, other->_bytes, _length);
}

CSData::~CSData() {
    free(_bytes);
}


bool CSData::isEqualToData(const CSData* data) const {
    if (_length != data->_length) {
        return false;
    }
    return memcmp(_bytes, data->_bytes, _length) == 0;
}

void CSData::setLength(uint length) {
    if (_length < length) {
        expand(length - _length);
    }
    _length = length;
}

void CSData::setBytes(const void* bytes, uint length) {
    _length = 0;
    appendBytes(bytes, length);
}

void CSData::expand(uint length) {
    uint capacity = _capacity;
    while (_length + length > capacity) {
        capacity *= 2;
    }
    if (capacity != _capacity) {
        _capacity = capacity;
        _bytes = frealloc(_bytes, capacity);
    }
}

void CSData::appendBytes(const void* bytes, uint length) {
    expand(length);
    memcpy((byte*)_bytes + _length, bytes, length);
    _length += length;
}

void CSData::appendData(const CSData* data) {
    appendBytes(data->_bytes, data->_length);
}

CSData* CSData::subdataWithRange(uint offset, uint length) const {
    CSAssert(offset + length <= _length, "data offset overflow");
    
    return CSData::dataWithBytes((byte*)_bytes + offset, length);
}

void CSData::shrink(uint position) {
    if (position) {
        CSAssert(position <= _length, "data offset overflow");
        if (position < _length) {
            memmove(_bytes, (byte*)_bytes + position, _length - position);
            _length -= position;
        }
        else {
            _length = 0;
        }
    }
}

bool CSData::AESEncrypt(const char* key, bool usingCheckSum) {
    if (usingCheckSum) {
        uint checkSumLength = CSMath::min(_length / 4, 256U);
        
        uint checkSum = 0;

        const byte* ptr = (const byte*)_bytes;
        for (uint i = 0; i < checkSumLength; i++) {
            checkSum ^= (uint)readInt(ptr);
        }
        
        appendBytes(&checkSum, 4);
    }
    uint length = _length;
    const void* bytes = CSCrypto::encrypt(_bytes, length, key);
    if (bytes) {
        _length = 0;
        appendBytes(bytes, length);
        return true;
    }
    if (usingCheckSum) {
        _length -= 4;
    }
    return false;
}
bool CSData::AESDecrypt(const char* key, bool usingCheckSum) {
    uint length = _length;
    const void* bytes = CSCrypto::decrypt(_bytes, length, key);
    if (bytes) {
        if (usingCheckSum) {
            if (length < 4) {
                return false;
            }
            uint checkSumLength = CSMath::min((length - 4) / 4, 256U);
            
            uint checkSum = 0;

            const byte* ptr = (const byte*)bytes;
            for (int i = 0; i < checkSumLength; i++) {
                checkSum ^= (uint)readInt(ptr);
            }
            
            uint dataCheckSum = bytesToInt((const byte*)bytes + length - 4);
            
            if (checkSum != dataCheckSum) {
                return false;
            }
            length -= 4;
        }
        _length = 0;
        appendBytes(bytes, length);
        return true;
    }
    return false;
}

CSString* CSData::sha1() const {
    return CSCrypto::sha1(_bytes, _length);
}

bool CSData::compress() {
    uint compLength = 0;
    void* comp = CSFile::createCompressedRawWithData(_bytes, _length, compLength);
    if (comp) {
        free(_bytes);
        _bytes = comp;
        _capacity = _length = compLength;
        return true;
    }
    return false;
}

bool CSData::uncompress() {
    uint uncompLength = 0;
    void* uncomp = CSFile::createRawWithCompressedData(_bytes, _length, 0, uncompLength);
    if (uncomp) {
        free(_bytes);
        _bytes = uncomp;
        _capacity = _length = uncompLength;
        return true;
    }
    return false;
}

bool CSData::writeToFile(const char* path, bool compression) const {
    return CSFile::writeRawToFile(path, _bytes, _length, compression);
}

uint CSData::hash() const {                //Jenkins'One at a Time hash
    uint length = CSMath::min(_length, 10U);
    
    const byte* ptr = (const byte*)_bytes;
    
    uint hash = 0;
    for (uint i = 0; i < length; i++) {
        hash += ptr[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    return hash;
}

bool CSData::isEqual(const CSObject* object) const {
    const CSData* data = dynamic_cast<const CSData*>(object);
    
    return data && isEqualToData(data);
}

CSObject* CSData::copy() const {
    return new CSData(this);
}

void CSData::hide() {
    memset(_bytes, 0, _capacity);
}

