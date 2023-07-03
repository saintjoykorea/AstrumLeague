//
//  CSPool.h
//  CDK
//
//  Created by 김찬 on 12. 11. 13..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSPool__
#define __CDK__CSPool__

#include "CSDictionary.h"

#include "CSDelegate.h"

#include "CSThread.h"

#include "CSTime.h"

template <typename V>
struct CSPoolLoadReturn {
    V* obj;
    int param;
    
    inline CSPoolLoadReturn() {
    
    }
private:
    inline CSPoolLoadReturn(V* obj, int param) : obj(obj), param(param) {
    
    }
public:
    static inline CSPoolLoadReturn success(V* obj, int cost) {
        CSAssert(obj && cost >= 0, "invalid operation");
        return CSPoolLoadReturn(obj, cost);
    }

    static inline CSPoolLoadReturn fail(bool popAndRetry) {
        return CSPoolLoadReturn(NULL, popAndRetry);
    }
};

template <typename K, class V>
class CSPoolEntry : public CSObject {
private:
    typename CSEntryType<K, true>::Type _key;
    V* _obj;
    int _cost;
public:
    double timeStamp;       //정확한 시간이 필요하지 않으므로 성능을 위해 메인쓰레드의 시간을 사용한다.

    CSPoolEntry* next;
    CSPoolEntry* prev;
    
public:
    CSPoolEntry(typename CSEntryType<K, true>::ConstValueParamType key, V* obj, int cost) : 
        _key(CSEntryImpl<K>::copy(key)), 
        _obj(retain(obj)), 
        _cost(cost), 
        timeStamp(CSThread::mainThread()->timeStamp())
    {

    }
    
private:
    ~CSPoolEntry() {
        CSEntryImpl<K>::release(_key);
        _obj->release();
    }
    
public:
    inline typename CSEntryType<K, true>::ConstValueParamType key() const {
        return _key;
    }
    
    inline V* object() const {
        return _obj;
    }
    
    inline int cost() const {
        return _cost;
    }
};

template <typename K, class V>
class CSPool : public CSObject {
public:
    typedef CSDelegate0<CSPoolLoadReturn<V>, typename CSEntryType<K, true>::ConstValueParamType> Delegate;
private:
    uint64 _cost;
    uint64 _limit;
    float _timeout;
    Delegate* _delegate;
    CSDictionary<K, CSPoolEntry<K, V>>* _dictionary;
    CSPoolEntry<K, V>* _first;
    CSPoolEntry<K, V>* _last;

public:
    CSPool(Delegate* delegate, uint64 limit = 0);
private:
    ~CSPool();
    
public:
    static CSPool* pool(Delegate* delegate, uint64 limit = 0);
    
    void setLimit(uint64 limit);

    inline uint64 limit() const {
        return _limit;
    }

    inline void setTimeout(float timeout) {
        _timeout = timeout;
    }

    inline float timeout() const {
        return _timeout;
    }
    
    inline uint64 cost() const {
        return _cost;
    }
    
    inline int count() const {
        return _dictionary->count();
    }
    
    V* entry(typename CSEntryType<K, true>::ConstValueParamType key);
    
    void clear();
    bool pop(bool useTimeout = true);
    void purge(uint cost, bool useTimeout = true);
private:
    void addImpl(typename CSEntryType<K, true>::ConstValueParamType key, V* obj, uint cost);
public:
    void add(typename CSEntryType<K, true>::ConstValueParamType key, V* obj, uint cost);
    void remove(typename CSEntryType<K, true>::ConstValueParamType key);
    bool contains(typename CSEntryType<K, true>::ConstValueParamType key) const;
};

template <typename K, class V>
CSPool<K, V>::CSPool(Delegate* delegate, uint64 limit) {
    _delegate = retain(delegate);
    _dictionary = new CSDictionary<K, CSPoolEntry<K, V>>(256);
    _limit = limit;
}

template <typename K, class V>
CSPool<K, V>::~CSPool() {
    _delegate->release();
    _dictionary->release();
}

template <typename K, class V>
CSPool<K, V>* CSPool<K, V>::pool(Delegate* delegate, uint64 limit) {
    return autorelease(new CSPool(delegate, limit));
}

template <typename K, class V>
void CSPool<K, V>::setLimit(uint64 limit) {
    _limit = limit;
    
    if (_limit) {
        while (_cost >= _limit && pop()) ;
    }
}

template <typename K, class V>
V* CSPool<K, V>::entry(typename CSEntryType<K, true>::ConstValueParamType key) {
    CSPoolEntry<K, V>* entry = _dictionary->objectForKey(key);
    
    if (entry) {
        if (entry != _last) {
            if (_first == entry) _first = entry->next;
            if (entry->prev) entry->prev->next = entry->next;
            if (entry->next) entry->next->prev = entry->prev;
            _last->next = entry;
            entry->prev = _last;
            entry->next = NULL;
            _last = entry;
        }
        entry->timeStamp = CSThread::mainThread()->timeStamp();
        return entry->object();
    }
    else {
        if (_limit) {
            while (_cost >= _limit && pop()) ;
        }
        CSPoolLoadReturn<V> rtn;
        
        for (;;) {
            rtn = (*_delegate)(key);
            
            if (rtn.obj) break;
            else if (!rtn.param || !pop()) return NULL;
        }
        
        addImpl(key, rtn.obj, rtn.param);
        
        CSLog("load:(count:%d), (cost:%d), (fill:%llu), (limit:%llu)", _dictionary->count(), rtn.param, _cost, _limit);
        
        return rtn.obj;
    }
}

template <typename K, class V>
void CSPool<K, V>::clear() {
    _cost = 0;
    _dictionary->removeAllObjects();
    _first = NULL;
    _last = NULL;
    CSLog("clear:(limit:%llu)", _limit);
}

template <typename K, class V>
bool CSPool<K, V>::pop(bool useTimeout) {
    if (!_first || (useTimeout && _timeout && CSThread::mainThread()->timeStamp() < _first->timeStamp + _timeout)) return false;

    CSPoolEntry<K, V>* next = _first->next;
    int cost = _first->cost();
    _cost -= cost;
    _dictionary->removeObject(_first->key());
    _first = next;
    if (_first) _first->prev = NULL;
    else _last = NULL;

    CSLog("pop:(count:%d), (cost:%d), (fill:%llu), (limit:%llu)", _dictionary->count(), cost, _cost, _limit);
    return true;
}

template <typename K, class V>
void CSPool<K, V>::purge(uint cost, bool useTimeout) {
    uint64 prevCost = _cost;
    
    while (pop(useTimeout) && prevCost - _cost < cost);
}

template <typename K, class V>
void CSPool<K, V>::addImpl(typename CSEntryType<K, true>::ConstValueParamType key, V* obj, uint cost) {
    if (_limit) {
        while (_cost + cost >= _limit && pop()) ;
    }
    CSPoolEntry<K, V>* entry = new CSPoolEntry<K, V>(key, obj, cost);
    entry->prev = _last;
    if (_last) _last->next = entry;
    _last = entry;
    if (!_first) _first = entry;
    _cost += cost;
    
    _dictionary->setObject(key, entry);
    entry->release();
}

template <typename K, class V>
void CSPool<K, V>::add(typename CSEntryType<K, true>::ConstValueParamType key, V* obj, uint cost) {
    remove(key);
    
    addImpl(key, obj, cost);
    
    CSLog("add:(count:%d), (cost:%d), (fill:%llu), (limit:%llu)", _dictionary->count(), cost, _cost, _limit);
}

template <typename K, class V>
void CSPool<K, V>::remove(typename CSEntryType<K, true>::ConstValueParamType key) {
    CSPoolEntry<K, V>* entry = _dictionary->objectForKey(key);
    
    if (entry) {
        int cost = entry->cost();
        _cost -= cost;
        if (entry->prev) entry->prev->next = entry->next;
        if (entry->next) entry->next->prev = entry->prev;
        if (entry == _first) _first = entry->next;
        if (entry == _last) _last = entry->prev;
        _dictionary->removeObject(key);

        CSLog("remove:(count:%d), (cost:%d), (fill:%llu), (limit:%llu)", _dictionary->count(), cost, _cost, _limit);
    }
}

template <typename K, class V>
bool CSPool<K, V>::contains(typename CSEntryType<K, true>::ConstValueParamType key) const {
    return _dictionary->containsKey(key);
}

#endif /* defined(__CDK__CSPool__) */
