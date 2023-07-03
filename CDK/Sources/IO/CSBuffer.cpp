//
//  CSBuffer.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 27..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBuffer.h"

#include "CSFile.h"

CSBuffer::CSBuffer() {
    _data = new CSData();
}

CSBuffer::CSBuffer(CSData* data) {
    CSAssert(data, "data is NULL");
    
    _data = retain(data);
}

CSBuffer::CSBuffer(void* data, uint length, bool copy) {
    _data = new CSData(data, length, copy);
}

CSBuffer::CSBuffer(uint capacity) {
    _data = new CSData(capacity);
}

CSBuffer::~CSBuffer() {
    _data->release();
}

CSBuffer* CSBuffer::createWithContentOfFile(const char* path, bool compression) {
    CSData* data = CSData::createWithContentOfFile(path, compression);
    CSBuffer* buffer = NULL;
    if (data) {
        buffer = new CSBuffer(data);
        data->release();
    }
    return buffer;
}

CSBuffer* CSBuffer::createWithContentOfFile(const char* path, uint offset, uint length, bool compression) {
    CSData* data = CSData::createWithContentOfFile(path, offset, length, compression);
    CSBuffer* buffer = NULL;
    if (data) {
        buffer = new CSBuffer(data);
        data->release();
    }
    return buffer;
}

CSBuffer* CSBuffer::createWithContentOfURL(const CSURLRequest* request) {
    CSData* data = CSData::createWithContentOfURL(request);
    CSBuffer* buffer = NULL;
    if (data) {
        buffer = new CSBuffer(data);
        data->release();
    }
    return buffer;
}

CSData* CSBuffer::remainedData() const {
    return _position < _data->length() ? _data->subdataWithRange(_position, _data->length() - _position) : NULL;
}

CSBuffer* CSBuffer::remainedBuffer() const {
    uint remainedLength = _data->length() - _position;
    
    return remainedLength > 0 ? CSBuffer::bufferWithBytes((byte*)_data->bytes() + _position, remainedLength) : NULL;
}

void CSBuffer::clear() {
    _position = 0;
    _data->setLength(0);
    _marked = false;
}

void CSBuffer::mark() {
    _markedPosition = _position;
    _markedLength = _data->length();
    _marked = true;
}

void CSBuffer::rewind() {
    if (_marked) {
        _position = _markedPosition;
        _data->setLength(_markedLength);
    }
}

void CSBuffer::shrink() {
    if (_position) {
        _data->shrink(_position);
        _position = 0;
    }
}

void CSBuffer::read(void* data, uint size) {
    CSAssert(_position <= _data->length() - size, "position overflow");
    memcpy(data, (const byte*)_data->bytes() + _position, size);
    _position += size;
}

CSData* CSBuffer::readData(uint length) {
    byte* data = (byte*)fmalloc(length);
    CSAssert(_position <= _data->length() - length, "position overflow");
    memcpy(data, (const byte*)_data->bytes() + _position, length);
    _position += length;
    
    return CSData::dataWithBytes(data, length, false);
}

int CSBuffer::readByte() {
    CSAssert(_position <= _data->length() - 1, "position overflow");
    return ((const sbyte*)_data->bytes())[_position++];
}

bool CSBuffer::readBoolean() {
    return readByte() != 0;
}

int CSBuffer::readShort() {
    CSAssert(_position <= _data->length() - 2, "position overflow");
    int v = bytesToShort((const byte*)_data->bytes() + _position);
    _position += 2;
    return v;
}

int CSBuffer::readInt() {
    CSAssert(_position <= _data->length() - 4, "position overflow");
    int v = bytesToInt((const byte*)_data->bytes() + _position);
    _position += 4;
    return v;
}

int64 CSBuffer::readLong() {
    CSAssert(_position <= _data->length() - 8, "position overflow");
    int64 v = bytesToLong((const byte*)_data->bytes() + _position);
    _position += 8;
    return v;
}

float CSBuffer::readFloat() {
    CSAssert(_position <= _data->length() - 4, "position overflow");
    float v = bytesToFloat((const byte*)_data->bytes() + _position);
    _position += 4;
    return v;
}

double CSBuffer::readDouble() {
    CSAssert(_position <= _data->length() - 8, "position overflow");
    double v = bytesToDouble((const byte*)_data->bytes() + _position);
    _position += 8;
    return v;
}

fixed CSBuffer::readFixed() {
    CSAssert(_position <= _data->length() - 8, "position overflow");
    fixed v = bytesToFixed((const byte*)_data->bytes() + _position);
    _position += 8;
    return v;
}

uint CSBuffer::readLength() {
    uint v = 0;
    uint current;
    
    do {
        CSAssert(_position <= _data->length() - 1, "position overflow");
        current = ((const byte*)_data->bytes())[_position++];
        
        v = (v << 7) | (current & 0x7f);
    } while (current & 0x80);
    CSAssert(_position <= _data->length(), "position overflow");
    return v;
}

CSString* CSBuffer::readString(CSStringEncoding encoding) {
    return CSString::stringWithBuffer(this, encoding);
}

CSLocaleString* CSBuffer::readLocaleString(CSStringEncoding encoding) {
    return CSLocaleString::stringWithBuffer(this, encoding);
}

void CSBuffer::write(const void* data, uint size) {
    _data->appendBytes((const byte*)data, size);
}

void CSBuffer::writeByte(int v) {
    _data->appendBytes(&v, 1);
}

void CSBuffer::writeBoolean(bool v) {
    writeByte(v);
}

void CSBuffer::writeShort(int v) {
    _data->appendBytes(&v, 2);
}

void CSBuffer::writeInt(int v) {
    _data->appendBytes(&v, 4);
}

void CSBuffer::writeLong(int64 v) {
    _data->appendBytes(&v, 8);
}

void CSBuffer::writeFloat(float v) {
    _data->appendBytes(&v, 4);
}

void CSBuffer::writeDouble(double v) {
    _data->appendBytes(&v, 8);
}

void CSBuffer::writeFixed(fixed v) {
    _data->appendBytes(&v.raw, 8);
}

void CSBuffer::writeLength(uint len) {
    for (uint i = 21; i > 0; i -= 7) {
        uint current = (len >> i) & 0x7f;
        
        if (current) {
            writeByte(current | 0x80);
        }
    }
    writeByte(len & 0x7f);
}

void CSBuffer::writeString(const char* str) {
    CSString::writeTo(this, str);
}

void CSBuffer::writeString(const CSString* str) {
    CSString::writeTo(this, str);
}

void CSBuffer::writeString(const CSString* str, CSStringEncoding encoding) {
    CSString::writeTo(this, str, encoding);
}

void CSBuffer::writeBuffer(CSBuffer* buffer) {
    _data->appendBytes(buffer->bytes(), buffer->remaining());
    buffer->setPosition(buffer->length());
}

void CSBuffer::writeData(const CSData* data) {
    _data->appendData(data);
}

bool CSBuffer::AESEncrypt(const char* key, bool usingCheckSum) {
    if (_data->AESEncrypt(key, usingCheckSum)) {
        _position = 0;
        _marked = false;
        return true;
    }
    return false;
}
bool CSBuffer::AESDecrypt(const char* key, bool usingCheckSum) {
    if (_data->AESDecrypt(key, usingCheckSum)) {
        _position = 0;
        _marked = false;
        return true;
    }
    return false;
}

bool CSBuffer::compress() {
    if (_data->compress()) {
        _position = 0;
        _marked = false;
        return true;
    }
    return false;
}

bool CSBuffer::uncompress() {
    if (_data->uncompress()) {
        _position = 0;
        _marked = false;
        return true;
    }
    return false;
}

bool CSBuffer::writeToFile(const char* path, bool compression) {
    return _data->writeToFile(path, compression);
}

CSObject* CSBuffer::copy() const {
    return new CSBuffer(_data->bytes(), _data->length());
}
