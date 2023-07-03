//
//  CSObject.h
//  CDK
//
//  Created by 김찬 on 12. 7. 31..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSObject__
#define __CDK__CSObject__

#include "CSTypes.h"
#include "CSMemory.h"
#include "CSLock.h"
#include "CSLog.h"

#include <typeinfo>
#include <atomic>

class CSString;

class CSSynchronizedBlock;

class CSObject {
private:
    std::atomic<uint> _retainCount;
    
    mutable CSLock* _lock;
public:
    CSObject();
protected:
    virtual ~CSObject();
public:
    void release();
    void retain();
    void autorelease();

    inline void release() const {
        const_cast<CSObject*>(this)->release();
    }
    
    inline void retain() const {
        const_cast<CSObject*>(this)->retain();
    }
    
    inline void autorelease() const {
        const_cast<CSObject*>(this)->autorelease();
    }
    
    uint retainCount() const;
    
    static void* operator new(size_t size);
    
    template <class T>
    static inline T* autorelease(T* t) {
        if (t) {
            t->autorelease();
        }
        return t;
    }
    
    template <class T>
    static inline T* retain(T* t) {
        if (t) {
            t->retain();
        }
        return t;
    }
    
    template <class T, class OtherT>
    static inline bool retain(T*& src, OtherT* dest) {
        if (src != dest) {
            release(src);
            src = retain(dest);
            return true;
        }
        return false;
    }
    
    template <class T>
    static inline void release(T*& t) {
        if (t) {
            t->release();
            t = NULL;
        }
    }
    
    virtual CSObject* copy() const;
    
    template <class T>
    static inline T* copy(const T* t) {
        return t ? static_cast<T*>(t->copy()) : NULL;
    }
    
    inline virtual uint hash() const {
        return (uint)(uint64) this;
    }
    
    inline virtual bool isEqual(const CSObject* object) const {
        return this == object;
    }
    
    inline virtual int compareTo(const CSObject* object) const {
        return (int)((uint64)this - (uint64)object);
    }
    
private:
    void useLock() const;
public:
    inline bool assertOnLocked(bool abort = true) const {
#ifdef CS_ASSERT_DEBUG
        CSAssert(!abort || _lock, "should be locked");
        return _lock && _lock->assertOnActive(abort);
#else
        return true;
#endif
    }
    inline bool assertOnUnlocked(bool abort = true) const {
#ifdef CS_ASSERT_DEBUG
        return !_lock || _lock->assertOnDeactive(abort);
#else
        return true;
#endif
    }
    inline bool trylock(int op = 0) const {
        useLock();
        return _lock->trylock(op);
    }
    inline void lock(int op = 0) const {
        useLock();
        _lock->lock(op);
    }
    inline void wait() const {
        CSAssert(_lock, "should be locked");
        _lock->wait();
    }
    inline void wait(float timeout) const {
        CSAssert(_lock, "should be locked");
        _lock->wait(timeout);
    }
    inline void signal() const {
        CSAssert(_lock, "should be locked");
        _lock->signal();
    }
    inline void unlock() const {
        CSAssert(_lock, "should be locked");
        _lock->unlock();
    }
    
    friend class CSSynchronizedBlock;
};

class CSSynchronizedBlock {
private:
    CSLock* _obj;
    bool _flag;
public:
    inline CSSynchronizedBlock(const CSObject* obj, int op = 0, bool commit = true) : _flag(true) {
        CSAssert(obj, "obj is null");
        if (commit) {
            obj->lock(op);
            _obj = obj->_lock;
        }
        else {
            _obj = NULL;
        }
    }
    inline CSSynchronizedBlock(CSLock& obj, int op = 0, bool commit = true) : _flag(true) {
        if (commit) {
            obj.lock(op);
            _obj = &obj;
        }
        else {
            _obj = NULL;
        }
    }
    inline ~CSSynchronizedBlock() {
        if (_obj) _obj->unlock();
    }
    inline operator bool() const {
        return _flag;
    }
	inline bool operator !() const {
		return !_flag;
	}
    inline void next() {
        _flag = false;
    }
};
#define synchronized(...) for (CSSynchronizedBlock __sb_ ## __LINE__(__VA_ARGS__); __sb_ ## __LINE__; __sb_ ## __LINE__.next())

#endif /* defined(__CDK__CSObject__) */
