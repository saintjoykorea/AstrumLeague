//
//  CSProgram.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSProgram.h"

#include "CSOpenGL.h"

#include "CSString.h"

CSProgram::CSProgram() {
    _object = glCreateProgramCS();
    
    _identifier.reset();
}

CSProgram::~CSProgram() {
    if (_identifier.isValid()) {
        glDeleteProgramCS(_object);
    }
}

void CSProgram::attach(const CSShader* shader) {
    _identifier.use();
    
    glAttachShaderCS(_object, shader->object());
}

void CSProgram::detach(const CSShader* shader) {
    _identifier.use();
    
    glDetachShaderCS(_object, shader->object());
}

bool CSProgram::link() {
    _identifier.use();
    
    glLinkProgramCS(_object);

    int status;
    glGetProgramivCS(_object, GL_LINK_STATUS, &status);
    
    if (status == 0) {
#ifdef CS_OES_DEBUG
        int logLength;
        glGetProgramivCS(_object, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            logLength++;
			GLchar* log = (GLchar*)alloca(logLength);
            glGetProgramInfoLogCS(_object, logLength, &logLength, log);
            CSLog("CSProgram::link log:\n%s", log);
        }
#endif
        return false;
    }
    
    return true;
}

int CSProgram::uniformIndex(const char* name) const {
    _identifier.use();
    
    int uniform = glGetUniformLocationCS(_object, name);
    
    return uniform;
}

