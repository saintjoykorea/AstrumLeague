//
//  CSSecret.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 9. 8..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef CSSecret_h
#define CSSecret_h

#include "CSArray.h"

#include "CSHandler.h"

#define CSSecretCapacity   127

class CSSecret;

class CSSecretValueBase {
protected:
    inline virtual ~CSSecretValueBase() {
        
    }

    virtual void reset(const uint64* table, uint64 valueMask, uint64 checkMask) = 0;
    
    friend class CSSecret;
};

class CSSecret {
public:
    uint64 table[CSSecretCapacity];
    uint64 valueMask;
    uint64 checkMask;

	CSHandler<> OnError;
private:
#ifdef CS_SECRET_THREAD_SAFE
    CSLock _lock;
#endif
    CSArray<CSSecretValueBase*>* _values;
    
    static CSSecret _sharedSecret;
private:
    CSSecret();
    ~CSSecret();
public:
    static inline CSSecret* sharedSecret() {
        return &_sharedSecret;
    }
#ifdef CS_SECRET_THREAD_SAFE
    inline void lock() {
        _lock.lock();
    }
    inline void unlock() {
        _lock.unlock();
    }
#else
    inline void lock() {}
    inline void unlock() {}
#endif
    void addValue(CSSecretValueBase* value);
    void removeValue(CSSecretValueBase* value);
    void clear();
    void reset(bool all);
    void occurError();
    
    inline uint valueCount() const {
        return _values->count();
    }
};

#endif /* CSSecret_h */
