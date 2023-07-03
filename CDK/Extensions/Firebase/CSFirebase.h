//
//  CSFirebase.h
//  CDK
//
//  Created by WooyolJung on 2017. 11. 10.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSFirebase__
#define __CDK__CSFirebase__

class CSFirebaseDelegate {
public:
    virtual void onFirebaseTokenRefresh(const char* token) = 0;
};

class CSFirebase {
public:
    static void initialize();
    static void finalize();
	
	static const char* token();
    static void setDelegate(CSFirebaseDelegate* delegate);
    static void addTopic(const char* topic);
    static void removeTopic(const char* topic);
    static void log(const char* name, int paramCount, ...);
    static void setUserId(const char* userId);
};

#endif
