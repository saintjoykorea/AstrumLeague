#ifdef CDK_WINDOWS

#ifdef CDK_IMPL

#ifndef __CDK__CSAudioOgg__
#define __CDK__CSAudioOgg__

void oggInitialize();
void oggFinalize();
void oggPlay(int control, const char* path, float mainVolume, float subVolume, bool loop, int priority, int category, bool single);
void oggStop(int control);
void oggPause(int control);
void oggResume(int control);
void oggSetVolume(int category, float volume);
void oggUpdate();
bool oggIsSinglePlaying();

#endif

#endif

#endif
