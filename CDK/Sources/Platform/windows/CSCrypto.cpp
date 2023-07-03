#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSCrypto.h"

#include <openssl/aes.h>
#include <openssl/sha.h>

#define AES_IV_SIZE 16

const void* CSCrypto::encrypt(const void* data, uint& length, const char* key) {
	int keyBit;
	switch (strlen(key)) {
		case 32:
			keyBit = 256;
			break;
		case 16:
			keyBit = 128;
			break;
		default:
			return NULL;
	}

	byte iv[AES_IV_SIZE] = {};
	memcpy(iv, key, AES_IV_SIZE);

	AES_KEY aes_ks3;
	AES_set_encrypt_key((const byte*)key, keyBit, &aes_ks3);

	uint body_len = length / AES_BLOCK_SIZE * AES_BLOCK_SIZE;
	uint out_len = body_len;
	if (length >= body_len) out_len += AES_BLOCK_SIZE;

	byte* out = (byte*)malloc(out_len);
	if (!out) {
		length = 0;
		return NULL;
	}
	if (body_len) AES_cbc_encrypt((const byte*)data, out, body_len, &aes_ks3, iv, AES_ENCRYPT);

	byte padding_len = AES_BLOCK_SIZE - length % AES_BLOCK_SIZE;
	byte remaining[AES_BLOCK_SIZE];
	memset(remaining, padding_len, sizeof(remaining));
	if (length > body_len) memcpy(remaining, (const byte*)data + body_len, length - body_len);
	AES_cbc_encrypt(remaining, out + body_len, AES_BLOCK_SIZE, &aes_ks3, iv, AES_ENCRYPT);

	length = out_len;

	autofree(out);

	return out;
}

const void* CSCrypto::decrypt(const void* data, uint& length, const char* key) {
	int keyBit;
	switch (strlen(key)) {
		case 32:
			keyBit = 256;
			break;
		case 16:
			keyBit = 128;
			break;
		default:
			return NULL;
	}

	byte iv[AES_IV_SIZE] = {};
	memcpy(iv, key, AES_IV_SIZE);

	AES_KEY aes_ks3;
	AES_set_decrypt_key((const byte*)key, keyBit, &aes_ks3);

	byte* out = (byte*)malloc(length);
	if (!out) {
		length = 0;
		return NULL;
	}
	AES_cbc_encrypt((const byte*)data, out, length, &aes_ks3, iv, AES_DECRYPT);

	length -= out[length - 1];

	autofree(out);

	return out;
}

CSString* CSCrypto::sha1(const void* data, uint length) {
	SHA_CTX context;

	byte md[SHA_DIGEST_LENGTH];

	if (!SHA1_Init(&context)) return NULL;
	if (!SHA1_Update(&context, (const byte*)data, length)) return NULL;
	if (!SHA1_Final(md, &context)) return NULL;

	CSString* rtn = CSString::string();
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		rtn->appendFormat("%02x", md[i]);
	}
	return rtn;
}

#endif
