#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSAudio.h"
#include "CSAudioWave.h"
#include "CSAudioOgg.h"

#include "CSDictionary.h"
#include "CSThread.h"

#include <AL/alc.h>

#define UpdateInterval	0.1f

typedef struct _Context {
	ALCcontext* context;
	CSDictionary<int, float>* volumes;
	CSThread* thread;
	CSTask* updateTask;
} Context;

static Context* __context = NULL;

static int __singleCounter = 0;

void CSAudio::initialize() {
	const ALCchar *defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	ALCdevice *device = alcOpenDevice(defaultDeviceName);

	if (device) {
		ALCcontext* context = alcCreateContext(device, NULL);

		if (context) {
			alcMakeContextCurrent(context);

			__context = new Context();
			__context->context = context;
			__context->volumes = new CSDictionary<int, float>();
			__context->thread = new CSThread();
			__context->thread->start(false);

			waveInitialize();
			oggInitialize();

			CSDelegate0<void>* inv = new CSDelegate0<void>([]() {
				waveUpdate();
				oggUpdate();
			});
			__context->updateTask = new CSTask(inv, UpdateInterval, CSTaskActivityRepeat);
			__context->thread->addTask(__context->updateTask);
			inv->release();
		}
		else
		{
			alcCloseDevice(device);

			CSErrorLog("unable to create context:%s", defaultDeviceName);
		}
	}
	else {
		CSErrorLog("unable to open device:%s", defaultDeviceName);
	}
}

void CSAudio::finalize() {
	if (__context) {
		__context->updateTask->stop();

		__context->thread->stop();
		__context->thread->release();
		__context->volumes->release();
		__context->updateTask->release();

		waveFinalize();
		oggFinalize();

		ALCdevice* device = alcGetContextsDevice(__context->context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(__context->context);
		alcCloseDevice(device);

		delete __context;
		__context = NULL;
	}
}

void CSAudio::autoPause() {
	//no need
}

void CSAudio::autoResume() {
	//no need
}

void CSAudio::play(int control, const char* path, float volume, bool loop, int priority, int category, bool fx, bool single) {
	if (!__context) return;

	char* cpath = strdup(path);

	CSDelegate0<void>* inv = new CSDelegate0<void>([control, cpath, volume, loop, priority, category, single]() {
		if (single) {
			if (waveIsSinglePlaying() || oggIsSinglePlaying()) return;

			__singleCounter++;
		}
		float* mv = __context->volumes->tryGetObjectForKey(category);
		float mainVolume = mv ? *mv : 1.0f;

		int pathlen = strlen(cpath);
		if (pathlen <= 3) {
			CSErrorLog("invalid path:%s", cpath);
			return;
		}
		else if (strcasecmp(cpath + pathlen - 3, "wav") == 0) wavePlay(control, cpath, mainVolume, volume, loop, priority, category, single);
		else if (strcasecmp(cpath + pathlen - 3, "ogg") == 0) oggPlay(control, cpath, mainVolume, volume, loop, priority, category, single);

		free(cpath);
	});
	CSTask* task = new CSTask(inv);
	__context->thread->addTask(task);
	task->release();
	inv->release();
}

void CSAudio::stop(int control) {
	if (!__context) return;

	CSDelegate0<void>* inv = new CSDelegate0<void>([control]() {
		waveStop(control);
		oggStop(control);
	});
	CSTask* task = new CSTask(inv);
	__context->thread->addTask(task);
	task->release();
	inv->release();
}

void CSAudio::pause(int control) {
	if (!__context) return;

	CSDelegate0<void>* inv = new CSDelegate0<void>([control]() {
		wavePause(control);
		oggPause(control);
	});
	CSTask* task = new CSTask(inv);
	__context->thread->addTask(task);
	task->release();
	inv->release();
}

void CSAudio::resume(int control) {
	if (!__context) return;

	CSDelegate0<void>* inv = new CSDelegate0<void>([control]() {
		waveResume(control);
		oggResume(control);
	});
	CSTask* task = new CSTask(inv);
	__context->thread->addTask(task);
	task->release();
	inv->release();
}

void CSAudio::setVolume(int category, float volume) {
	if (!__context) return;

	CSDelegate0<void>* inv = new CSDelegate0<void>([category, volume]() {
		if (volume == 1.0f) {
			__context->volumes->removeObject(category);
		}
		else {
			__context->volumes->setObject(category, volume);
		}

		waveSetVolume(category, volume);
		oggSetVolume(category, volume);
	});
	CSTask* task = new CSTask(inv);
	__context->thread->addTask(task);
	task->release();
	inv->release();
}

int CSAudio::singleCounter() {
	return __singleCounter;
}

void CSAudio::vibrate(float time) {
	//nothing to do
}

#endif