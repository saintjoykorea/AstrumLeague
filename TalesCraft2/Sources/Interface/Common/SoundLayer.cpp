//
//  SoundLayer.cpp
//  TalesCraft2
//
//  Created by ChangSun on 2016. 10. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//
#include "SoundLayer.h"
#include "Sound.h"

SoundLayer::SoundLayer(const SoundIndex& index) : _index(index){
    
}

void SoundLayer::onTouchesEnded() {
	Sound::play(SoundControlEffect, _index, SoundPlayEffect);
	
	CSLayer::onTouchesEnded();
}
