//
//  CSTask.cpp
//  CDK
//
//  Created by chan on 13. 4. 19..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSTask.h"

#include "CSTime.h"

CSTask::CSTask(CSDelegate<void>* delegate, float delay, CSTaskActivity activity) : _delegate(retain(delegate)), _activity(activity), _delay(delay) {

}

CSTask::~CSTask() {
    _delegate->release();
}

double CSTask::operator()() {
	if (!_alive) {
		return 0.0;
	}
	double currentTime = CSTime::currentTime();

	if (currentTime >= _elapsed) {
		synchronized(this) {
			(*_delegate)();
		}

		switch (_activity) {
			case CSTaskActivityWait:
				synchronized(this) {
					_alive = false;
					signal();
				}
				return 0.0;
			case CSTaskActivityNone:
				_alive = false;
				return 0.0;
		}
		_elapsed = currentTime + _delay;
	}

	return _elapsed;
}

void CSTask::stop() {
	synchronized(this) {
		_alive = false;
		signal();
	}
}

bool CSTask::start() {
    if (!_alive) {
        _elapsed = _delay ? CSTime::currentTime() + _delay : 0.0;
        _alive = true;
        return true;
    }
    return false;
}
