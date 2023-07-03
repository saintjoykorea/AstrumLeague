#define CDK_IMPL

#include "CSRootImage.h"

#include "CSOpenGL.h"
#include "CSBytes.h"

bool CSRootImage::loadDXTData(const byte *data, uint length, uint format, Texture &texture) {
#ifdef CDK_WINDOWS
	uint internalFormat;
	uint blockLength;
	switch (format & 0xff) {
		case CSImageFormatDXT1:
			internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_NV;
			blockLength = 8;
			break;
		case CSImageFormatDXT3:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_NV;
			blockLength = 16;
			break;
		case CSImageFormatDXT5:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_NV;
			blockLength = 16;
			break;
		default:
			return false;
	}

	texture.width = (texture.contentWidth + 3) / 4 * 4;
	texture.height = (texture.contentHeight + 3) / 4 * 4;

	return loadCompressedData(data, length, format, internalFormat, 4, blockLength, false, texture);
#else
	return false;
#endif
}
