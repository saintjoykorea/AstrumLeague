//
//  Sound.m
//  NomoCraft
//
//  Created by 풍식 이 on 12. 10. 9..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#include "Sound.h"

void Sound::play(SoundControl control, const SoundIndex& index, SoundPlay play, float volume) {
	static CSDictionary<SoundIndex, CSString>* paths = new CSDictionary<SoundIndex, CSString>(256);

	const char* path;

	if (paths->containsKey(index)) {
		path = *paths->objectForKey(index);
	}
	else {
		path = CSFile::findPath(CSString::cstringWithFormat("sounds/sound[%03d][%03d][%03d].ogg", index.indices[0], index.indices[1], index.indices[2]));

		paths->setObject(index, path ? CSString::string(path) : NULL);
	}
    if (path) {
		switch (play) {
			case SoundPlayEffectEssential:
			case SoundPlayEffect:
				CSAudio::play(control, path, volume, false, play, 1, true, false);
				break;
			case SoundPlayVoice:
				CSAudio::play(control, path, volume, false, play, 2, true, false);
				break;
			case SoundPlayVoiceSingle:
				CSAudio::play(control, path, volume, false, play, 2, true, true);
				break;
			case SoundPlayBgm:
				CSAudio::play(control, path, volume, true, play, 0, false, false);
				break;
		}
    }
    else {
        CSErrorLog("sound path not found:%d, %d", index.indices[0], index.indices[1]);
    }
}
void Sound::stop(SoundControl control) {
    CSAudio::stop(control);
}
void Sound::pause(SoundControl control) {
    CSAudio::pause(control);
}
void Sound::resume(SoundControl control) {
    CSAudio::resume(control);
}
int Sound::voiceCounter() {
	return CSAudio::singleCounter();
}

