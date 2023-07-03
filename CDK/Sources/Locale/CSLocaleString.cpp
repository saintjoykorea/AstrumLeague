//
//  CSLocaleString.cpp
//  CDK
//
//  Created by Kim Chan on 2016. 8. 10..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLocaleString.h"

#include "CSBuffer.h"

#include "CSBytes.h"

const CSString* CSLocaleDefault = new CSString("en");

uint CSLocaleString::_localeMark = 1;
const CSString* CSLocaleString::_locale = NULL;
char CSLocaleString::_digitGroupSeperator = '\0';

CSLocaleString::LocaleValue::~LocaleValue() {
    release(locale);
    release(value);
}

CSLocaleString::CSLocaleString(){
    _localeValues = new CSArray<LocaleValue>();
}
CSLocaleString::CSLocaleString(CSBuffer* buffer, CSStringEncoding encoding) {
    int localeCount = buffer->readLength();
    _localeValues = new CSArray<LocaleValue>(localeCount ? localeCount : 1);
    for (int i = 0; i < localeCount; i++) {
        LocaleValue& lv = _localeValues->addObject();
        lv.locale = CSString::createWithBuffer(buffer, encoding);
        lv.value = CSString::createWithBuffer(buffer, encoding);
    }
}
CSLocaleString::CSLocaleString(const byte*& data, CSStringEncoding encoding) {
    int localeCount = readLength(data);
    _localeValues = new CSArray<LocaleValue>(localeCount ? localeCount : 1);
    for (int i = 0; i < localeCount; i++) {
        LocaleValue& lv = _localeValues->addObject();
        lv.locale = retain(readString(data, encoding));
        lv.value = retain(readString(data, encoding));
    }
}

CSLocaleString::~CSLocaleString() {
    _localeValues->release();
}
CSLocaleString* CSLocaleString::stringWithBuffer(CSBuffer* buffer) {
    return autorelease(new CSLocaleString(buffer));
}
CSLocaleString* CSLocaleString::stringWithBuffer(CSBuffer* buffer, CSStringEncoding encoding) {
    return autorelease(new CSLocaleString(buffer, encoding));
}

CSString* CSLocaleString::value() {
    if (_currentLocaleMark != _localeMark) {
        _currentLocaleMark = _localeMark;
        _currentLocaleValue = NULL;
        
        if (_locale) {
            CSString* defaultValue = NULL;
            foreach(const LocaleValue&, localeValue, _localeValues) {
                if (localeValue.locale->isEqualToString(_locale)) {
                    _currentLocaleValue = localeValue.value;
                    return _currentLocaleValue;
                }
                if (localeValue.locale->isEqualToString(CSLocaleDefault)) {
                    defaultValue = localeValue.value;
                }
            }
            _currentLocaleValue = defaultValue;
        }
    }
    return _currentLocaleValue;
}
CSString* CSLocaleString::localeValue(const char* locale, bool useDefault) {
    CSString* defaultValue = NULL;
    foreach(const LocaleValue&, localeValue, _localeValues) {
        if (localeValue.locale->isEqualToString(locale)) {
            return localeValue.value;
        }
        if (useDefault && localeValue.locale->isEqualToString(CSLocaleDefault)) {
            defaultValue = localeValue.value;
        }
    }
    return defaultValue;
}
void CSLocaleString::setLocaleValue(const char* locale, const char* value) {
    _currentLocaleMark = 0;
    if (value) {
        foreach(LocaleValue&, lv, _localeValues) {
            if (lv.locale->isEqualToString(locale)) {
                lv.value->release();
                lv.value = new CSString(value);
                return;
            }
        }
        {
            LocaleValue& lv = _localeValues->addObject();
            lv.locale = new CSString(locale);
            lv.value = new CSString(value);
        }
    }
    else {
        for (int i = 0; i < _localeValues->count(); i++) {
            if (_localeValues->objectAtIndex(i).locale->isEqualToString(locale)) {
                _localeValues->removeObjectAtIndex(i);
                break;
            }
        }
    }
}

void CSLocaleString::setLocale(const char* locale) {
    if (!CSString::isEqual(*_locale, locale)) {
        _localeMark++;
        release(_locale);
        _locale = locale ? new CSString(locale) : NULL;
    }
}
