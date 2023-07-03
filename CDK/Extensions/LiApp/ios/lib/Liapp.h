//
//  Liapp.h
//  Liapp
//
//  Created by LIAPP TEAM on 2016. 4. 26..
//  Copyright © 2016년 LIAPP TEAM. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Liapp : NSObject
{
    NSString *strNotify;
}


@property (readwrite, assign) NSString *strNotify;

+ (int)LA1;
+ (int)LA2;
+ (NSString*)GA;
+ (NSString*)GA: (NSString*)strData;
+ (NSString*)GetMessage;

typedef enum
{
    LIAPP_EXCEPTION = -1,
    LIAPP_SUCCESS = 0,
    LIAPP_DETECTED = 1,
    LIAPP_DETECTED_VM = 3,
    LIAPP_DETECTED_ROOTING = 7,
    LIAPP_DETECTED_HACKING_TOOL = 8,
}LIAPP_RET_CODE;

@end


