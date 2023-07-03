#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSVideoViewBridge.h"

void* CSVideoViewBridge::createHandle(CSVideoView* videoView) {
	return NULL;	//TODO:IMPL
}
void CSVideoViewBridge::destroyHandle(void* handle) {
	//TODO:IMPL
}
void CSVideoViewBridge::addToView(void* handle) {
	//TODO:IMPL
}
void CSVideoViewBridge::removeFromView(void* handle) {
	//TODO:IMPL
}
void CSVideoViewBridge::setFrame(void* handle, const CSRect& frame) {
	//TODO:IMPL
}

void CSVideoViewBridge::playStart(void* handle, const char *path) {
	//TODO:IMPL
}
#endif
