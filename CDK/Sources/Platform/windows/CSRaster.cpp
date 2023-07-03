#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSRaster.h"

#include "CSFontImpl.h"

#include <windows.h>
#include <FreeImage.h>

void CSRaster::loadFontHandle(const char* name, CSFontStyle style, const char* path) {
	CSFontImpl::sharedImpl()->loadFaces(name, style, path);
}

void* CSRaster::createSystemFontHandle(float size, CSFontStyle style) {
	return CSFontImpl::sharedImpl()->getSystemFaces(style);
}

void* CSRaster::createFontHandle(const char* name, float size, CSFontStyle style) {
	return CSFontImpl::sharedImpl()->getFaces(name, style);
}

void CSRaster::destroyFontHandle(void* handle) {
	CSArray<FT_Face>* faces = (CSArray<FT_Face>*)handle;

	faces->release();
}

CSSize CSRaster::characterSize(const uchar* cc, uint cclen, const CSFont* font) {
	CSArray<FT_Face>* faces = (CSArray<FT_Face>*)font->handle();
	
	foreach (FT_Face, face, faces) {
		FT_Set_Char_Size(face, font->size() * 64, 0, 72, 72);

		CSSize size;
		size.width = 0;
		size.height = 0;

		uint i = 0;

		bool success = true;

		while (i < cclen) {
			uint code = CSString::toCodePoint(cc, i);
			uint index = FT_Get_Char_Index(face, code);
			if (index == 0 || FT_Load_Glyph(face, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP) != 0) {
				success = false;
				break;
			}

			float w = face->glyph->metrics.horiAdvance / 64.0f;
			float h = font->size() + (-face->glyph->metrics.horiBearingY + face->glyph->metrics.height) / 64.0f;

			if (size.width < w) size.width = w;
			if (size.height < h) size.height = h;
		}

		if (success) return size;
	}
	return CSSize::Zero;
}

void* CSRaster::createRasterFromCharacter(const uchar* cc, uint cclen, const CSFont* font, uint& width, uint& height, float& offy) {
	CSArray<FT_Face>* faces = (CSArray<FT_Face>*)font->handle();

	foreach (FT_Face, face, faces) {
		FT_Set_Char_Size(face, font->size() * 64, 0, 72, 72);

		width = 0;
		height = 0;
		offy = 0;

		float bottom = 0;

		uint i = 0;
		bool success = true;

		while (i < cclen) {
			uint code = CSString::toCodePoint(cc, i);
			uint index = FT_Get_Char_Index(face, code);
			if (index == 0 || FT_Load_Glyph(face, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP) != 0) {
				success = false;
				break;
			}

			int dx = CSMath::max((int)(face->glyph->metrics.horiBearingX >> 6) + 1, 0);
			int w = dx + (face->glyph->metrics.width >> 6);
			if (width < w) width = w;
			float y = face->glyph->metrics.horiBearingY / 64.0f;
			if (y > offy) offy = y;
			float b = -y + (face->glyph->metrics.height / 64.0f);
			if (bottom < b) bottom = b;
		}

		height = CSMath::ceil(offy + bottom);

		if (success) {
			byte* buf = (byte*)calloc(width * height, 4);

			if (!buf) return NULL;

			i = 0;

			while (i < cclen) {
				uint code = CSString::toCodePoint(cc, i);
				if (!u_isgraph(code)) continue;
				uint index = FT_Get_Char_Index(face, code);
				FT_Load_Glyph(face, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
				FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

				//CSLog("code point:%d / %d = %04X", i, cclen, code);

				int dx = CSMath::max((int)(face->glyph->metrics.horiBearingX >> 6) + 1, 0);
				int dy = offy - (face->glyph->metrics.horiBearingY / 64.0f);

				CSAssert(dx + face->glyph->bitmap.width <= width && dy >= 0 && dy + face->glyph->bitmap.rows <= height, "invalid code");

				for (int y = 0; y < face->glyph->bitmap.rows; y++) {
					for (int x = 0; x < face->glyph->bitmap.width; x++) {
						byte* cbuf = &buf[((y + dy) * width + (x + dx)) * 4];
						switch (face->glyph->bitmap.pixel_mode) {
							case FT_PIXEL_MODE_GRAY:
								{
									byte b = face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch + x];
									if (cbuf[0] < b) cbuf[0] = b;
									if (cbuf[1] < b) cbuf[1] = b;
									if (cbuf[2] < b) cbuf[2] = b;
									if (cbuf[3] < b) cbuf[3] = b;
								}
								break;
							case FT_PIXEL_MODE_BGRA:
								{
									byte* bp = &face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch + x * 4];
									if (cbuf[0] < bp[3]) cbuf[0] = bp[3];
									if (cbuf[1] < bp[2]) cbuf[1] = bp[2];
									if (cbuf[2] < bp[1]) cbuf[2] = bp[1];
									if (cbuf[3] < bp[0]) cbuf[3] = bp[0];
								}
								break;
						}
					}
				}
			}
			offy = font->size() - offy;
			return buf;
		}
	}
	return NULL;
}

void* CSRaster::createRasterFromImageSource(const void* source, uint len, uint& width, uint& height) {
	FIMEMORY* buf = FreeImage_OpenMemory((BYTE*)source, len);
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(buf);

	if (format == FIF_UNKNOWN) {
		width = 0;
		height = 0;
		return NULL;
	}
	FIBITMAP* img = FreeImage_LoadFromMemory(format, buf);
	FreeImage_CloseMemory(buf);

	if (!img) {
		width = 0;
		height = 0;
		return NULL;
	}

	BYTE *ptr = FreeImage_GetBits(img);

	if (!ptr) {
		FreeImage_Unload(img);
		width = 0;
		height = 0;
		return NULL;
	}
	width = FreeImage_GetWidth(img);
	height = FreeImage_GetHeight(img);

	byte* raster = (byte*)malloc(width * height * 4);

	if (!raster) {
		FreeImage_Unload(img);
		width = 0;
		height = 0;
		return NULL;
	}

	int i = 0;
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			RGBQUAD color;
			FreeImage_GetPixelColor(img, x, y, &color);

			raster[i++] = color.rgbRed;
			raster[i++] = color.rgbGreen;
			raster[i++] = color.rgbBlue;
			raster[i++] = 255;
		}
	}
	
	FreeImage_Unload(img);

	return raster;
}

bool CSRaster::saveImageFromRaster(const char* path, const void* raster, uint width, uint height) {
	FIBITMAP* img = FreeImage_Allocate(width, height, 24);
	if (!img) return false;
	
	const byte* ptr = (const byte*)raster;

	for (int y = 0; y < height; y++) {		//flip?
		for (int x = 0; x < width; x++) {
			RGBQUAD color;
			color.rgbRed = *ptr++;
			color.rgbGreen = *ptr++;
			color.rgbBlue = *ptr++;
			ptr++;

			FreeImage_SetPixelColor(img, x, y, &color);
		}
	}
	bool rtn = FreeImage_Save(FreeImage_GetFIFFromFilename(path), img, path, 0);
	FreeImage_Unload(img);
	return rtn;
}

#endif
