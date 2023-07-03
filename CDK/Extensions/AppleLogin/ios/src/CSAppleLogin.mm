//
//  CSAppleLogin.mm
//  CDK
//
//  Created by Kim Su Hyun on 2020. 6. 8.
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppleLogin.h"

#import "CSAppDelegate.h"
#import "CSFile.h"
#import "CSBuffer.h"

#import <AuthenticationServices/AuthenticationServices.h>

@interface CSAppleLoginImpl : NSObject<ASAuthorizationControllerDelegate, ASAuthorizationControllerPresentationContextProviding> {
    CSAppleLoginDelegate* _delegate;
	NSString* _userId;
	NSString* _email;
}

@property (readwrite, assign) CSAppleLoginDelegate* delegate;
@property (readonly) NSString* userId;
@property (readonly) NSString* email;
@property (readonly, getter = isConnected) BOOL connected;

-(void)dealloc;

-(void)login;

@end

@implementation CSAppleLoginImpl

@synthesize delegate = _delegate;
@synthesize userId = _userId;
@synthesize email = _email;

-(void)dealloc {
	[_userId release];
	[_email release];
    
    [super dealloc];
}

-(BOOL)isConnected {
	return _userId != nil;
}

-(void)login {
    if (@available(iOS 13.0, *)) {
		ASAuthorizationAppleIDProvider *appleIDProvider = [ASAuthorizationAppleIDProvider new];
		ASAuthorizationAppleIDRequest *request = appleIDProvider.createRequest;
		request.requestedScopes = @[ASAuthorizationScopeEmail];
		ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
		controller.delegate = self;
		controller.presentationContextProvider = self;
		[controller performRequests];
    }
	else {
		NSLog(@"CSAppleLogin login error: lower version");
		
		if (_delegate) {
			_delegate->onAppleLogin(CSAppleLoginResultError);
		}
	}
}

-(ASPresentationAnchor)presentationAnchorForAuthorizationController:(ASAuthorizationController*)controller API_AVAILABLE(ios(13.0)){
    return [[[CSAppDelegate sharedDelegate] view] window];
}

-(void)authorizationController:(ASAuthorizationController*)controller didCompleteWithAuthorization:(ASAuthorization*)authorization  API_AVAILABLE(ios(13.0)){
    if ([authorization.credential isKindOfClass:[ASAuthorizationAppleIDCredential class]]) {
        ASAuthorizationAppleIDCredential *appleIDCredential = authorization.credential;
        
		[_userId release];
		_userId = [appleIDCredential.user retain];
		[_email release];
		_email = [appleIDCredential.email retain];
		
		if (_delegate) {
			_delegate->onAppleLogin(CSAppleLoginResultSuccess);
		}
    }
}

-(void)authorizationController:(ASAuthorizationController*)controller didCompleteWithError:(NSError*)error  API_AVAILABLE(ios(13.0)){
	NSLog(@"CSAppleLogin login error: %@", error);
	
	if (_delegate) {
		_delegate->onAppleLogin(error.code == ASAuthorizationErrorCanceled ? CSAppleLoginResultCancelled : CSAppleLoginResultError);
	}
}

@end

static CSAppleLoginImpl* __sharedImpl = nil;

void CSAppleLogin::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSAppleLoginImpl alloc] init];
    }
}

void CSAppleLogin::finalize() {
    if (__sharedImpl) {
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSAppleLogin::setDelegate(CSAppleLoginDelegate* delegate) {
	[__sharedImpl setDelegate:delegate];
}

void CSAppleLogin::login() {
	[__sharedImpl login];
}

bool CSAppleLogin::isConnected() {
	return __sharedImpl.isConnected;
}

const char* CSAppleLogin::userId() {
	return __sharedImpl.userId.UTF8String;
}

const char* CSAppleLogin::email() {
	return __sharedImpl.email.UTF8String;
}

bool CSAppleLogin::isEnabled() {
    if (@available(iOS 13, *)) {
        return true;
    }
    return false;
}

#endif
