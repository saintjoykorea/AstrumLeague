//
//  CSAppSafer.h
//  TalesCraft2
//
//  Created by Kim Chan on 2021. 9. 28..
//  Copyright © 2021년 brgames. All rights reserved.
//

#ifndef CSAppSafer_h
#define CSAppSafer_h

class CSAppSafer {
public:
    static void initialize();
	static void finalize();
	static void setUserId(const char* userId);
};

#endif /* CSAppSafer_h */
