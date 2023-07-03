//
//  CSJSONParser.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSJSONParser.h"

#include "CSJSONArray.h"

#include "CSString.h"
#include "CSValue.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

static CSObject* objectWithValueIterator(const Value& value) {
    switch (value.GetType()) {
        case kNullType:
            return NULL;
            
        case kFalseType:
            return CSBool::value(false);
            
        case kTrueType:
            return CSBool::value(true);
            
        case kObjectType:
            {
                CSJSONObject* dic = CSJSONObject::object();
                for (Value::ConstMemberIterator i = value.MemberBegin(); i != value.MemberEnd(); ++i) {
                    CSObject* obj = objectWithValueIterator(i->value);
                    if (obj) {
                        dic->setObject(CSString::string(i->name.GetString()), obj);
                    }
                }
                return dic;
            }
        case kArrayType:
            {
                CSJSONArray* arr = CSJSONArray::array();
                for (SizeType i = 0; i < value.Size(); i++) {
                    arr->addObject(objectWithValueIterator(value[i]));  // nullable obj
                }
                return arr;
            }
            
        case kStringType:
            return CSString::string(value.GetString());
            
        case kNumberType:
            if (value.IsDouble()) {
                return CSDouble::value(value.GetDouble());
            }
            return CSLong::value(value.GetInt64());
    }
    CSAssert(false, "unknown type");
    return NULL;
}

CSJSONObject* CSJSONParser::parse(const char* json) {
    Document d;
    
    char* jsondup = strdup(json);
    
    if (d.ParseInsitu<0>(jsondup).HasParseError()) {
        free(jsondup);
        return NULL;
    }
    
    CSJSONObject* object = CSJSONObject::object();
    
    for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i) {
        CSObject* obj = objectWithValueIterator(i->value);
        if (obj) {
            object->setObject(CSString::string(i->name.GetString()), obj);
        }
    }
    
    free(jsondup);
    
    return object;
}

