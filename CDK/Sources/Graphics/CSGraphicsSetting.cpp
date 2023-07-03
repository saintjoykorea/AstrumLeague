#define CDK_IMPL

#include "CSGraphicsSetting.h"

#include "CSRootImage.h"

#include "CSOpenGL.h"

static bool __usingMipmapLinear = true;

bool CSGraphicsSetting::usingMipmapLinear() {
	return __usingMipmapLinear;
}

void CSGraphicsSetting::setUsingMipmapLInear(bool on) {
	if (__usingMipmapLinear != on) {
		__usingMipmapLinear = on;

		foreach(const CSRootImage*, image, CSRootImage::allImages()) {
			uint format = image->format();

			if (format & CSImageFormatMipmapLinear) {
				glBindTextureCS(GL_TEXTURE_2D, image->texture0());
				
				GLint minFilter = on ?
					((format & CSImageFormatMinFilterNearest) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR) :
					((format & CSImageFormatMinFilterNearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
			
				glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

				uint texture1 = image->texture1();

				if (texture1) {
					glBindTextureCS(GL_TEXTURE_2D, image->texture1());
					glTexParameteriCS(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				}
			}
		}
	}
}
