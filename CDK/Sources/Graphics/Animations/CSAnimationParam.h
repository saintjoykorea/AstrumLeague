//
//  CSAnimationParam.h
//  CDK
//
//  Created by 김찬 on 2014. 12. 1..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef __CDK__CSAnimationParam__
#define __CDK__CSAnimationParam__

#include "CSAnimationLoop.h"

enum CSAnimationState {
    CSAnimationStateNone,
    CSAnimationStateStopped,
    CSAnimationStateAlive,
    CSAnimationStateFinishing
};

enum CSAnimationDuration {
    CSAnimationDurationMin,
    CSAnimationDurationMax,
    CSAnimationDurationAvg
};

#endif
