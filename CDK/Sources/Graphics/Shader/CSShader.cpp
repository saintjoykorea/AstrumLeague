//
//  CSShader.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSShader.h"

#include "CSOpenGL.h"

#include "CSString.h"

CSShader::CSShader(uint object) : _object(object) {
    _identifier.reset();
}

CSShader::~CSShader() {
    if (_identifier.isValid()) {
        glDeleteShaderCS(_object);
    }
}

CSShader* CSShader::create(const char* source, uint type, const char* options) {
    uint object = glCreateShaderCS(type);
    
    std::string strbuf;
    
#ifdef CDK_WINDOWS
	strbuf.append("#version 330\n");
#else
	strbuf.append("#version 300 es\n");
#endif

	if (options) {
		char* str = strdup(options);
		CSAssert(str, "options string fail");
		char* tok = strtok(str, ";");

		while (tok) {
			uint len = strlen(tok);
			if (len) {
				for (uint i = 0; i < len; i++) {
					if (tok[i] == '=') {
						tok[i] = ' ';
					}
				}
				strbuf.append("#define ");
				strbuf.append(tok);
				strbuf.append("\n");
			}
			tok = strtok(NULL, ";");
		}
		free(str);
	}
	strbuf.append(source);
	source = strbuf.c_str();
    
    glShaderSourceCS(object, 1, &source, NULL);
    glCompileShaderCS(object);
    
    int status;
    glGetShaderivCS(object, GL_COMPILE_STATUS, &status);
    
    if (status == 0) {
#ifdef CS_OES_DEBUG
        int logLength;
        glGetShaderivCS(object, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            logLength++;
            GLchar* log = (GLchar*)alloca(logLength);
            glGetShaderInfoLogCS(object, logLength, &logLength, log);
            CSErrorLog("compile log:\n%s\n\n%s", log, source);
        }
#endif
		glDeleteShaderCS(object);
        return NULL;
    }
    return new CSShader(object);
}

