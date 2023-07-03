#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSEncoder.h"

CSString* CSEncoder::createStringWithBytes(const void* bytes, uint length, CSStringEncoding encoding) {
	switch (encoding) {
		case CSStringEncodingUTF8:
			return new CSString((const char*)bytes, length);
		case CSStringEncodingUTF16:
		case CSStringEncodingUTF16LE:
			{
				length /= sizeof(uchar);
				char* cstr = CSString::cstring((const uchar*)bytes, length, false);
				if (!cstr) {
					length = 0;
					return NULL;
				}
				CSString* str = new CSString(cstr);
				free(cstr);

				return str;
			}
		case CSStringEncodingUTF16BE:
			{
				uchar* dest = (uchar*)malloc(length);
				if (!dest) {
					length = 0;
					return NULL;
				}
				const uchar* src = (const uchar*)bytes;

				length /= sizeof(uchar);

				for (int i = 0; i < length; i++) {
					dest[i] = (uchar)(((src[i] << 8) & 0xFF00) | ((src[i] >> 8) & 0x00FF));
				}
				char* cstr = CSString::cstring(dest, length, false);
				free(dest);
				CSString* str = new CSString(cstr);
				free(cstr);

				return str;
			}
	}
	return NULL;
}

void* CSEncoder::createRawWithString(const char* str, uint& length, CSStringEncoding encoding) {
	length = strlen(str);

	if (length) {
		switch (encoding) {
			case CSStringEncodingUTF8:
				{
					void* buf = fmalloc(length);
					memcpy(buf, str, length);
					return buf;
				}
			case CSStringEncodingUTF16:
			case CSStringEncodingUTF16LE:
				{
					uchar* ustr = CSString::ucstring(str, false);
					length = u_strlen(ustr) * sizeof(uchar);
					return ustr;
				}
			case CSStringEncodingUTF16BE:
				{
					uchar* ustr = CSString::ucstring(str, false);
					length = u_strlen(ustr);
					for (int i = 0; i < length; i++) {
						ustr[i] = (uchar)(((ustr[i] << 8) & 0xFF00) | ((ustr[i] >> 8) & 0x00FF));
					}
					length *= sizeof(uchar);
					return ustr;
				}
		}
	}
	return NULL;
}

#endif