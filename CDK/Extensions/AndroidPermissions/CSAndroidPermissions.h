//
//  CSAndroidPermissions.h
//  CDK
//
//  Created by WooyolJung on 2018. 04. 19.
//  Copyright © 2018 brgames. All rights reserved.
//

#ifndef __CDK__CSAndroidPermissions__
#define __CDK__CSAndroidPermissions__

enum CSAndroidPermissionResult {
    CSAndroidPermissionResultGranted,
    CSAndroidPermissionResultRetry,
    CSAndroidPermissionResultDenied
};

class CSAndroidPermissionsDelegate {
public:
	inline virtual void onAndroidPermissionsCheckDone(CSAndroidPermissionResult result) {}
};

class CSAndroidPermissions {
public:
    static void initialize();
    static void finalize();
    
	static void setDelegate(CSAndroidPermissionsDelegate* delegate);
    static void addEssentialPermission(const char* essentialPermission);
    static void checkPermissions();
    static void requestPermissions();
    static void showMoveToOptions(const char* title, const char* message, const char* positive, const char* negative);
};

#endif
