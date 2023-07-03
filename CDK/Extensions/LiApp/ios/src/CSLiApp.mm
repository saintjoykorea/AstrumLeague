//
//  CSLiapp.mm
//  CDK
//
//  Created by Kim Su Hyun 2020.08.03
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSLiapp.h"

#import "CSAppDelegate.h"

#import "Liapp.h"

#ifndef DEBUG
static void showAlert(NSString* message, BOOL exit) {
	UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Alert" message:message preferredStyle:UIAlertControllerStyleAlert];
	if (exit) {
		[alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
			::exit(0);
		}]];
	}
    [[[CSAppDelegate sharedDelegate] viewController] presentViewController:alert animated:YES completion:nil];
}
#endif

void CSLiApp::initialize() {
#ifdef DEBUG
	NSLog(@"LiApp is off in debug mode");
#else
	int result = [Liapp LA1];
	
	if (result != LIAPP_SUCCESS) {
		NSLog(@"CSLiApp detection code:%x(%s)", result, [Liapp GetMessage].UTF8String);
		
		switch(result) {
			case LIAPP_EXCEPTION:
				showAlert([Liapp GetMessage], NO);
				break;
			case LIAPP_DETECTED_ROOTING:
				//no action
				break;
			default:
				showAlert([Liapp GetMessage], YES);
				break;
		}
	}
#endif
}

void CSLiApp::finalize() {

}

#endif
