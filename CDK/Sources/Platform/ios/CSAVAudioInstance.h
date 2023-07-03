//
//  CSAVAudioInstance.h
//  CDK
//
//  Created by Kim Chan on 2016. 11. 22..
//  Copyright © 2016년 brgames. All rights reserved.
//

#if defined(CDK_IOS) && defined(CDK_IMPL)

#import "CSAudioInstance.h"

#import <AVFoundation/AVFoundation.h>

@interface CSAVAudioInstance : CSAudioInstance<AVAudioPlayerDelegate> {
    AVAudioPlayer* _component;
}

-(id)initWithPath:(NSString*)path;

@end

#endif
