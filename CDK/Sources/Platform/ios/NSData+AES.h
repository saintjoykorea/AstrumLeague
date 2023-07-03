//
//  NSData+AES.h
//  CDK
//
//  Created by 김찬 on 13. 7. 29..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#if defined(CDK_IOS) && defined(CDK_IMPL)

#import <Foundation/Foundation.h>

@interface NSData (AES)

- (NSData*)encryptWithKey:(const char*)key;
- (NSData*)decryptWithKey:(const char*)key;

@end

#endif
