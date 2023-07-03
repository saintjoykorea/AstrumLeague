#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSFontImpl.h"

#include "CSBuffer.h"
#include "CSFile.h"

#include <windows.h>

#include <freetype/ftsnames.h>

CSFontImpl::FaceEntry::FaceEntry(FT_Face face, const char* name, CSFontStyle style) : face(face), name(strdup(name)), style(style) {

}
CSFontImpl::FaceEntry::~FaceEntry() {
	FT_Done_Face(face);
	free(name);
}

CSFontImpl* CSFontImpl::__sharedImpl = NULL;

CSFontImpl::CSFontImpl() {
	if (FT_Init_FreeType(&_library)) {
		CSErrorLog("can not load font library");
		abort();
	}

	//현재 시스템폰트는 Arial만 지원
	{
		char winDir[MAX_PATH];
		GetWindowsDirectory(winDir, MAX_PATH);
		const char* systemFacePath = CSString::cstringWithFormat("%s\\Fonts\\arial.ttf", winDir);
		if (FT_New_Face(_library, systemFacePath, 0, &_systemFace) != 0) {
			CSErrorLog("can not load system font:%s", systemFacePath);
			abort();
		}
	}

	_faces = new CSArray<FaceEntry>();
}

CSFontImpl::~CSFontImpl() {
	FT_Done_Face(_systemFace);
	_faces->release();
	FT_Done_FreeType(_library);
}

void CSFontImpl::initialize() {
	if (!__sharedImpl) {
		__sharedImpl = new CSFontImpl();
	}
}

void CSFontImpl::finalize() {
	if (__sharedImpl) {
		delete __sharedImpl;
		__sharedImpl = NULL;
	}
}

void CSFontImpl::loadFaces(const char* name, CSFontStyle style, const char* path) {
	FT_Face face;

	if (FT_New_Face(_library, path, -1, &face) == 0) {
		int faceCount = face->num_faces;
		FT_Done_Face(face);

		for (int i = 0; i < faceCount; i++) {
			if (FT_New_Face(_library, path, i, &face) == 0) {
				new (&_faces->addObject()) FaceEntry(face, name, style);
			}
		}
		CSLog("load font:%s faces:%d", path, faceCount);
	}
	else {
		CSErrorLog("can not load font:%s", path);
	}
}

CSArray<FT_Face>* CSFontImpl::getFaces(const char* name, CSFontStyle style) {
	CSArray<FT_Face>* faces = new CSArray<FT_Face>();
	if (name) {
		foreach(const FaceEntry&, e, _faces) {
			if (strcmp(e.name, name) == 0 && e.style == style) {
				faces->addObject(e.face);
			}
		}
		if (faces->count()) return faces;

		CSWarnLog("unabled to find faces:%s", name);
	}
	faces->addObject(_systemFace);
	return faces;
}

CSArray<FT_Face>* CSFontImpl::getSystemFaces(CSFontStyle style) {
	CSArray<FT_Face>* faces = new CSArray<FT_Face>();
	faces->addObject(_systemFace);
	return faces;
}

#endif