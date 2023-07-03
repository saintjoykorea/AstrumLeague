//
//  CSBytes.cpp
//  RTS
//
//  Created by 찬 김 on 11. 7. 20..
//  Copyright 2011 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSBytes.h"

#include "CSData.h"

uint readLength(const byte*& bytes) {
    uint v = 0;
    uint current;
    
    do {
        current = *bytes;
        
        bytes++;
        
        v = (v << 7) | (current & 0x7f);
    } while (current & 0x80);
    
    return v;
}

CSString* readString(const byte*& bytes, CSStringEncoding encoding) {
    uint len = readLength(bytes);
    
    CSString* str = NULL;
    
    if (len) {
        str = CSString::stringWithBytes(bytes, len, encoding);
        bytes += len;
    }
    
    return str;
}

CSLocaleString* readLocaleString(const byte*& bytes, CSStringEncoding encoding) {
    return CSLocaleString::stringWithBytes(bytes, encoding);
}

void writeLength(byte*& bytes, uint length) {
    for (uint i = 21; i > 0; i -= 7) {
        uint current = (length >> i) & 0x7f;
        
        if (current) {
            writeByte(bytes, current | 0x80);
        }
    }
    writeByte(bytes, length & 0x7f);
}

void writeString(byte*& bytes, const char* str) {
    uint len = strlen(str);
    writeLength(bytes, len);
    memcpy(bytes, str, len);
    bytes += len;
}

void writeString(byte*& bytes, const CSString* str, CSStringEncoding encoding) {
    if (!str) {
        writeLength(bytes, 0);
    }
    else if (encoding == CSStringEncodingUTF8) {
        const char* cstr = *str;
        uint len = strlen(cstr);
        writeLength(bytes, len);
        memcpy(bytes, cstr, len);
        bytes += len;
    }
    else {
        const CSData* data = str->data(encoding);
        writeLength(bytes, data->length());
        memcpy(bytes, data->bytes(), data->length());
        bytes += data->length();
    }
}

