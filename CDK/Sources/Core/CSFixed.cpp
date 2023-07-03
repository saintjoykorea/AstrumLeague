//
//  CSFixed.cpp
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSFixed.h"

const int64 fixed::OneRaw = 1L << 16;
const int64 fixed::FracRaw = OneRaw - 1;
const fixed fixed::One(fixed::Raw, fixed::OneRaw);
const fixed fixed::Half(fixed::Raw, fixed::OneRaw / 2);
const fixed fixed::Quarter(fixed::Raw, fixed::OneRaw / 4);
const fixed fixed::Zero(fixed::Raw, 0);
const fixed fixed::Max(fixed::Raw, 0x00007FFFFFFFFFFF);
const fixed fixed::Min(fixed::Raw, 0xFFFF800000000000);
const fixed fixed::Pi(fixed::Raw, 0x3243F);
const fixed fixed::TwoPi(fixed::Raw, fixed::Pi.raw * 2);
const fixed fixed::PiOverTwo(fixed::Raw, fixed::Pi.raw / 2);
const fixed fixed::PiOverFour(fixed::Raw, fixed::Pi.raw / 4);
const fixed fixed::Epsilon(fixed::Raw, 1);
