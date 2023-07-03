//
//  CSDiagnostics.cpp
//  CDK
//
//  Created by Kim Chan on 2017. 2. 13..
//  Copyright © 2017년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSDiagnostics.h"

#include "CSTime.h"

#include "CSThread.h"

#ifdef CS_DIAGNOSTICS

const char* CSDiagnosticsRecord::toString() const {
	return CSString::cstringWithFormat("sample:%d latency:%.8f / %.8f geometry(new):%d / %d geometry(batch):%d / %d vertex:%d / %d",
		sample,
		sample ? latency / sample : 0,
		latency,
		sample ? geometryCount[0] / sample : 0,
		geometryCount[0],
		sample ? geometryCount[1] / sample : 0,
		geometryCount[1],
		sample ? vertexCount / sample : 0,
		vertexCount);
}

CSDiagnostics::Stack::Stack(const CSString* name, bool first) : name(CSObject::retain(name)), elapsed(CSTime::currentTime()), first(first) {

}

CSDiagnostics::Stack::~Stack() {
	name->release();
}

CSDiagnostics::CSDiagnostics() : _records(new CSDictionary<CSString, CSDiagnosticsRecord>()), _stack(new CSArray<Stack>()) {
    
}

CSDiagnostics::~CSDiagnostics() {
	_records->release();
	_stack->release();
}

void CSDiagnostics::remove(const CSString* name) {
	_records->removeObject(name);
}

void CSDiagnostics::begin(const CSString* name) {
	CSDiagnosticsRecord* record = _records->tryGetObjectForKey(name);
    
    if (!record) {
		record = &_records->setObject(name);
        memset(record, 0, sizeof(CSDiagnosticsRecord));
    }
	bool first = true;
	foreach(Stack&, other, _stack) {
		if (other.name->isEqualToString(name)) {
			first = false;
			break;
		}
	}
	new (&_stack->addObject()) Stack(name, first);
}

void CSDiagnostics::end() {
    if (_stack->count()) {
		Stack& stack = _stack->lastObject();

		CSDiagnosticsRecord& record = _records->objectForKey(stack.name);
        
        double delay = CSTime::currentTime() - stack.elapsed;
		record.latency += delay;
		record.sample++;
        
		_stack->removeLastObject();
    }
}

void CSDiagnostics::addGeometry(bool batch) {
	foreach(const Stack&, stack, _stack) {
		if (stack.first) {
			CSDiagnosticsRecord& entry = _records->objectForKey(stack.name);
			entry.geometryCount[batch]++;
		}
	}
}

void CSDiagnostics::addDraw(uint vertexCount) {
	foreach(const Stack&, stack, _stack) {
		if (stack.first) {
			CSDiagnosticsRecord& entry = _records->objectForKey(stack.name);
			entry.vertexCount += vertexCount;
		}
	}
}

const CSDiagnosticsRecord* CSDiagnostics::record(const CSString* name) const {
	return _records->tryGetObjectForKey(name);
}

#endif
