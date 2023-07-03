//
//  Sound.h
//  TalesCraft2
//
//  Created by 김 찬 on 12. 10. 9..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef Sound_h
#define Sound_h

#include "AssetIndex.h"

enum SoundPlay {
    SoundPlayEffect,
    SoundPlayEffectEssential,
	SoundPlayVoice,
	SoundPlayVoiceSingle,
    SoundPlayBgm
};
enum SoundControl {
    SoundControlEffect,
    SoundControlTitleBgm,
    SoundControlLobbyBgm,
    SoundControlGameBgm
};

class Sound {
public:
    static void play(SoundControl control, const SoundIndex& index, SoundPlay play, float volume = 1.0f);
    static void stop(SoundControl control);
    static void pause(SoundControl control);
    static void resume(SoundControl control);
	static int voiceCounter();
};

#endif /* Sound_h */
