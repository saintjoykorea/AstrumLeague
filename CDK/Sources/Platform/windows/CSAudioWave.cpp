#ifdef CDK_WINDOWS

#define CDK_IMPL

//TODO:purge가 필요하다면 추후 추가, 현재는 무제한

#include "CSAudioWave.h"

#include "CSArray.h"

#include <windows.h>
#include <ks.h>
#include <ksmedia.h>
#include <stdio.h>

#include <AL/al.h>

enum WAVEFILETYPE {
	WF_EX = 1,
	WF_EXT = 2
};

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX {
	WORD    wFormatTag;
	WORD    nChannels;
	DWORD   nSamplesPerSec;
	DWORD   nAvgBytesPerSec;
	WORD    nBlockAlign;
	WORD    wBitsPerSample;
	WORD    cbSize;
} WAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
	WAVEFORMATEX    Format;
	union {
		WORD wValidBitsPerSample;       /* bits of precision  */
		WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
		WORD wReserved;                 /* If neither applies, set to zero. */
	} Samples;
	DWORD           dwChannelMask;      /* which channels are */
										/* present in stream  */
	GUID            SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif // !_WAVEFORMATEXTENSIBLE_

typedef struct {
	WAVEFILETYPE	wfType;
	WAVEFORMATEXTENSIBLE wfEXT;		// For non-WAVEFORMATEXTENSIBLE wavefiles, the header is stored in the Format member of wfEXT
	char			*pData;
	unsigned long	ulDataSize;
} WAVEFILEINFO, *LPWAVEFILEINFO;

#pragma pack(push, 4)

typedef struct
{
	char			szRIFF[4];
	unsigned long	ulRIFFSize;
	char			szWAVE[4];
} WAVEFILEHEADER;

typedef struct
{
	char			szChunkName[4];
	unsigned long	ulChunkSize;
} RIFFCHUNK;

typedef struct
{
	unsigned short	usFormatTag;
	unsigned short	usChannels;
	unsigned long	ulSamplesPerSec;
	unsigned long	ulAvgBytesPerSec;
	unsigned short	usBlockAlign;
	unsigned short	usBitsPerSample;
	unsigned short	usSize;
	unsigned short  usReserved;
	unsigned long	ulChannelMask;
	GUID            guidSubFormat;
} WAVEFMT;

#pragma pack(pop)

static LPWAVEFILEINFO loadInfo(const char* szFilename) {
	FILE* fp = fopen(szFilename, "rb");

	if (fp) {
		WAVEFILEHEADER header;

		fread(&header, 1, sizeof(WAVEFILEHEADER), fp);

		if (!_strnicmp(header.szRIFF, "RIFF", 4) && !_strnicmp(header.szWAVE, "WAVE", 4)) {
			RIFFCHUNK riffChunk;

			WAVEFMT fmt;

			unsigned int dataOffset = 0;

			LPWAVEFILEINFO wave = new WAVEFILEINFO;
			
			memset(wave, 0, sizeof(WAVEFILEINFO));

			while (fread(&riffChunk, 1, sizeof(RIFFCHUNK), fp) == sizeof(RIFFCHUNK)) {
				if (!_strnicmp(riffChunk.szChunkName, "fmt ", 4)) {
					if (riffChunk.ulChunkSize <= sizeof(WAVEFMT)) {
						fread(&fmt, 1, riffChunk.ulChunkSize, fp);

						if (fmt.usFormatTag == WAVE_FORMAT_PCM) { 
							wave->wfType = WF_EX;
							memcpy(&wave->wfEXT.Format, &fmt, sizeof(PCMWAVEFORMAT));
						}
						else if (fmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)	{
							wave->wfType = WF_EXT;
							memcpy(&wave->wfEXT, &fmt, sizeof(WAVEFORMATEXTENSIBLE));
						}
					}
					else {
						fseek(fp, riffChunk.ulChunkSize, SEEK_CUR);
					}
				}
				else if (!_strnicmp(riffChunk.szChunkName, "data", 4)) {
					wave->ulDataSize = riffChunk.ulChunkSize;
					dataOffset = ftell(fp);
					fseek(fp, riffChunk.ulChunkSize, SEEK_CUR);
				}
				else
				{
					fseek(fp, riffChunk.ulChunkSize, SEEK_CUR);
				}
				
				if (riffChunk.ulChunkSize & 1) fseek(fp, 1, SEEK_CUR);
			}

			if (wave->ulDataSize && dataOffset && ((wave->wfType == WF_EX) || (wave->wfType == WF_EXT))) {
				wave->pData = new char[wave->ulDataSize];
				if (wave->pData) {
					fseek(fp, dataOffset, SEEK_SET);

					if (fread(wave->pData, 1, wave->ulDataSize, fp) == wave->ulDataSize) {
						fclose(fp);
						return wave;
					}
					else {
						free(wave->pData);
					}
				}
			}

			delete wave;
		}

		fclose(fp);
	}
	
	return NULL;
}

static void releaseInfo(LPWAVEFILEINFO wave) {
	free(wave->pData);
	delete wave;
}

static int getALBufferFormat(LPWAVEFILEINFO wave) {
	int format = 0;
	
	if (wave->wfType == WF_EX) {
		if (wave->wfEXT.Format.nChannels == 1) {
			switch (wave->wfEXT.Format.wBitsPerSample) {
				case 4:
					return alGetEnumValue("AL_FORMAT_MONO_IMA4");
				case 8:
					return AL_FORMAT_MONO8;
				case 16:
					return AL_FORMAT_MONO16;
			}
		}
		else if (wave->wfEXT.Format.nChannels == 2)	{
			switch (wave->wfEXT.Format.wBitsPerSample)	{
				case 4:
					return alGetEnumValue("AL_FORMAT_STEREO_IMA4");
				case 8:
					return AL_FORMAT_STEREO8;
				case 16:
					return AL_FORMAT_STEREO16;
			}
		}
		else if ((wave->wfEXT.Format.nChannels == 4) && (wave->wfEXT.Format.wBitsPerSample == 16)) {
			return alGetEnumValue("AL_FORMAT_QUAD16");
		}
	}
	else if (wave->wfType == WF_EXT) {
		if ((wave->wfEXT.Format.nChannels == 1) &&
			((wave->wfEXT.dwChannelMask == SPEAKER_FRONT_CENTER) ||
			(wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)) ||
				(wave->wfEXT.dwChannelMask == 0)))
		{
			switch (wave->wfEXT.Format.wBitsPerSample) {
				case 4:
					return alGetEnumValue("AL_FORMAT_MONO_IMA4");
					break;
				case 8:
					return AL_FORMAT_MONO8;
				case 16:
					return AL_FORMAT_MONO16;
			}
		}
		else if ((wave->wfEXT.Format.nChannels == 2) && (wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT))) {
			switch (wave->wfEXT.Format.wBitsPerSample) {
				case 4:
					return alGetEnumValue("AL_FORMAT_STEREO_IMA4");
				case 8:
					return AL_FORMAT_STEREO8;
				case 16:
					return AL_FORMAT_STEREO16;
			}
		}
		else if ((wave->wfEXT.Format.nChannels == 2) && (wave->wfEXT.Format.wBitsPerSample == 16) && (wave->wfEXT.dwChannelMask == (SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
			return alGetEnumValue("AL_FORMAT_REAR16");
		else if ((wave->wfEXT.Format.nChannels == 4) && (wave->wfEXT.Format.wBitsPerSample == 16) && (wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
			return alGetEnumValue("AL_FORMAT_QUAD16");
		else if ((wave->wfEXT.Format.nChannels == 6) && (wave->wfEXT.Format.wBitsPerSample == 16) && (wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
			return alGetEnumValue("AL_FORMAT_51CHN16");
		else if ((wave->wfEXT.Format.nChannels == 7) && (wave->wfEXT.Format.wBitsPerSample == 16) && (wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER)))
			return alGetEnumValue("AL_FORMAT_61CHN16");
		else if ((wave->wfEXT.Format.nChannels == 8) && (wave->wfEXT.Format.wBitsPerSample == 16) && (wave->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)))
			return alGetEnumValue("AL_FORMAT_71CHN16");
	}

	return 0;
}

static uint loadBuffer(const char* path) {
	LPWAVEFILEINFO wave = loadInfo(path);

	if (!wave) {
		CSErrorLog("unable to load wave:%s", path);
		return 0;
	}

	uint obj;
	alGetError();
	alGenBuffers(1, &obj);
	alBufferData(obj, getALBufferFormat(wave), wave->pData, wave->ulDataSize, wave->wfEXT.Format.nSamplesPerSec);

	releaseInfo(wave);

	return obj;
}

//==================================================================================
typedef struct _Buffer {
	char* path;
	uint object;
	uint retainCount;
} Buffer;

typedef struct _Stream {
	uint object;
	uint buffer;
	int control;
	int category;
	float volume;
	bool loop;
	bool single;
} Stream;

static CSArray<Buffer>* __buffers;
static CSArray<Stream>* __streams;

static void releaseStream(Stream& stream) {
	foreach(Buffer&, buffer, __buffers) {
		if (buffer.object == stream.buffer) {
			buffer.retainCount--;
			break;
		}
	}
	alSourceStop(stream.object);
	alDeleteSources(1, &stream.object);
}

void waveInitialize() {
	__buffers = new CSArray<Buffer>();
	__streams = new CSArray<Stream>();
}

void waveFinalize() {
	foreach(Stream&, stream, __streams) {
		alSourceStop(stream.object);
		alDeleteSources(1, &stream.object);
	}
	foreach(Buffer&, buffer, __buffers) {
		alDeleteBuffers(1, &buffer.object);
		free(buffer.path);
	}
	__buffers->release();
	__streams->release();
}

void wavePlay(int control, const char* path, float mainVolume, float subVolume, bool loop, int priority, int category, bool single) {
	int bufobj = 0;
	foreach(Buffer&, other, __buffers) {
		if (strcasecmp(other.path, path) == 0) {
			bufobj = other.object;
			other.retainCount++;
			break;
		}
	}
	if (!bufobj) {
		bufobj = loadBuffer(path);

		Buffer& buffer = __buffers->addObject();
		buffer.path = strdup(path);
		buffer.object = bufobj;
		buffer.retainCount = 1;
	}

	uint srcobj;
	alGenSources(1, &srcobj);
	alSourcei(srcobj, AL_BUFFER, bufobj);
	alSourcef(srcobj, AL_GAIN, mainVolume * subVolume);
	alSourcePlay(srcobj);

	Stream& stream = __streams->addObject();
	stream.control = control;
	stream.category = category;
	stream.buffer = bufobj;
	stream.object = srcobj;
	stream.volume = subVolume;
	stream.loop = loop;
	stream.single = single;
}

void waveStop(int control) {
	int i = 0;
	while (i < __streams->count()) {
		Stream& stream = __streams->objectAtIndex(i);

		if (stream.control == control) {
			releaseStream(stream);
			__streams->removeObjectAtIndex(i);
		}
		else {
			i++;
		}
	}
}

void wavePause(int control) {
	foreach(Stream&, stream, __streams) {
		if (stream.control == control) {
			alSourcePause(stream.object);
		}
	}
}

void waveResume(int control) {
	foreach(Stream&, stream, __streams) {
		if (stream.control == control) {
			alSourcePlay(stream.object);
		}
	}
}

void waveSetVolume(int category, float volume) {
	foreach(Stream&, stream, __streams) {
		if (stream.category == category) {
			alSourcef(stream.object, AL_GAIN, volume * stream.volume);
		}
	}
}

void waveUpdate() {
	int i = 0;
	while (i < __streams->count()) {
		Stream& stream = __streams->objectAtIndex(i);

		int state;
		alGetSourcei(stream.object, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && state != AL_PAUSED) {
			if (stream.loop) alSourcePlay(stream.object);
			else {
				releaseStream(stream);
				__streams->removeObjectAtIndex(i);
			}
		}
		else {
			i++;
		}
	}
}

bool waveIsSinglePlaying() {
	foreach(const Stream&, stream, __streams) {
		if (stream.single) return true;
	}
	return false;
}

#endif