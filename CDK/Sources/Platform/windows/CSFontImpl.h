#ifdef CDK_WINDOWS

#ifndef __CDK__CSFontImpl__
#define __CDK__CSFontImpl__

#include "CSArray.h"
#include "CSFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class CSFontImpl {
private:
	FT_Library _library;
	FT_Face _systemFace;

	struct FaceEntry {
		FT_Face face;
		char* name;
		CSFontStyle style;

		FaceEntry(FT_Face face, const char* name, CSFontStyle style);
		~FaceEntry();
	};
	CSArray<FaceEntry>* _faces;

	CSFontImpl();
	~CSFontImpl();

	static CSFontImpl* __sharedImpl;
public:
	static void initialize();
	static void finalize();

	static inline CSFontImpl* sharedImpl() {
		return __sharedImpl;
	}

	void loadFaces(const char* name, CSFontStyle style, const char* path);
	CSArray<FT_Face>* getFaces(const char* name, CSFontStyle style);
	CSArray<FT_Face>* getSystemFaces(CSFontStyle style);
};

#endif

#endif