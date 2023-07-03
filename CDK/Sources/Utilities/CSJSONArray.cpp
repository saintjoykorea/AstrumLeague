//
//  CSJSONArray.cpp
//  CDK
//
//  Created by 김찬 on 13. 6. 25..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSJSONArray.h"

#include "CSJSONObject.h"

#include "CSValue.h"

#include "CSString.h"

bool CSJSONArray::boolAtIndex(uint index) const {
    const CSBool* obj = dynamic_cast<const CSBool*>(objectAtIndex(index));
    
    if (obj) {
        return *obj;
    }
    CSWarnLog("invalid boolAtIndex(%d)", index);
    return false;
}

int64 CSJSONArray::numberAtIndex(uint index) const {
    const CSLong* obj = dynamic_cast<const CSLong*>(objectAtIndex(index));
    
    if (obj) {
        return *obj;
    }
    CSWarnLog("invalid numberAtIndex(%d)", index);
    return 0;
}

double CSJSONArray::doubleAtIndex(uint index) const {
    const CSDouble* obj = dynamic_cast<const CSDouble*>(objectAtIndex(index));
    
    if (obj) {
        return *obj;
    }
    CSWarnLog("invalid doubleAtIndex(%d)", index);
    return 0.0;
}

const CSString* CSJSONArray::stringAtIndex(uint index) const {
    const CSString* obj = dynamic_cast<const CSString*>(objectAtIndex(index));
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid stringAtIndex(%d)", index);
    }
#endif
    return obj;
}

const CSJSONArray* CSJSONArray::jsonArrayAtIndex(uint index) const {
    const CSJSONArray* obj = dynamic_cast<const CSJSONArray*>(objectAtIndex(index));
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid jsonArrayAtIndex(%d)", index);
    }
#endif
    return obj;
}

const CSJSONObject* CSJSONArray::jsonObjectAtIndex(uint index) const {
    const CSJSONObject* obj = dynamic_cast<const CSJSONObject*>(objectAtIndex(index));
    
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid jsonObjectAtIndex(%d)", index);
    }
#endif
    return obj;
}

CSString* CSJSONArray::stringAtIndex(uint index) {
    CSString* obj = dynamic_cast<CSString*>(objectAtIndex(index));
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid stringAtIndex(%d)", index);
    }
#endif
    return obj;
}
CSJSONArray* CSJSONArray::jsonArrayAtIndex(uint index) {
    CSJSONArray* obj = dynamic_cast<CSJSONArray*>(objectAtIndex(index));
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid jsonArrayAtIndex(%d)", index);
    }
#endif
    return obj;
}
CSJSONObject* CSJSONArray::jsonObjectAtIndex(uint index) {
    CSJSONObject* obj = dynamic_cast<CSJSONObject*>(objectAtIndex(index));
    
#ifdef CS_CONSOLE_DEBUG
    if (!obj) {
        CSWarnLog("invalid jsonObjectAtIndex(%d)", index);
    }
#endif
    return obj;
}

void CSJSONArray::addBool(bool value) {
    CSArray<CSObject>::addObject(CSBool::value(value));
}
void CSJSONArray::addNumber(int64 value) {
    CSArray<CSObject>::addObject(CSLong::value(value));
}
void CSJSONArray::addDouble(double value) {
    CSArray<CSObject>::addObject(CSDouble::value(value));
}
void CSJSONArray::addString(const char* value) {
    CSArray<CSObject>::addObject(CSString::string(value));
}
void CSJSONArray::addJSONArray(const CSJSONArray* value) {
    CSArray<CSObject>::addObject(value->copy());
}
void CSJSONArray::addJSONObject(const CSJSONObject* value) {
    CSArray<CSObject>::addObject(value->copy());
}

void CSJSONArray::insertBool(uint index, bool value) {
    CSArray<CSObject>::insertObject(index, CSBool::value(value));
}
void CSJSONArray::insertNumber(uint index, int64 value) {
    CSArray<CSObject>::insertObject(index, CSLong::value(value));
}
void CSJSONArray::insertDouble(uint index, double value) {
    CSArray<CSObject>::insertObject(index, CSDouble::value(value));
}
void CSJSONArray::insertString(uint index, const char* value) {
    CSArray<CSObject>::insertObject(index, CSString::string(value));
}
void CSJSONArray::insertJSONArray(uint index, const CSJSONArray* value) {
    CSArray<CSObject>::insertObject(index, value->copy());
}
void CSJSONArray::insertJSONObject(uint index, const CSJSONObject* value) {
    CSArray<CSObject>::insertObject(index, value->copy());
}
