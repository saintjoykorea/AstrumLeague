//
//  CSTask.h
//  CDK
//
//  Created by chan on 13. 4. 19..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef __CDK__CSTask__
#define __CDK__CSTask__

#include "CSDelegate.h"

#include "CSArray.h"

enum CSTaskActivity {
    CSTaskActivityNone,
    CSTaskActivityWait,
    CSTaskActivityRepeat
};

class CSThread;

class CSTask : public CSObject {
private:
	CSDelegate<void>* _delegate;
    CSTaskActivity _activity;
    double _delay;
    double _elapsed;
    bool _alive;
public:
    CSTask(CSDelegate<void>* delegate, float delay = 0, CSTaskActivity activity = CSTaskActivityNone);
private:
    ~CSTask();
public:
    static inline CSTask* task(CSDelegate<void>* delegate, float delay = 0, CSTaskActivity activity = CSTaskActivityNone) {
        return autorelease(new CSTask(delegate, delay, activity));
    }
    
    inline CSDelegate<void>* delegate() const {
        return _delegate;
    }
    
    inline void setDelay(float delay) {
        _delay = delay;
    }
    
    inline float delay() const {
        return _delay;
    }
    
    inline void setActivity(CSTaskActivity activity) {
        _activity = activity;
    }
    
    inline CSTaskActivity activity() const {
        return _activity;
    }
    
    inline bool isAlive() const {
        return _alive;
    }
    
	void stop();
private:
    bool start();

	double operator()();
    
    friend class CSThread;
};

#endif /* defined(__CDK__CSTask__) */
