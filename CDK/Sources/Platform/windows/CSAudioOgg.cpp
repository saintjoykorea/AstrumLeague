#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSAudioOgg.h"

#include "CSArray.h"

#include <stdio.h>

#include <vorbis/vorbisfile.h>

#include <AL/al.h>

static size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource) {
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

static int ov_seek_func(void *datasource, ogg_int64_t offset, int whence) {
	return fseek((FILE*)datasource, (long)offset, whence);
}

static int ov_close_func(void *datasource) {
	return fclose((FILE*)datasource);
}

static long ov_tell_func(void *datasource) {
	return ftell((FILE*)datasource);
}

#define BUFFER_COUNT	4

typedef struct _Stream {
	OggVorbis_File* fp;
	void* decodeBuffer;
	unsigned int decodeBufferSize;
	int channels;
	int frequency;
	int bufferFormat;
	unsigned int buffers[BUFFER_COUNT];
	unsigned int source;
	int control;
	int category;
	float volume;
	bool loop;
	bool single;
}Stream;

static void swap(short &s1, short &s2) {
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

static unsigned int oggDecode(Stream* stream) {
	int selection;

	unsigned int decodeLen = 0;

	for (;;) {
		int len = ov_read(stream->fp, (char*)stream->decodeBuffer + decodeLen, stream->decodeBufferSize - decodeLen, 0, 2, 1, &selection);
		if (len > 0) {
			decodeLen += len;

			if (decodeLen >= stream->decodeBufferSize) break;
		}
		else {
			if (decodeLen == 0 && stream->loop) {
				ov_raw_seek(stream->fp, 0);
			}
			else {
				break;
			}
		}
	}

	if (stream->channels == 6) {
		short* samples = (short*)stream->decodeBuffer;
		for (int ulSamples = 0; ulSamples < (stream->decodeBufferSize >> 1); ulSamples += 6) {
			swap(samples[ulSamples + 1], samples[ulSamples + 2]);
			swap(samples[ulSamples + 3], samples[ulSamples + 5]);
			swap(samples[ulSamples + 4], samples[ulSamples + 5]);
		}
	}
	return decodeLen;
}

static Stream* oggOpen(const char* path, bool loop) {
	FILE* fp = fopen(path, "rb");

	if (!fp) return NULL;

	ov_callbacks callbacks;
	OggVorbis_File*	ovfp = new OggVorbis_File;

	callbacks.read_func = ov_read_func;
	callbacks.seek_func = ov_seek_func;
	callbacks.close_func = ov_close_func;
	callbacks.tell_func = ov_tell_func;

	if (ov_open_callbacks(fp, ovfp, NULL, 0, callbacks)) {
		fclose(fp);
		delete ovfp;
		return NULL;
	}

	vorbis_info* vorbisInfo = ov_info(ovfp, -1);

	if (!vorbisInfo) {
		ov_clear(ovfp);
		delete ovfp;
		return NULL;
	}

	int bufferFormat;
	int decodeBufferSize;
	int frequency = vorbisInfo->rate;

	if (vorbisInfo->channels == 1) {
		bufferFormat = AL_FORMAT_MONO16;
		decodeBufferSize = frequency >> 1;
		decodeBufferSize -= (decodeBufferSize % 2);
	}
	else if (vorbisInfo->channels == 2) {
		bufferFormat = AL_FORMAT_STEREO16;
		decodeBufferSize = frequency;
		decodeBufferSize -= (decodeBufferSize % 4);
	}
	else if (vorbisInfo->channels == 4) {
		bufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
		decodeBufferSize = frequency * 2;
		decodeBufferSize -= (decodeBufferSize % 8);
	}
	else if (vorbisInfo->channels == 6) {
		bufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
		decodeBufferSize = frequency * 3;
		decodeBufferSize -= (decodeBufferSize % 12);
	}
	else {
		ov_clear(ovfp);
		delete ovfp;
		return NULL;
	}

	void* decodeBuffer = malloc(decodeBufferSize);

	if (!decodeBuffer) {
		ov_clear(ovfp);
		delete ovfp;
		return NULL;
	}

	Stream* stream = new Stream;
	stream->fp = ovfp;
	stream->decodeBuffer = decodeBuffer;
	stream->decodeBufferSize = decodeBufferSize;
	stream->bufferFormat = bufferFormat;
	stream->frequency = frequency;
	stream->loop = loop;
	alGenBuffers(BUFFER_COUNT, stream->buffers);
	alGenSources(1, &stream->source);

	for (int i = 0; i < BUFFER_COUNT; i++) {
		unsigned int len = oggDecode(stream);
		if (len) {
			alBufferData(stream->buffers[i], stream->bufferFormat, stream->decodeBuffer, len, stream->frequency);
			alSourceQueueBuffers(stream->source, 1, &stream->buffers[i]);
		}
	}

	return stream;
}

static void oggClose(Stream* stream) {
	alSourceStop(stream->source);
	alDeleteSources(1, &stream->source);
	alDeleteBuffers(BUFFER_COUNT, stream->buffers);

	ov_clear(stream->fp);
	delete stream->fp;

	free(stream->decodeBuffer);

	delete stream;
}

static bool oggUpdate(Stream* stream) {
	int buffersProcessed = 0;
	alGetSourcei(stream->source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	while (buffersProcessed) {
		unsigned int buffer = 0;
		alSourceUnqueueBuffers(stream->source, 1, &buffer);
		unsigned int len = oggDecode(stream);
		if (len) {
			alBufferData(buffer, stream->bufferFormat, stream->decodeBuffer, len, stream->frequency);
			alSourceQueueBuffers(stream->source, 1, &buffer);
		}
		buffersProcessed--;
	}
	int state;
	alGetSourcei(stream->source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING && state != AL_PAUSED) {
		int queuedBuffer;
		alGetSourcei(stream->source, AL_BUFFERS_QUEUED, &queuedBuffer);
		if (queuedBuffer) {
			alSourcePlay(stream->source);
		}
		else {
			return false;
		}
	}
	return true;
}


//=============================================================================

static CSArray<Stream*>* __streams;

void oggInitialize() {
	__streams = new CSArray<Stream*>();
}

void oggFinalize() {
	foreach(Stream*, stream, __streams) {
		oggClose(stream);
	}
	__streams->release();
}

void oggPlay(int control, const char* path, float mainVolume, float subVolume, bool loop, int priority, int category, bool single) {
	Stream* stream = oggOpen(path, loop);

	if (!stream) {
		CSErrorLog("unable to play:%s", path);
		return;
	}

	alSourcef(stream->source, AL_GAIN, mainVolume * subVolume);
	alSourcePlay(stream->source);

	stream->control = control;
	stream->category = category;
	stream->volume = subVolume;
	stream->single = single;
	__streams->addObject(stream);
}

void oggStop(int control) {
	int i = 0;
	while (i < __streams->count()) {
		Stream* stream = __streams->objectAtIndex(i);

		if (stream->control == control) {
			oggClose(stream);
			__streams->removeObjectAtIndex(i);
		}
		else {
			i++;
		}
	}
}

void oggPause(int control) {
	foreach(Stream*, stream, __streams) {
		if (stream->control == control) {
			alSourcePause(stream->source);
		}
	}
}

void oggResume(int control) {
	foreach(Stream*, stream, __streams) {
		if (stream->control == control) {
			alSourcePlay(stream->source);
		}
	}
}

void oggSetVolume(int category, float volume) {
	foreach(Stream*, stream, __streams) {
		if (stream->category == category) {
			alSourcef(stream->source, AL_GAIN, volume * stream->volume);
		}
	}
}

void oggUpdate() {
	int i = 0;
	while (i < __streams->count()) {
		Stream* stream = __streams->objectAtIndex(i);

		if (oggUpdate(stream)) i++;
		else {
			oggClose(stream);
			__streams->removeObjectAtIndex(i);
		}
	}
}

bool oggIsSinglePlaying() {
	foreach(Stream*, stream, __streams) {
		if (stream->single) return true;
	}
	return false;
}

#endif