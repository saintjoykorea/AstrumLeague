//
//  CSFileRawImpl.cpp
//  CDK
//
//  Created by 김찬 on 2015. 5. 14..
//  Copyright (c) 2015년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSFileRawImpl.h"

#include "CSFile.h"

#include "CSMath.h"

#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define GzipWindowBits				31

#define GzipUncompressBufferRate	4

static const uint GzipBufferIncrement = 65536;

void* CSFile::createCompressedRawWithData(const void* src, uint srcLength, uint& destLength) {
    z_stream stm = {};
    stm.next_in = (Bytef*)src;
    stm.total_in = stm.avail_in = srcLength;
    
    int rtn = deflateInit2(&stm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, GzipWindowBits, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    
    if (rtn != Z_OK) {
        destLength = 0;
        return NULL;
    }
    uint bufferSize = srcLength;
    void* dest = malloc(srcLength);
    
    if (!dest) {
        destLength = 0;
        deflateEnd(&stm);
        return NULL;
    }
    
    stm.next_out = (Byte*)dest;
    stm.avail_out = bufferSize;
    
    for (; ; ) {
        rtn = deflate(&stm, Z_FINISH);
        
        switch (rtn) {
            case Z_OK:
                {
                    void* tmp = realloc(dest, bufferSize + GzipBufferIncrement);
                    
                    if (!tmp) {
                        free(dest);
                        dest = NULL;
                        destLength = 0;
                        goto exit;
                    }
                    dest = tmp;
                }
                stm.next_out = (Bytef*)dest + bufferSize;
                stm.avail_out = GzipBufferIncrement;
                bufferSize += GzipBufferIncrement;
                break;
            case Z_STREAM_END:
                destLength = stm.total_out;
				if (destLength < bufferSize) {
					dest = realloc(dest, destLength);
				}
                goto exit;
            default:
                free(dest);
                dest = NULL;
                destLength = 0;
                goto exit;
        }
    }
exit:
    deflateEnd(&stm);
    
    return dest;
}

void* CSFile::createRawWithCompressedData(const void* src, uint srcLength, uint destOffset, uint& destLength) {
    z_stream stm = {};
    
    stm.next_in = (Bytef*)src;
    stm.total_in = stm.avail_in = srcLength;
    
    int rtn = inflateInit2(&stm, GzipWindowBits);
    
    if (rtn != Z_OK) {
        destLength = 0;
        return NULL;
    }
    
    if (destOffset) {
        void* buf = malloc(destOffset);
        
        if (!buf) {
            destLength = 0;
            inflateEnd(&stm);
            return NULL;
        }
        stm.next_out = (Bytef*)buf;
        stm.avail_out = destOffset;
            
        rtn = inflate(&stm, Z_NO_FLUSH);      //no finish
            
		free(buf);
		
		if (rtn != Z_OK) {
            destLength = 0;
            inflateEnd(&stm);
            return NULL;
        }
    }
    
    uint bufferSize = destLength ? destLength : srcLength * GzipUncompressBufferRate;
    void* dest = malloc(bufferSize);
    
    if (!dest) {
        destLength = 0;
        inflateEnd(&stm);
        return NULL;
    }
    
    stm.next_out = (Byte*)dest;
    stm.avail_out = bufferSize;
    
    for (; ; ) {
        rtn = inflate(&stm, Z_NO_FLUSH);      //no finish
        
        switch (rtn) {
            case Z_OK:
                if (destLength) {
                    if (destOffset + destLength == stm.total_out) {
                        goto exit;
                    }
                }
                else {
                    void* tmp = realloc(dest, bufferSize + GzipBufferIncrement);
                    
                    if (!tmp) {
                        free(dest);
                        dest = NULL;
                        destLength = 0;
                        goto exit;
                    }
                    dest = tmp;
                    
                    stm.next_out = (Bytef*)dest + bufferSize;
                    stm.avail_out = GzipBufferIncrement;
                    bufferSize += GzipBufferIncrement;
                }
                break;
            case Z_STREAM_END:
                destLength = stm.total_out - destOffset;
				if (destLength < bufferSize) {
					dest = realloc(dest, destLength);
				}
                goto exit;
            default:
                free(dest);
                dest = NULL;
                destLength = 0;
                goto exit;
        }
    }
exit:
    inflateEnd(&stm);
    
    return dest;
}

void* createRawWithCompressedContentOfFile(const char* path, uint offset, uint& length) {
	uint bufferSize;
	if (length) bufferSize = length;
	else {
		FILE* cfp = fopen(path, "rb");
		if (!cfp) {
			length = 0;
			return NULL;
		}
		fseek(cfp, 0, SEEK_END);
		uint compLength = ftell(cfp);
		fclose(cfp);

		bufferSize = compLength * GzipUncompressBufferRate;
	}
    
	gzFile fp = gzopen(path, "rb");

	if (!fp) {
		length = 0;
		return NULL;
	}
	if (offset) {
		gzseek(fp, offset, SEEK_CUR);
	}

    void* data = malloc(bufferSize);
    
    if (data) {
        if (length) {
            int len = gzread(fp, data, length);
            
            if (len >= 0) {
                length = len;
            }
            else {
                free(data);
                data = NULL;
                length = 0;
            }
        }
        else {
            for (; ; ) {
                int remaining = bufferSize - length;
                int len = gzread(fp, (byte*)data + length, remaining);
                
                if (len < 0) {
                    free(data);
                    data = NULL;
                    length = 0;
                    break;
                }
                if (len == 0) {
                    break;
                }
                length += len;
                if (len < remaining) {
                    break;
                }
                bufferSize += GzipBufferIncrement;
                void* tmp = realloc(data, bufferSize);
                
                if (!tmp) {
                    free(data);
                    data = NULL;
                    length = 0;
                    break;
                }
                data = tmp;
            }
			if (length < bufferSize) {
				data = realloc(data, length);
			}
        }
    }
    else {
        length = 0;
    }
    gzclose(fp);
    
    return data;
}

void* createRawWithContentOfFile(const char* path, uint offset, uint& length) {
    FILE* fp = fopen(path, "rb");
    
    void* data = NULL;
    
    if (fp) {
        fseek(fp, 0, SEEK_END);
        uint end = ftell(fp);
        if (offset < end) {
            if (length == 0 || offset + length > end) {
                length = end - offset;
            }
            fseek(fp, offset, SEEK_SET);
            data = malloc(length);
            if (data) {
                length = fread(data, sizeof(byte), length, fp);
            }
            else {
                length = 0;
            }
        }
        else {
            length = 0;
        }
        fclose(fp);
    }
    else {
        length = 0;
    }
    return data;
}

bool writeRawToFile(const char* path, const void* data, uint length) {
    remove(path);
    
    FILE* fp = fopen(path, "wb");
    
    bool complete = false;
    
    if (fp) {
        if (length) {
            complete = (length == fwrite(data, 1, length, fp));
            if (complete) {
                complete = (fflush(fp) == 0);
            }
        }
        else {
            complete = true;
        }
        fclose(fp);
    }

    return complete;
}

bool writeCompressedRawToFile(const char* path, const void* data, uint length) {
    remove(path);
    
    gzFile fp = gzopen(path, "wb");
    
    if (!fp) {
        return false;
    }
    uint pos = 0;
    do {
        int len = gzwrite(fp, (const byte*)data + pos, length - pos);
        
        if (len <= 0) {
            remove(path);
            gzclose(fp);
            return false;
        }
        pos += len;
    } while (pos < length);
    
    int ret = gzflush(fp, Z_SYNC_FLUSH);
    
    gzclose(fp);

    if (ret == Z_OK) {
        return true;
    }
    else {
        remove(path);
        return false;
    }
}








