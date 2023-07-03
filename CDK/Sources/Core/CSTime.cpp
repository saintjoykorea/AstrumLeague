//
//  CSTime.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 10..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTime.h"

#include <time.h>
#include <sys/time.h>

CSTime::CSTime() {
    time_t sec;
    time(&sec);
	setTime(sec);
}

CSTime::CSTime(time_t sec) {
	setTime(sec);
}

CSTime::CSTime(int year, int month, int day, int hour, int minute, int second) {
	setTime(year, month, day, hour, minute, second);
}

void CSTime::setTime(time_t sec) {
#ifdef _WIN32
	localtime_s(&_tm, &sec);
#else
	localtime_r(&sec, &_tm);
#endif
}

void CSTime::setYear(int year) {
	_tm.tm_year = year - 1900;
	setTime(mktime(&_tm));
}

void CSTime::setMonth(int month) {
	_tm.tm_mon = month - 1;
	setTime(mktime(&_tm));
}

void CSTime::setDay(int day) {
	_tm.tm_mday = day;
	setTime(mktime(&_tm));
}

void CSTime::setHour(int hour) {
	_tm.tm_hour = hour;
}

void CSTime::setMinute(int minute) {
	_tm.tm_min = minute;
}

void CSTime::setSecond(int second) {
	_tm.tm_sec = second;
}

void CSTime::setTime(int year, int month, int day, int hour, int minute, int second) {
	_tm.tm_year = year - 1900;
	_tm.tm_mon = month - 1;
	_tm.tm_mday = day;
	_tm.tm_hour = hour;
	_tm.tm_min = minute;
	_tm.tm_sec = second;
	setTime(mktime(&_tm));
}

int64 CSTime::diff(const CSTime& other) const {
	time_t a = mktime(&_tm);
	time_t b = mktime(&other._tm);
	return a - b;
}

void CSTime::revise(int64 second) {
	time_t a = mktime(&_tm) + second;

	setTime(a);
}

CSString* CSTime::stringWithFormat(const char* format) const {                  // yyyy-mm-dd HH:MM:SS -> %Y-%m-%d %H:%M:%S
    char str[256];
    strftime(str, 256, format, &_tm);
    return CSString::string(str);
}

int64 CSTime::currentTimeSecond() {
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec;
}

double CSTime::currentTime() {
    timeval tv;
    gettimeofday(&tv, 0);
    
    double ret = (double)tv.tv_sec;
    ret += (1.0E-6 * (double)tv.tv_usec);
    return ret;
}
