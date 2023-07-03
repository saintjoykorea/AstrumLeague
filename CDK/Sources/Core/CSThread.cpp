//
//  CSThread.cpp
//  CDK
//
//  Created by chan on 13. 2. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSThread.h"
#include "CSTime.h"
#include "CSLog.h"
#include "CSMath.h"
#include "CSAutoreleasePool.h"
#include "CSLock.h"

#include <algorithm>

#ifdef CDK_WINDOWS
#include <windows.h>
#include <sched.h>
#else
#include <unistd.h>
#ifdef CDK_ANDROID
#include <sched.h>
#include "CSJNI.h"
#endif
#endif

#define KeepAliveDuration       40

static CSThread* __mainThread = NULL;
static std::vector<CSThread*> __threads;
static CSLock __threadLock(true);
static bool __paused = false;

void pthread_empty(pthread_t& t) {
#ifdef CDK_WINDOWS
	t.p = NULL;
	t.x = 0;
#else
	t = 0;
#endif
}

bool pthread_exists(const pthread_t& t) {
#ifdef CDK_WINDOWS
	return t.p != NULL;
#else
	return t != 0;
#endif
}

CSThread::CSThread() {
    _queue = new CSArray<CSTask>();
}

CSThread::~CSThread() {
    _queue->release();
    
    if (_autoreleasePool) {
        delete _autoreleasePool;
    }
}

CSThread* CSThread::mainThread() {
    return __mainThread;
}

CSThread* CSThread::currentThread() {
    pthread_t pt = pthread_self();
    if (pthread_equal(pt, __mainThread->_pthread)) {
        return __mainThread;
    }
    synchronized(__threadLock) {
        for (int i = 0; i < __threads.size(); i++) {
            CSThread* thread = __threads[i];
            if (pthread_equal(pt, thread->_pthread)) {
                return thread;
            }
        }
    }
    return NULL;
}

void CSThread::yield() {
    sched_yield();
}

void CSThread::sleep(float timeout) {
#ifdef CDK_WINDOWS
	Sleep(timeout * 1000);
#else
    usleep(timeout * 1000000);
#endif
}

bool CSThread::isMainThread() const {
    return this == __mainThread;
}

void CSThread::addTask(CSTask* task) {
    if (task->start()) {
        synchronized(this) {
            _queue->addObject(task);
            if (this != __mainThread) {
                signal();
            }
        }

        if (task->activity() == CSTaskActivityWait && !isActive()) {
            synchronized(task) {
                if (task->isAlive()) {
                    task->wait();
                }
            }
        }
    }
}

float CSThread::execute() {
    double minNextElapsed = DoubleMax;
    
    int i = 0;
    
	for (;;) {
		CSTask* task = NULL;
		synchronized(this) {
			if (i >= _queue->count()) goto fin;
			task = _queue->objectAtIndex(i);
		}
		double nextElapsed = (*task)();

		if (nextElapsed) {
			if (nextElapsed < minNextElapsed) {
				minNextElapsed = nextElapsed;
			}
			i++;
		}
		else {
			synchronized(this) {
				_queue->removeObjectAtIndex(i);
			}
		}
	}

fin:
    
    _autoreleasePool->drain();

    _timeStamp = CSTime::currentTime();
    _timeSeq++;
    
    return minNextElapsed != DoubleMax ? CSMath::max((float)(minNextElapsed - _timeStamp), 0.001f) : 0.0f;
}

void* CSThread::run(void* param) {
    CSThread* thread = (CSThread*)param;
    thread->_pthread = pthread_self();
    thread->_autoreleasePool = new CSAutoreleasePool(true);
    
    synchronized(__threadLock) {
        __threads.push_back(thread);
    }
    
	for (;;) {
        float remaining = thread->execute();
        
        synchronized(thread) {
			if (!thread->_alive) goto fin;
            else if (remaining > 0) thread->wait(remaining);
			else thread->wait();
        }
    }
fin:
    synchronized(__threadLock) {
        __threads.erase(std::remove(__threads.begin(), __threads.end(), thread), __threads.end());
    }
    
#ifdef CDK_ANDROID
    CSJNI::detachThread();
#endif
    
    delete thread->_autoreleasePool;
    thread->_autoreleasePool = NULL;
    thread->release();
    return NULL;
}

void CSThread::start(bool detach) {
    if (this != __mainThread && !_alive) {
        _timeStamp = _timeElapsed = CSTime::currentTime();

		_alive = true;
        _detach = detach;

        retain();

        if (detach) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&_pthread, &attr, &CSThread::run, this);
            pthread_attr_destroy(&attr);
        }
        else pthread_create(&_pthread, NULL, &CSThread::run, this);
    }
}

void CSThread::stop() {
	if (this != __mainThread && _alive) {
		_alive = false;
		synchronized(this) {
			signal();
		}
		if (!_detach) pthread_join(_pthread, NULL);
	}
}

void CSThread::keepAlive() {
    _timeStamp = CSTime::currentTime();
}

void CSThread::notifyPause() {
    _paused = true;
}

void CSThread::notifyResume() {
    _paused = false;
    _timeStamp = CSTime::currentTime();
}

void CSThread::notifyPauseApp() {
    synchronized(__threadLock) {
        __paused = true;
        __threadLock.signal();
    }
}

void CSThread::notifyResumeApp() {
    synchronized(__threadLock) {
        double currentTime = CSTime::currentTime();
        if (!__mainThread->_paused) {
            __mainThread->_timeStamp = __mainThread->_timeElapsed = currentTime;
        }
        for (int i = 0; i < __threads.size(); i++) {
			CSThread* thread = __threads[i];
            if (!thread->_paused) {
                thread->_timeStamp = thread->_timeElapsed = currentTime;
            }
        }
        __paused = false;
        __threadLock.signal();
    }
}

void CSThread::initialize() {
    CSAssert(!__mainThread, "invalid operation");

    __mainThread = new CSThread();
    __mainThread->_pthread = pthread_self();
    __mainThread->_timeStamp = __mainThread->_timeElapsed = CSTime::currentTime();
	__mainThread->_alive = true;
    __mainThread->_detach = true;
    __mainThread->_autoreleasePool = new CSAutoreleasePool(false);
#ifndef DEBUG
    pthread_t mp;
    pthread_create(&mp, NULL, &CSThread::runMonitor, NULL);
#endif
}

void CSThread::finalize() {
    __mainThread->_alive = false;
    synchronized(__threadLock) {
        release(__mainThread);
    }
}

void CSThread::reload() {
    __mainThread->_pthread = pthread_self();
}

#ifndef DEBUG
void* CSThread::runMonitor(void* param) {
    for (; ; ) {
        int64 currentTime = CSTime::currentTimeSecond();
        
        synchronized(__threadLock) {
            if (!__mainThread) goto exit;
            if (!__mainThread->_paused && currentTime - __mainThread->_timeStamp > KeepAliveDuration) {
                pthread_kill(__mainThread->_pthread, SIGABRT);
                goto exit;
            }
            for (int i = 0; i < __threads.size(); i++) {
                CSThread* thread = __threads[i];
                if (!thread->_paused && currentTime - thread->_timeStamp > KeepAliveDuration) {
                    pthread_kill(thread->_pthread, SIGABRT);
                    goto exit;
                }
            }
            if (!__paused) {
                __threadLock.wait(5);
            }
			if (!__mainThread) goto exit;
            if (__paused) {
                __threadLock.wait();
            }
        }
    }
exit:
    return NULL;
}
#endif
