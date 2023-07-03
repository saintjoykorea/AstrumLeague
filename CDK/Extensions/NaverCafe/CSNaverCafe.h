//
//  CSNaverCafe.h
//  CDK
//
//  Created by WooyolJung on 2017. 11. 21.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSNaverCafe__
#define __CDK__CSNaverCafe__

class CSNaverCafe {
public:
    static void initialize();
    static void finalize();
    
    static void startHome();
    static void startArticle(int articleId);
};

#endif
