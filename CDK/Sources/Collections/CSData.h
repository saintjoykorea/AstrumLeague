//
//  CSData.h
//  CDK
//
//  Created by 김찬 on 12. 8. 27..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSData__
#define __CDK__CSData__

#include "CSObject.h"

class CSURLRequest;

class CSData : public CSObject {
protected:
    void* _bytes;
    uint _length;
    uint _capacity;
public:
    CSData();
    CSData(uint capacity);
    CSData(const void* bytes, uint length, bool copy = true);
    static CSData* createWithContentOfFile(const char* path, bool compression = false);
    static CSData* createWithContentOfFile(const char* path, uint offset, uint length, bool compression = false);
    static CSData* createWithContentOfURL(const CSURLRequest* request);
    static CSData* createWithBase64EncodedString(const char* str);
    CSData(const CSData* other);
protected:
    virtual ~CSData();
public:
    static inline CSData* data() {
        return autorelease(new CSData());
    }
    static inline CSData* dataWithCapacity(uint capacity) {
        return autorelease(new CSData(capacity));
    }
    static inline CSData* dataWithBytes(const void* bytes, uint length, bool copy = true) {
        return autorelease(new CSData(bytes, length, copy));
    }
    static inline CSData* dataWithContentOfFile(const char* path, bool compression = false) {
        return autorelease(createWithContentOfFile(path, compression));
    }
    static inline CSData* dataWithContentOfFile(const char* path, uint offset, uint length, bool compression = false) {
        return autorelease(createWithContentOfFile(path, offset, length, compression));
    }
    static inline CSData* dataWithContentOfURL(const CSURLRequest* request) {
        return autorelease(createWithContentOfURL(request));
    }
    static inline CSData* dataWithBase64EncodedString(const char* str) {
        return autorelease(createWithBase64EncodedString(str));
    }
    static inline CSData* dataWithData(const CSData* other) {
        return autorelease(new CSData(other));
    }
    
    inline const void* bytes() const {
        return _bytes;
    }
    
    inline void* bytes() {
        return _bytes;
    }
    
    inline uint length() const {
        return _length;
    }
    
    CSString* base64EncodedString() const;
    bool isEqualToData(const CSData* data) const;
    
    void setLength(uint length);
    void setBytes(const void* bytes, uint length);
    void appendBytes(const void* bytes, uint length);
    void appendData(const CSData* data);
    
    CSData* subdataWithRange(uint offset, uint length) const;
    
    void shrink(uint position);
    
    bool AESEncrypt(const char* key, bool usingCheckSum);
    bool AESDecrypt(const char* key, bool usingCheckSum);
    CSString* sha1() const;
    
    bool compress();
    bool uncompress();
    
    bool writeToFile(const char* path, bool compression = false) const;
    
    uint hash() const override;
    bool isEqual(const CSObject* object) const override;
    CSObject* copy() const override;

    void hide();
private:
    void expand(uint length);
};

#endif /* defined(__CDK__CSData__) */
