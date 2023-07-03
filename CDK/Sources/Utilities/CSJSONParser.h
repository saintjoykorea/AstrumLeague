//
//  CSJSONParser.h
//  CDK
//
//  Created by 김찬 on 13. 5. 15..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSJSONParser__
#define __CDK__CSJSONParser__

#include "CSJSONObject.h"
#include "CSJSONArray.h"

class CSJSONParser : public CSObject {
public:
    static CSJSONObject* parse(const char* json);
};

#endif /* defined(__CDK__CSJSONParser__) */
