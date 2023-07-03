//
//  CSIGAWorks.h
//  CDK
//
//  Created by WooyolJung on 2017. 12. 04.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSIGAWorks__
#define __CDK__CSIGAWorks__

class CSIGAWorks {
public:
    static void initialize();
    static void finalize();

    static void firstTimeExperience(const char* userActivity);
    static void retention(const char* userActivity, const char* subActivity = NULL);
    static void purchase(const char* productId, const char* productName, double price, const char* currency);
};

#endif
