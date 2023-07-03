//
//  CSAudio.h
//  CDK
//
//  Created by Kim Chan on 2017. 7. 4..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifndef __CDK__CSAudio__
#define __CDK__CSAudio__

class CSAudio {
public:
#ifdef CDK_IMPL
	static void initialize();
	static void finalize();
	static void autoPause();
	static void autoResume();
#ifdef CDK_IOS
	static void restoreCategory();
#endif
#endif
	static void play(int control, const char* path, float volume, bool loop, int priority, int category, bool fx, bool single);
    static void stop(int control);
    static void pause(int control);
    static void resume(int control);
    static void setVolume(int category, float volume);
	static int singleCounter();
	static void vibrate(float time);
};

#endif
