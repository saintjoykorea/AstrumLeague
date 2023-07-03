//
//  CSLock.h
//  CDK
//
//  Created by chan on 13. 4. 10..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSLock__
#define __CDK__CSLock__

#include "CSConfig.h"

#include <pthread.h>

class CSLock {
private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
#ifdef CS_ASSERT_DEBUG
    pthread_t _owner;
    unsigned int _ownCount;
#endif
public:
    CSLock(bool recursive = false);
    ~CSLock();
    
    bool trylock(int op = 0);
    void lock(int op = 0);
    void wait();
    void wait(float timeout);
    void signal();
    void unlock();
#ifdef CS_ASSERT_DEBUG
    bool assertOnActive(bool abort = true) const;
    bool assertOnDeactive(bool abort = true) const;
#else
    inline bool assertOnActive(bool abort = true) const {
        return true;
    }
    inline bool assertOnDeactive(bool abort = true) const {
        return true;
    }
#endif

#ifdef CS_LOCK_TIME
	static float waitingTime(int op);
	static float totalWaitingTime();
	static void resetWaitingTime();
#else
    static inline float waitingTime(int op) {
        return 0.0f;
    }
    static inline float totalWaitingTime() {
        return 0.0f;
    }
	static inline void resetWaitingTime() {

	}
#endif
};

#endif /* defined(__CDK__CSLock__) */
