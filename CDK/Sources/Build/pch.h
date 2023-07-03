#ifndef __CDK__pch__
#define __CDK__pch__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <memory.h>
#include <inttypes.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/ubrk.h>
#include <zlib.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <typeinfo>
#include <atomic>
#include <cfloat>
#endif

#ifdef CDK_WINDOWS

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftsnames.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#elif defined(CDK_ANDROID)

#include <direct.h>
#include <unzip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <err.h>
#include <linux/tcp.h>
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>

#elif defined(CDK_IOS)

#include <execinfo.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <err.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/utsname.h>
#include <mach/mach.h>
#include <CommonCrypto/CommonDigest.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <Availability.h>
#include <Vorbis/vorbisfile.h>
#include <AudioToolbox/AudioToolbox.h>
#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>
#import <WebKit/WebKit.h>
#endif

#endif

#endif
