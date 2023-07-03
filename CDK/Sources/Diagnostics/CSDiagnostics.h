//
//  CSDiagnostics.h
//  CDK
//
//  Created by Kim Chan on 2017. 2. 13..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSDiagnostics__
#define __CDK__CSDiagnostics__

#include "CSDictionary.h"

struct CSDiagnosticsRecord {
	double latency;
	uint geometryCount[2];
	uint vertexCount;
	uint sample;

	inline float averageLatency() const {
		return sample ? latency / sample : 0.0f;
	}
	inline uint averageGeometryCount(bool batch) const {
		return sample ? geometryCount[batch] / sample : 0;
	}
	inline uint averageVertexCount() const {
		return sample ? vertexCount / sample : 0;
	}
	const char* toString() const;
};

class CSDiagnostics {
#ifdef CS_DIAGNOSTICS
private:
    CSDictionary<CSString, CSDiagnosticsRecord>* _records;

	struct Stack {
		const CSString* name;
		double elapsed;
		bool first;

		Stack(const CSString* name, bool first);
		~Stack();

		inline Stack(const Stack& other) {
			CSAssert(false, "invalid operation");
		}
		inline Stack& operator=(const Stack& other) {
			CSAssert(false, "invalid operation");
			return *this;
		}
	};
	CSArray<Stack>* _stack;
#ifdef CDK_IMPL
public:
#else
private:
#endif
    CSDiagnostics();
    ~CSDiagnostics();
public:
	void remove(const CSString* name);
	inline void remove(const char* name) {
		remove(CSString::string(name));
	}
	void begin(const CSString* name);
	inline void begin(const char* name) {
		begin(CSString::string(name));
	}
	void end();
#ifdef CDK_IMPL
	void addGeometry(bool batch);
	void addDraw(uint vertexCount);
#endif
	const CSDiagnosticsRecord* record(const CSString* name) const;
	inline const CSDiagnosticsRecord* record(const char* name) const {
		return record(CSString::string(name));
	}
	inline const CSDictionary<CSString, CSDiagnosticsRecord>* allRecords() {
		return _records;
	}
#else
public:
    inline CSDiagnostics() {}
    inline ~CSDiagnostics() {}

	inline void remove(const CSString* name) {}
	inline void remove(const char* name) {}
	inline void begin(const CSString* name) {}
	inline void begin(const char* name) {}
	inline void end() {}

#ifdef CDK_IMPL
	inline void addGeometry(bool batch) {}
	inline void addDraw(uint vertexCount) {}
#endif
	inline const CSDiagnosticsRecord* record(const CSString* name) const {
		return NULL;
	}
	inline const CSDiagnosticsRecord* record(const char* name) const {
		return NULL;
	}
	inline const CSDictionary<CSString, CSDiagnosticsRecord>* allRecords() {
		return NULL;
	}
#endif
};

#endif /* __CDK__CSDiagnostics__ */
