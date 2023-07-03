//
//  CSLock.cpp
//  CDK
//
//  Created by chan on 13. 4. 10..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSLock.h"
#include "CSThread.h"
#include "CSTime.h"
#include "CSDictionary.h"

#include <sys/time.h>

#ifdef CS_LOCK_TIME
static CSDictionary<int, float>* __waitingTimes = new CSDictionary<int, float>();
static float __totalWaitingTime = 0.0f;
#endif

CSLock::CSLock(bool recursive) :
    _cond(PTHREAD_COND_INITIALIZER)
{
    if (recursive) {
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER
# define PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif
        _mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    }
    else {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
    }
#ifdef CS_ASSERT_DEBUG
	pthread_empty(_owner);
    _ownCount = 0;
#endif
}

CSLock::~CSLock() {
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);
}

bool CSLock::trylock(int op) {
#ifdef CS_LOCK_TIME
    double currentTime = CSTime::currentTime();
#endif
    bool lock = (pthread_mutex_trylock(&_mutex) == 0);

#ifdef CS_ASSERT_DEBUG
    if (lock) {
        _owner = pthread_self();
        _ownCount++;
    }
#endif
    
#ifdef CS_LOCK_TIME
    float delay = CSTime::currentTime() - currentTime;
	if (op) {
		synchronized(__waitingTimes) {
			float* pv = __waitingTimes->tryGetObjectForKey(op);
			if (pv) {
				*pv += delay;
			}
			else {
				__waitingTimes->setObject(op, delay);
			}
		}
	}
	__totalWaitingTime += delay;
#endif
    return lock;
}

void CSLock::lock(int op) {
#ifdef CS_LOCK_TIME
    double currentTime = CSTime::currentTime();
#endif
    pthread_mutex_lock(&_mutex);
#ifdef CS_ASSERT_DEBUG
    _owner = pthread_self();
    _ownCount++;
#endif
#ifdef CS_LOCK_TIME
    float delay = CSTime::currentTime() - currentTime;
	if (op) {
		synchronized(__waitingTimes) {
			float* pv = __waitingTimes->tryGetObjectForKey(op);
			if (pv) {
				*pv += delay;
			}
			else {
				__waitingTimes->setObject(op, delay);
			}
		}
	}
	__totalWaitingTime += delay;
#endif
}

void CSLock::unlock() {
#ifdef CS_ASSERT_DEBUG
    pthread_t owner = _owner;
    CSAssert(pthread_exists(owner) && pthread_equal(owner, pthread_self()), "invalid operation");
    if (--_ownCount == 0) {
		pthread_empty(_owner);
    }
#endif
    pthread_mutex_unlock(&_mutex);
}

void CSLock::wait() {
    assertOnActive();
    
    CSThread* thread = CSThread::currentThread();
    if (thread) thread->notifyPause();
#ifdef CS_ASSERT_DEBUG
    uint ownCount = _ownCount;
	pthread_empty(_owner);
    _ownCount = 0;
#endif
    pthread_cond_wait(&_cond, &_mutex);
#ifdef CS_ASSERT_DEBUG
    _owner = pthread_self();
    _ownCount = ownCount;
#endif
    if (thread) thread->notifyResume();
}

void CSLock::wait(float timeout) {
    assertOnActive();
    
    CSThread* thread = CSThread::currentThread();
    if (thread) thread->notifyPause();
#ifdef CS_ASSERT_DEBUG
    uint ownCount = _ownCount;
	pthread_empty(_owner);
    _ownCount = 0;
#endif
    struct timeval now;
    struct timespec ts;
    gettimeofday(&now, NULL);
    ts.tv_sec = now.tv_sec + (int)timeout;
    ts.tv_nsec = now.tv_usec * 1000 + (int)((timeout - (int)timeout) * 1000);
    
    pthread_cond_timedwait(&_cond, &_mutex, &ts);
#ifdef CS_ASSERT_DEBUG
    _owner = pthread_self();
    _ownCount = ownCount;
#endif
    if (thread) thread->notifyResume();
}

void CSLock::signal() {
    assertOnActive();
    
    pthread_cond_signal(&_cond);
}

#ifdef CS_ASSERT_DEBUG
bool CSLock::assertOnActive(bool abort) const {
    pthread_t owner = _owner;
    
    if (pthread_exists(owner) && pthread_equal(owner, pthread_self())) {
        return true;
    }
    else {
        CSAssert(!abort, "should be locked");
        return false;
    }
}

bool CSLock::assertOnDeactive(bool abort) const {
    pthread_t owner = _owner;
    
    if (pthread_exists(owner) && pthread_equal(owner, pthread_self())) {
        CSAssert(!abort, "should be unlocked");
        return false;
    }
    else {
        return true;
    }
}

#endif

#ifdef CS_LOCK_TIME

float CSLock::waitingTime(int op) {
	synchronized(__waitingTimes) {
		float* pv = __waitingTimes->tryGetObjectForKey(op);
		if (pv) return *pv;
	}
	return 0.0f;
}

float CSLock::totalWaitingTime() {
	return __totalWaitingTime;
}

void CSLock::resetWaitingTime() {
	synchronized(__waitingTimes) {
		__waitingTimes->removeAllObjects();
	}
	__totalWaitingTime = 0.0f;
}

#endif
