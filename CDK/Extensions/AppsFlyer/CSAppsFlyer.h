//
//  CSAppsFlyer.h
//  TalesCraft2
//
//  Created by ChangSun on 2018. 8. 10..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSAppsFlyer_h
#define CSAppsFlyer_h

class CSAppsFlyer {
public:
    static void initialize();
    static void finalize();
    
    static void log(const char* name, int paramCount, ...);
};

#endif /* CSAppsFlyer_h */
