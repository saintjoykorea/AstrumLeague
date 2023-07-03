//
//  CSSignal.h
//  CDK
//
//  Created by 김찬 on 2015. 3. 17..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSSignal__
#define __CDK__CSSignal__

class CSSignal {
public:
#ifdef CDK_IMPL
    static void initialize();
    static void finalize();
#endif
    static const char* const FileName;
    static const char* filePath();
    static const char* readLog();
    static void deleteLog();
};

#endif /* defined(__CDK__CSSignal__) */
