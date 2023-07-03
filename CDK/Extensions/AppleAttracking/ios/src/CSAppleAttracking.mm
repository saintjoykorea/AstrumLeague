//
//  CSAppleLogin.mm
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS
#define CDK_IMPL

#import "CSAppleAttracking.h"
#import "CSAppDelegate.h"
#import <AppTrackingTransparency/AppTrackingTransparency.h>
#import <AdSupport/AdSupport.h>

void CSAppleAttracking::initialize() {

}

void CSAppleAttracking::finalize() {

}

void CSAppleAttracking::show() {
    if (@available(iOS 14.0, *)) {
        [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
            switch (status) {
                case ATTrackingManagerAuthorizationStatusDenied:
                    NSLog(@"ATTrackingManagerAuthorizationStatusDenied");
                    break;
                case ATTrackingManagerAuthorizationStatusAuthorized:
                    NSLog(@"ATTrackingManagerAuthorizationStatusAuthorized");
                    //NSString *idfaString = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
                    //NSLog(@"idfa %@",[[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString]);
                    break;
                case ATTrackingManagerAuthorizationStatusNotDetermined:
                    NSLog(@"ATTrackingManagerAuthorizationStatusNotDetermined");
                    break;
                case ATTrackingManagerAuthorizationStatusRestricted:
                    NSLog(@"ATTrackingManagerAuthorizationStatusRestricted");
                    break;
            }
        }];
    }
}

#endif

