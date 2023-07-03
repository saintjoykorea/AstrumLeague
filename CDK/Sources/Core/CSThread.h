//
//  CSThread.h
//  CDK
//
//  Created by chan on 13. 2. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSThread__
#define __CDK__CSThread__

#include "CSArray.h"
#include "CSTask.h"
#include "CSTime.h"
#include "CSAutoreleasePool.h"
#include "CSDiagnostics.h"

void pthread_empty(pthread_t& t);
bool pthread_exists(const pthread_t& t);

class CSThread : public CSObject {
private:
    CSArray<CSTask>* _queue;
    CSAutoreleasePool* _autoreleasePool;
	CSDiagnostics _diagnostics;
    pthread_t _pthread;
    double _timeStamp;
    double _timeElapsed;
    uint64 _timeSeq;
	bool _alive;
    bool _detach;
    bool _paused;
public:
    CSThread();
private:
    ~CSThread();
public:
    static inline CSThread* thread() {
        return autorelease(new CSThread());
    }
    
    static CSThread* mainThread();
    static CSThread* currentThread();
    
    static void yield();
    static void sleep(float timeout);
    
    inline bool isAlive() const {
		return _alive;
    }
    
    inline bool isActive() const {
        return pthread_equal(_pthread, pthread_self()) != 0;
    }
    
    bool isMainThread() const;
    
    inline double timeStamp() const {
        return _timeStamp;
    }
    inline double timeTotal() const {
        return CSTime::currentTime() - _timeElapsed;
    }
	inline uint64 timeSequence() const {
		return _timeSeq;
	}
    inline double timeAverage() const {
        return _timeSeq ? timeTotal() / _timeSeq : 0.0;
    }
    inline void resetTime() {
        _timeElapsed = CSTime::currentTime();
        _timeSeq = 0;
    }

    inline CSAutoreleasePool* autoreleasePool() {
        CSAssert(isActive(), "invalid operation");
        return _autoreleasePool;
    }

	inline CSDiagnostics* diagnostics() {
		return &_diagnostics;
	}

    void addTask(CSTask* task);
    void start(bool detach);
	void stop();
    
    void keepAlive();
#ifdef CDK_IMPL
    void notifyPause();
    void notifyResume();
    static void notifyPauseApp();
    static void notifyResumeApp();
    
    float execute();
    
    static void initialize();
    static void finalize();
    static void reload();
    static void* run(void* thread);
    static void* runMonitor(void* param);
#endif
};

#endif /* defined(__CDK__CSThread__) */
