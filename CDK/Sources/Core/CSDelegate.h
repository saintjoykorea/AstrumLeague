//
//  CSDelegate.h
//  CDK
//
//  Created by 김찬 on 12. 11. 29..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSDelegate__
#define __CDK__CSDelegate__

#include "CSMember.h"

#include <functional>

template <typename Ret, typename ... Params>
class CSDelegate : public CSObject {
protected:
	const void* _object;

	inline CSDelegate(const void* obj) : _object(obj) {}
	inline virtual ~CSDelegate() {}
public:
	virtual Ret operator ()(Params... params) = 0;
	inline const void* object() const {
		return _object;
	}
};

template <typename Ret, typename ... Params>
class CSDelegate0 : public CSDelegate<Ret, Params...> {
private:
	std::function<Ret(Params...)> _func;
public:
	inline CSDelegate0(std::function<Ret(Params...)> func) : CSDelegate<Ret, Params...>(NULL), _func(func) {}

	template <class Object>
	inline CSDelegate0(Object* obj, Ret(Object::*method)(Params...)) : CSDelegate<Ret, Params...>(obj) {
		_func = [obj, method](Params... params) -> Ret {
			return (obj->*method)(params...);
		};
	}
private:
	inline ~CSDelegate0() {}
public:
	static inline CSDelegate0<Ret, Params...>* delegate(std::function<Ret(Params...)> func) {
		return CSObject::autorelease(new CSDelegate0<Ret, Params...>(func));
	}

	template <class Object>
	static inline CSDelegate0<Ret, Params...>* delegate(Object* obj, Ret(Object::*method)(Params...)) {
		return CSObject::autorelease(new CSDelegate0<Ret, Params...>(obj, method));
	}

	inline Ret operator ()(Params... params) override {
		return _func(params...);
	}
};

template <typename Ret, typename Param0, typename ... Params>
class CSDelegate1 : public CSDelegate<Ret, Params...> {
private:
	std::function<Ret(Params..., Param0)> _func;
	Param0 _param0;
public:
	inline CSDelegate1(std::function<Ret(Params..., Param0)> func) : CSDelegate<Ret, Params...>(NULL), _func(func) {}

	template <class Object>
	inline CSDelegate1(Object* obj, Ret(Object::*method)(Params..., Param0)) : CSDelegate<Ret, Params...>(obj) {
		_func = [obj, method](Params... params, Param0 param0) -> Ret {
			return (obj->*method)(params..., param0);
		};
	}

	inline CSDelegate1(std::function<Ret(Params..., Param0)> func, Param0 param0) : CSDelegate1(func) {
		CSMember::retain(_param0, param0);
	}

	template <class Object>
	inline CSDelegate1(Object* obj, Ret(Object::*method)(Params..., Param0), Param0 param0) : CSDelegate1(obj, method) {
		CSMember::retain(_param0, param0);
	}
private:
	inline ~CSDelegate1() {
		CSMember::release(_param0);
	}
public:
	static inline CSDelegate1<Ret, Param0, Params...>* delegate(std::function<Ret(Params..., Param0)> func) {
		return CSObject::autorelease(new CSDelegate1<Ret, Param0, Params...>(func));
	}

	template <class Object>
	static inline CSDelegate1<Ret, Param0, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0)) {
		return CSObject::autorelease(new CSDelegate1<Ret, Param0, Params...>(obj, method));
	}

	static inline CSDelegate1<Ret, Param0, Params...>* delegate(std::function<Ret(Params..., Param0)> func, Param0 param0) {
		return CSObject::autorelease(new CSDelegate1<Ret, Param0, Params...>(func, param0));
	}

	template <class Object>
	static inline CSDelegate1<Ret, Param0, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0), Param0 param0) {
		return CSObject::autorelease(new CSDelegate1<Ret, Param0, Params...>(obj, method, param0));
	}

	inline Ret operator ()(Params... params) override {
		return _func(params..., _param0);
	}

	inline void setParam0(Param0 param0) {
		CSMember::retain(_param0, param0);
	}

	inline Param0 param0() const {
		return _param0;
	}
};

template <typename Ret, typename Param0, typename Param1, typename ... Params>
class CSDelegate2 : public CSDelegate<Ret, Params...> {
private:
	std::function<Ret(Params..., Param0, Param1)> _func;
	Param0 _param0;
	Param1 _param1;
public:
	inline CSDelegate2(std::function<Ret(Params..., Param0, Param1)> func) : CSDelegate<Ret, Params...>(NULL), _func(func) {}

	template <class Object>
	inline CSDelegate2(Object* obj, Ret(Object::*method)(Params..., Param0, Param1)) : CSDelegate<Ret, Params...>(obj) {
		_func = [this, obj, method](Params... params, Param0 param0, Param1 param1) -> Ret {
			return (obj->*method)(params..., param0, param1);
		};
	}

	inline CSDelegate2(std::function<Ret(Params..., Param0, Param1)> func, Param0 param0, Param1 param1) : CSDelegate2(func) {
		CSMember::retain(_param0, param0);
		CSMember::retain(_param1, param1);
	}

	template <class Object>
	inline CSDelegate2(Object* obj, Ret(Object::*method)(Params..., Param0, Param1), Param0 param0, Param1 param1) : CSDelegate2(obj, method) {
		CSMember::retain(_param0, param0);
		CSMember::retain(_param1, param1);
	}
private:
	inline ~CSDelegate2() {
		CSMember::release(_param0);
		CSMember::release(_param1);
	}
public:
	static inline CSDelegate2<Ret, Param0, Param1, Params...>* delegate(std::function<Ret(Params..., Param0, Param1)> func) {
		return CSObject::autorelease(new CSDelegate2<Ret, Param0, Param1, Params...>(func));
	}

	template <class Object>
	static inline CSDelegate2<Ret, Param0, Param1, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0, Param1)) {
		return CSObject::autorelease(new CSDelegate2<Ret, Param0, Param1, Params...>(obj, method));
	}

	static inline CSDelegate2<Ret, Param0, Param1, Params...>* delegate(std::function<Ret(Params..., Param0, Param1)> func, Param0 param0, Param1 param1) {
		return CSObject::autorelease(new CSDelegate2<Ret, Param0, Param1, Params...>(func, param0, param1));
	}

	template <class Object>
	static inline CSDelegate2<Ret, Param0, Param1, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0, Param1), Param0 param0, Param1 param1) {
		return CSObject::autorelease(new CSDelegate2<Ret, Param0, Param1, Params...>(obj, method, param0, param1));
	}

	inline Ret operator ()(Params... params) override {
		return _func(params..., _param0, _param1);
	}

	inline void setParam0(Param0 param0) {
		CSMember::retain(_param0, param0);
	}

	inline Param0 param0() const {
		return _param0;
	}

	inline void setParam1(Param1 param1) {
		CSMember::retain(_param1, param1);
	}

	inline Param1 param1() const {
		return _param1;
	}
};

template <typename Ret, typename Param0, typename Param1, typename Param2, typename ... Params>
class CSDelegate3 : public CSDelegate<Ret, Params...> {
private:
	std::function<Ret(Params..., Param0, Param1, Param2)> _func;
	Param0 _param0;
	Param1 _param1;
	Param2 _param2;
public:
	inline CSDelegate3(std::function<Ret(Params..., Param0, Param1, Param2)> func) : CSDelegate<Ret, Params...>(NULL), _func(func) {}

	template <class Object>
	inline CSDelegate3(Object* obj, Ret(Object::*method)(Params..., Param0, Param1, Param2)) : CSDelegate<Ret, Params...>(obj) {
		_func = [this, obj, method](Params... params, Param0 param0, Param1 param1, Param2 param2) -> Ret {
			return (obj->*method)(params..., param0, param1, param2);
		};
	}

	inline CSDelegate3(std::function<Ret(Params..., Param0, Param1, Param2)> func, Param0 param0, Param1 param1, Param2 param2) : CSDelegate3(func) {
		CSMember::retain(_param0, param0);
		CSMember::retain(_param1, param1);
		CSMember::retain(_param2, param2);
	}

	template <class Object>
	inline CSDelegate3(Object* obj, Ret(Object::*method)(Params..., Param0, Param1, Param2), Param0 param0, Param1 param1, Param2 param2) : CSDelegate3(obj, method) {
		CSMember::retain(_param0, param0);
		CSMember::retain(_param1, param1);
		CSMember::retain(_param2, param2);
	}
private:
	inline ~CSDelegate3() {
		CSMember::release(_param0);
		CSMember::release(_param1);
		CSMember::release(_param2);
	}
public:
	static inline CSDelegate3<Ret, Param0, Param1, Param2, Params...>* delegate(std::function<Ret(Params..., Param0, Param1, Param2)> func) {
		return CSObject::autorelease(new CSDelegate3<Ret, Param0, Param1, Param2, Params...>(func));
	}

	template <class Object>
	static inline CSDelegate3<Ret, Param0, Param1, Param2, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0, Param1, Param2)) {
		return CSObject::autorelease(new CSDelegate3<Ret, Param0, Param1, Param2, Params...>(obj, method));
	}

	static inline CSDelegate3<Ret, Param0, Param1, Param2, Params...>* delegate(std::function<Ret(Params..., Param0, Param1, Param2)> func, Param0 param0, Param1 param1, Param2 param2) {
		return CSObject::autorelease(new CSDelegate3<Ret, Param0, Param1, Param2, Params...>(func, param0, param1, param2));
	}

	template <class Object>
	static inline CSDelegate3<Ret, Param0, Param1, Param2, Params...>* delegate(Object* obj, Ret(Object::*method)(Params..., Param0, Param1, Param2), Param0 param0, Param1 param1, Param2 param2) {
		return CSObject::autorelease(new CSDelegate3<Ret, Param0, Param1, Param2, Params...>(obj, method, param0, param1, param2));
	}

	inline Ret operator ()(Params... params) override {
		return _func(params..., _param0, _param1, _param2);
	}

	inline void setParam0(Param0 param0) {
		CSMember::retain(_param0, param0);
	}

	inline Param0 param0() const {
		return _param0;
	}

	inline void setParam1(Param1 param1) {
		CSMember::retain(_param1, param1);
	}

	inline Param1 param1() const {
		return _param1;
	}

	inline void setParam2(Param2 param2) {
		CSMember::retain(_param2, param2);
	}

	inline Param2 param2() const {
		return _param2;
	}
};

#endif /* defined(__CDK__CSDelegate__) */
