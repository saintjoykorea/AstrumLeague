#ifdef CDK_WINDOWS

#define CDK_IMPL
#include "CSApplication.h"

void CSApplication::setVersion(const char* version) {
	if (!_version) _version = new CSString(version);
	else _version->set(version);
}

void CSApplication::openURL(const char* url) {

}

static CSString* __clipboard = NULL;

const CSString* CSApplication::clipboard() {
	return __clipboard;
}

void CSApplication::setClipboard(const char* text) {
	if (text) {
		if (!__clipboard) __clipboard = new CSString(text);
		else __clipboard->set(text);
	}
	else {
		CSObject::release(__clipboard);
	}
}

void finish() {

}

void setNotificationEnabled(bool flag) {

}

void setNotificationNightEnabled(bool flag) {

}

#endif