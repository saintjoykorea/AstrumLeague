//
//  CSMemory.h
//  CDK
//
//  Created by 김찬 on 12. 12. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSMemory__
#define __CDK__CSMemory__

#include <stdlib.h>
#include <string.h>

void* fmalloc(size_t size);
void* fcalloc(size_t n, size_t size);
void* frealloc(void* ptr, size_t size);
void autofree(void* p);

#endif /* defined(__CDK__CSMemory__) */
