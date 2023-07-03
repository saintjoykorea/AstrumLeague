//
//  Speech.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 14..
//  Copyright © 2016년 brgames. All rights reserved.
//
#define GameImpl

#include "Speech.h"

Speech::Speech(const ImageIndex& portrait, const CSString* name, const CSString* msg) : portrait(portrait), name(retain(name)), msg(retain(msg)) {

}

Speech::~Speech() {
    release(name);
    release(msg);
}
