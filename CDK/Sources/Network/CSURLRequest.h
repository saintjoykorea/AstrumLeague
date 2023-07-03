//
//  CSURLRequest.h
//  CDK
//
//  Created by chan on 13. 6. 13..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSURLRequest__
#define __CDK__CSURLRequest__

#include "CSString.h"

#include "CSDictionary.h"

#define CSURLRequestTimeoutDefault	30

class CSURLRequest : public CSObject {
private:
    void* _handle;

    inline CSURLRequest(void* handle) : _handle(handle) {
        
    }
    ~CSURLRequest();
public:
    static CSURLRequest* create(const char* url, const char* method = "GET", bool usingCache = true, float timeoutInterval = CSURLRequestTimeoutDefault);

    static inline CSURLRequest* request(const char* url, const char* method = "GET", bool usingCache = true, float timeoutInterval = CSURLRequestTimeoutDefault) {
        return autorelease(create(url, method, usingCache, timeoutInterval));
    }
    
    inline void* handle() const {
        return _handle;
    }
	void setURL(const char* url);
	const char* URL() const;

	void setMethod(const char* method);
	const char* method() const;

	void setUsingCache(bool usingCache);
	bool usingCache() const;

	void setTimeoutInterval(float timeoutInterval);
	float timeoutInterval() const;

	void setHeaderField(const char* name, const char* value);
	const char* headerField(const char* name) const;
	const CSDictionary<CSString, CSString>* headerFields() const;

	void setBody(const CSData* data);
	void setBody(const void* data, uint length);
	const CSData* body() const;
};

#endif /* defined(__CDK__CSURLRequest__) */
