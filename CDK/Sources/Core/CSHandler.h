//
//  CSHandler.h
//  CDK
//
//  Created by 김찬 on 14. 1. 9..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSHandler__
#define __CDK__CSHandler__

#include "CSArray.h"

#include "CSDelegate.h"

template <typename ... Params>
class CSHandler {
public:
    typedef CSDelegate<void, Params...> Delegate;

	typedef CSDelegate0<void, Params...> Delegate0;
	template <typename Param0>
	using Delegate1 = CSDelegate1<void, Param0, Params...>;
	template <typename Param0, typename Param1>
	using Delegate2 = CSDelegate2<void, Param0, Param1, Params...>;
	template <typename Param0, typename Param1, typename Param2>
	using Delegate3 = CSDelegate3<void, Param0, Param1, Param2, Params...>;
protected:
    CSArray<Delegate>* _delegates;
public:
	inline CSHandler() : _delegates(NULL) {
        
    }
	inline virtual ~CSHandler() {
        if (_delegates) _delegates->release();
    }
	inline void add(Delegate* delegate) {
        if (!_delegates) _delegates = new CSArray<Delegate>(1);
        _delegates->addObject(delegate);
    }
	inline void add(std::function<void(Params...)> func) {
		add(Delegate0::delegate(func));
	}
    template <class Object>
	inline void add(Object* obj, void(Object::*method)(Params...)) {
        add(Delegate0::delegate(obj, method));
    }
    inline void remove(const Delegate* delegate) {
        if (_delegates) _delegates->removeObjectIdenticalTo(delegate);
    }
    void remove(const void* obj) {
        if (_delegates) {
            int i = 0;
            while (i < _delegates->count()) {
                if (_delegates->objectAtIndex(i)->object() == obj) {
                    _delegates->removeObjectAtIndex(i);
                }
                else {
                    i++;
                }
            }
        }
    }
    inline void clear() {
        if (_delegates) _delegates->removeAllObjects();
    }
    void operator()(Params... params) {
        if (_delegates) {
			if (_delegates->count() == 1) {
				(*_delegates->objectAtIndex(0))(params...);
			}
			else if (_delegates->count() > 1) {
				CSArray<Delegate>* delegates = new CSArray<Delegate>(_delegates, false);
				foreach(Delegate*, delegate, delegates) {
					(*delegate)(params...);
				}
				delegates->release();
			}
        }
    }
    inline bool exists() const {
        return _delegates && _delegates->count() != 0;
    }
};

#endif /* defined(__CDK__CSHandler__) */
