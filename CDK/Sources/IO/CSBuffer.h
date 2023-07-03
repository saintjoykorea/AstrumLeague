//
//  CSBuffer.h
//  CDK
//
//  Created by 김찬 on 12. 8. 27..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSBuffer__
#define __CDK__CSBuffer__

#include "CSData.h"
#include "CSBytes.h"

#include "CSLocaleString.h"
#include "CSDelegate.h"

class CSBuffer : public CSObject {
private:
    CSData* _data;
    
    uint _position;
    uint _markedPosition;
    uint _markedLength;
    bool _marked;
    
public:
    CSBuffer();
    CSBuffer(CSData* data);
    CSBuffer(void* data, uint length, bool copy = true);
    CSBuffer(uint capacity);
private:
    ~CSBuffer();
public:
    static CSBuffer* createWithContentOfFile(const char* path, bool compression = false);
    static CSBuffer* createWithContentOfFile(const char* path, uint offset, uint length, bool compression = false);
    static CSBuffer* createWithContentOfURL(const CSURLRequest* request);
    
    static inline CSBuffer* buffer() {
        return autorelease(new CSBuffer());
    }
    static inline CSBuffer* bufferWithData(CSData* data) {
        return autorelease(new CSBuffer(data));
    }
    static inline CSBuffer* bufferWithBytes(void* data, uint length, bool copy = true) {
        return autorelease(new CSBuffer(data, length, copy));
    }
    static inline CSBuffer* bufferWithCapacity(uint capacity) {
        return autorelease(new CSBuffer(capacity));
    }
    static inline CSBuffer* bufferWithContentOfFile(const char* path, bool compression = false) {
        return autorelease(createWithContentOfFile(path, compression));
    }
    static inline CSBuffer* bufferWithContentOfFile(const char* path, uint offset, uint length, bool compression = false) {
        return autorelease(createWithContentOfFile(path, offset, length, compression));
    }
    static inline CSBuffer* bufferWithContentOfURL(const CSURLRequest* request) {
        return autorelease(createWithContentOfURL(request));
    }
    
    CSData* remainedData() const;
    CSBuffer* remainedBuffer() const;
    
    void clear();
    void mark();
    void rewind();
    void shrink();
    void read(void* data, uint size);
    CSData* readData(uint length);
    int readByte();
    bool readBoolean();
    int readShort();
    int readInt();
    int64 readLong();
    float readFloat();
    fixed readFixed();
    double readDouble();
    uint readLength();
    CSString* readString(CSStringEncoding encoding = CSStringEncodingUTF8);
    CSLocaleString* readLocaleString(CSStringEncoding encoding = CSStringEncodingUTF8);
    
    template <typename V, int dimension = 1>
    CSArray<V, dimension> * readArray(bool nullIfEmpty = true);
    template <typename V, int dimension = 1, typename P>
    CSArray<V, dimension> * readArrayWithParam(P param, bool nullIfEmpty = true);
    //===============================================================
    template <typename V, bool retain = derived<CSObject, V>::value>
    struct ReadArrayFunc {
        typedef V* (*func)(CSBuffer*);
    };
    template <typename V>
    struct ReadArrayFunc<V, false> {
        typedef void (*func)(CSBuffer*, V&);
    };
    template <typename V, typename P, bool retain = derived<CSObject, V>::value>
    struct ReadArrayFuncWithParam {
        typedef V* (*func)(CSBuffer*, P);
    };
    template <typename V, typename P>
    struct ReadArrayFuncWithParam<V, P, false> {
        typedef void (*func)(CSBuffer*, P, V&);
    };
    //===============================================================
    template <typename V, int dimension = 1>
    CSArray<V, dimension> * readArrayFunc(typename ReadArrayFunc<V>::func func, bool nullIfEmpty = true);
    template <typename V, int dimension = 1, typename P>
    CSArray<V, dimension> * readArrayFuncWithParam(typename ReadArrayFuncWithParam<V, P>::func func, P param, bool nullIfEmpty = true);
    
    void write(const void* data, uint size);
    void writeByte(int v);
    void writeBoolean(bool v);
    void writeShort(int v);
    void writeInt(int v);
    void writeLong(int64 v);
    void writeFloat(float v);
    void writeDouble(double v);
    void writeFixed(fixed v);
    void writeLength(uint len);
    void writeString(const char* str);
    void writeString(const CSString* str);
    void writeString(const CSString* str, CSStringEncoding encoding);
    
    template <typename V, int dimension>
    void writeArray(const CSArray<V, dimension>* array);
    template <typename V, int dimension>
    void writeArray(const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*) const);
    template <typename V, int dimension, typename P>
    void writeArray(const CSArray<V, dimension>* array, void (V:: *func)(CSBuffer*, P) const, P param);
    template <typename V, int dimension>
    void writeArray(const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType));
    template <typename V, int dimension, typename P>
    void writeArray(const CSArray<V, dimension>* array, void (*func)(CSBuffer*, typename CSEntryType<V, true>::ConstValueParamType, P), P param);
    
    void writeBuffer(CSBuffer* buffer);
    void writeData(const CSData* data);
    
    bool AESEncrypt(const char* key, bool usingCheckSum);
    bool AESDecrypt(const char* key, bool usingCheckSum);
    
    bool compress();
    bool uncompress();
    
    bool writeToFile(const char* path, bool compression = false);
    
    inline const CSData* data() const {
        return _data;
    }
    
    inline uint position() const {
        return _position;
    }
    
    inline void setPosition(uint position) {
        _position = position;
        CSAssert(_position <= _data->length(), "position overflow");
    }
    
    inline void skip(uint length) {
        _position += length;
        CSAssert(_position <= _data->length(), "position overflow");
    }
    
    inline uint length() const {
        return _data->length();
    }
    
    inline void setLength(uint length) {
        _data->setLength(length);
    }
    
    inline uint remaining() const {
        return _data->length() - _position;
    }
    
    inline const void* bytes() const {
        return (const byte*)_data->bytes() + _position;
    }
    
    inline void* bytes() {
        return (byte*)_data->bytes() + _position;
    }

    CSObject* copy() const override;
};

#include "CSBuffer+array.h"

#endif /* defined(__CDK__CSBuffer__) */
