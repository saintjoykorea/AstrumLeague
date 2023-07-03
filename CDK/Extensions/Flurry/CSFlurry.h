//
//  CSFlurry.h
//  TalesCraft2
//
//  Created by ChangSun on 2018. 8. 10..
//  Copyright © 2018년 brgames. All rights reserved.
//

#ifndef CSFlurry_h
#define CSFlurry_h

class CSFlurry {
public:
    static void initialize();
    static void finalize();

    static void log(const char* keyword, int paramCount, ...);
};

#endif /* CSFlurry_h */
