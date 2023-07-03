#ifdef CDK_WINDOWS

#ifndef __CDK__CSURLRequestImpl__
#define __CDK__CSURLRequestImpl__

#include "CSDictionary.h"

#include "CSData.h"

struct CSURLRequestHandle {
	char* url;
	char* method;
	bool usingCache;
	CSDictionary<CSString, CSString>* headerFields;
	float timeoutInterval;
	const CSData* body;

	CSURLRequestHandle(const char* url, const char* method, bool usingCache, float timeoutInterval);
	~CSURLRequestHandle();

private:
	CSURLRequestHandle(const CSURLRequestHandle&);
	CSURLRequestHandle& operator=(const CSURLRequestHandle&);
};


#endif
#endif


