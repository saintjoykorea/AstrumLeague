//
//  CSEncoder.cpp
//  CDK
//
//  Created by 김찬 on 13. 5. 9..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSEncoder.h"

#include "CSJNI.h"

CSString* CSEncoder::createStringWithBytes(const void* bytes, uint length, CSStringEncoding encoding) {
	const char* enc;
	switch (encoding) {
		case CSStringEncodingUTF8:
			return new CSString((const char*)bytes, length);
		case CSStringEncodingUTF16:
			enc = "UTF-16";
			break;
		case CSStringEncodingUTF16LE:
			enc = "UTF-16LE";
			break;
		case CSStringEncodingUTF16BE:
			enc = "UTF-16BE";
			break;
		default:
			return NULL;
	}

	CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassEncoder, "decodeString", "([BLjava/lang/String;)Ljava/lang/String;", false);

	jstring jenc = mi.env->NewStringUTF(enc);
	jbyteArray jbytes = mi.env->NewByteArray(length);
	mi.env->SetByteArrayRegion(jbytes, 0, length, (const jbyte*)bytes);
	jstring jstr = (jstring)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jbytes, jenc);

    CSString* rtn = NULL;
    
    if (jstr) {
        const char* str = mi.env->GetStringUTFChars(jstr, NULL);
        
        rtn = new CSString(str);

        mi.env->ReleaseStringUTFChars(jstr, str);
        mi.env->DeleteLocalRef(jstr);
    }
	mi.env->DeleteLocalRef(jenc);
	mi.env->DeleteLocalRef(jbytes);

	return rtn;
}

void* CSEncoder::createRawWithString(const char* str, uint& length, CSStringEncoding encoding) {
	const char* enc;
	switch (encoding) {
		case CSStringEncodingUTF8:
			{
				length = strlen(str);
                if (!length) {
                    return NULL;
                }
				void* buf = fmalloc(length);
                memcpy(buf, str, length);
				return buf;
			}
		case CSStringEncodingUTF16:
			enc = "UTF-16";
			break;
		case CSStringEncodingUTF16LE:
			enc = "UTF-16LE";
			break;
		case CSStringEncodingUTF16BE:
			enc = "UTF-16BE";
			break;
		default:
			return NULL;
	}

	void* rtn = NULL;

    CSJNIMethod mi;
	CSJNI::findMethod(mi, CSJNIClassEncoder, "encodeString", "(Ljava/lang/String;Ljava/lang/String;)[B", false);

    jstring jstr = mi.env->NewStringUTF(str);
	jstring jenc = mi.env->NewStringUTF(enc);

	jbyteArray joutbytes = (jbyteArray)mi.env->CallStaticObjectMethod(mi.classId, mi.methodId, jstr, jenc);

	if (joutbytes) {
        length = mi.env->GetArrayLength(joutbytes);
        if (length) {
            rtn = fmalloc(length);
            mi.env->GetByteArrayRegion(joutbytes, 0, length, (jbyte*)rtn);
        }
        mi.env->DeleteLocalRef(joutbytes);
	}

    mi.env->DeleteLocalRef(jstr);
    mi.env->DeleteLocalRef(jenc);

	return rtn;
}

#endif
