//
//  CSLogger.h
//  CDK
//
//  Created by 김찬 on 2015. 4. 7..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSLogger__
#define __CDK__CSLogger__

#include "CSObject.h"

#include <stdio.h>

class CSLogger : public CSObject {
private:
    FILE* _fp;
private:
    CSLogger(FILE* fp);
    ~CSLogger();
public:
    static CSLogger* create(const char* path, bool append = false);
    static inline CSLogger* logger(const char* path, bool append) {
        return autorelease(create(path, append));
    }
    
    void print(const char* format, ...);
    void flush();
};


#endif /* defined(__CDK__CSLogger__) */
