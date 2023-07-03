//
//  CSIGAWorks.mm
//  CDK
//
//  Created by wooyoljung on 2017. 7. 5.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSIGAWorks.h"

#import "CSAppDelegate.h"

#import <IgaworksCore/IgaworksCore.h>
#import <AdBrix/AdBrix.h>
#import <AdSupport/AdSupport.h>

void CSIGAWorks::initialize() {
	NSString* appKey = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"IGAWorksAppKey"];
	NSString* hashKey = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"IGAWorksHashKey"];

	if (!appKey || !hashKey) {
		NSLog(@"CSIGAWorks not ready");
		abort();
	}

	if (NSClassFromString(@"ASIdentifierManager")){
		NSUUID *ifa =[[ASIdentifierManager sharedManager] advertisingIdentifier];
		BOOL isAppleAdvertisingTrackingEnalbed = [[ASIdentifierManager sharedManager]isAdvertisingTrackingEnabled];
		[IgaworksCore setAppleAdvertisingIdentifier:[ifa UUIDString] isAppleAdvertisingTrackingEnabled:isAppleAdvertisingTrackingEnalbed];
	}
	
	[IgaworksCore igaworksCoreWithAppKey:appKey andHashKey:hashKey];
	[IgaworksCore setLogLevel:IgaworksCoreLogInfo];
}

void CSIGAWorks::finalize() {

}

void CSIGAWorks::firstTimeExperience(const char* userActivity) {
    [AdBrix firstTimeExperience:[NSString stringWithUTF8String:userActivity]];
}

void CSIGAWorks::retention(const char* userActivity, const char* subActivity) {
    if (subActivity == NULL) {
        [AdBrix retention:[NSString stringWithUTF8String:userActivity]];
    }
    else {
        [AdBrix retention:[NSString stringWithUTF8String:userActivity] param:[NSString stringWithUTF8String:subActivity]];
    }
}

void CSIGAWorks::purchase(const char* productId, const char* productName, double price, const char* currency) {
    AdBrixCommerceProductModel* product = [AdBrix createCommerceProductModel:[NSString stringWithUTF8String:productId]
                                                                 productName:[NSString stringWithUTF8String:productName]
                                                                       price:price
                                                                    discount:0
                                                                    quantity:1
                                                              currencyString:[NSString stringWithUTF8String:currency]
                                                                    category:nil
                                                               extraAttrsMap:nil];
    
    [AdBrix purchase:nil product:product paymentMethod:[AdBrix paymentMethod:AdBrixPaymentMobilePayment]];
    
}

#endif
