//
//  CSJSONObject.cpp
//  CDK
//
//  Created by 김찬 on 13. 6. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSJSONObject.h"

#include "CSJSONArray.h"

#include "CSValue.h"

#include "CSString.h"

#ifdef CS_CONSOLE_DEBUG
#define NotFoundLog(...)    if (essential) CSErrorLog(__VA_ARGS__)
#else
#define NotFoundLog(...);
#endif

bool CSJSONObject::containsKey(const char* key) const {
    return CSDictionary<CSString, CSObject>::containsKey(CSString::string(key));
}

bool CSJSONObject::boolForKey(const char* key, bool essential) const {
    const CSBool* obj = dynamic_cast<const CSBool*>(objectForKey(CSString::string(key)));
    if (obj) {
        return *obj;
    }
    NotFoundLog("invalid boolForKey(%s)", key);
    return false;
}

int64 CSJSONObject::numberForKey(const char* key, bool essential) const {
    const CSLong* obj = dynamic_cast<const CSLong*>(objectForKey(CSString::string(key)));
    if (obj) {
        return *obj;
    }
    NotFoundLog("invalid numberForKey(%s)", key);
    return 0;
}

double CSJSONObject::doubleForKey(const char* key, bool essential) const {
    const CSDouble* obj = dynamic_cast<const CSDouble*>(objectForKey(CSString::string(key)));
    if (obj) {
        return *obj;
    }
    NotFoundLog("invalid doubleForKey(%s)", key);
    return 0.0;
}

const CSString* CSJSONObject::stringForKey(const char* key, bool essential) const {
    const CSString* obj = dynamic_cast<const CSString*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid stringForKey(%s)", key);
    return NULL;
}

const CSJSONArray* CSJSONObject::jsonArrayForKey(const char* key, bool essential) const {
    const CSJSONArray* obj = dynamic_cast<const CSJSONArray*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid jsonArrayForKey(%s)", key);
    return NULL;
}

const CSJSONObject* CSJSONObject::jsonObjectForKey(const char* key, bool essential) const {
    const CSJSONObject* obj = dynamic_cast<const CSJSONObject*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid jsonObjectForKey(%s)", key);
    return NULL;
}

CSString* CSJSONObject::stringForKey(const char* key, bool essential) {
    CSString* obj = dynamic_cast<CSString*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid stringForKey(%s)", key);
    return NULL;
}
CSJSONArray* CSJSONObject::jsonArrayForKey(const char* key, bool essential) {
    CSJSONArray* obj = dynamic_cast<CSJSONArray*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid jsonArrayForKey(%s)", key);
    return NULL;
}
CSJSONObject* CSJSONObject::jsonObjectForKey(const char* key, bool essential) {
    CSJSONObject* obj = dynamic_cast<CSJSONObject*>(objectForKey(CSString::string(key)));
    if (obj) {
        return obj;
    }
    NotFoundLog("invalid jsonObjectForKey(%s)", key);
    return NULL;
}

void CSJSONObject::setBool(const char* key, bool value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), CSBool::value(value));
}
void CSJSONObject::setNumber(const char* key, int64 value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), CSLong::value(value));
}
void CSJSONObject::setDouble(const char* key, double value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), CSDouble::value(value));
}
void CSJSONObject::setString(const char* key, const char* value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), CSString::string(value));
}
void CSJSONObject::setJSONArray(const char* key, const CSJSONArray* value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), value->copy());
}
void CSJSONObject::setJSONObject(const char* key, const CSJSONObject* value) {
    CSDictionary<CSString, CSObject>::setObject(CSString::string(key), value->copy());
}

