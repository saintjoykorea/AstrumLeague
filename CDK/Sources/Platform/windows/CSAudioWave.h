#ifdef CDK_WINDOWS

#ifdef CDK_IMPL

#ifndef __CDK__CSAudioWave__
#define __CDK__CSAudioWave__

void waveInitialize();
void waveFinalize();
void wavePlay(int control, const char* path, float mainVolume, float subVolume, bool loop, int priority, int category, bool single);
void waveStop(int control);
void wavePause(int control);
void waveResume(int control);
void waveSetVolume(int category, float volume);
void waveUpdate();
bool waveIsSinglePlaying();

#endif

#endif

#endif