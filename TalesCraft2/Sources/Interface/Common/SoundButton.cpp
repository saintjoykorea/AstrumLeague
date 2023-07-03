//
//  SoundButton.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2016. 10. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "SoundButton.h"

#include "Sound.h"

SoundButton::SoundButton(const SoundIndex& index) : _index(index){
    
}

void SoundButton::onTouchesEnded() {
	Sound::play(SoundControlEffect, _index, SoundPlayEffect);
	
	CSButton::onTouchesEnded();
}
