//
//  Header.h
//  TalesCraft2
//
//  Created by ChangSun on 2018. 11. 2..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSScreenRecord_h
#define CSScreenRecord_h

#include "CDK.h"

class CSScreenRecord {
public:
    static void initialize();
    static void finalize();
    
    static bool isRecording();
    static bool canQualitySelect();
    static bool canMicSelect();
    static void recordStart(int displayWidth, int displayHeight, bool micEnabled);
    static void recordEnd();
};

#endif
   
