//
//  CSOpenGL.h
//  CDK
//
//  Created by 김찬 on 12. 12. 7..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSOpenGL__
#define __CDK__CSOpenGL__

#include "CSConfig.h"

#ifdef CDK_ANDROID
# include <GLES3/gl32.h>
# include <GLES3/gl3ext.h>
#elif defined(CDK_IOS)
# include <OpenGLES/ES3/gl.h>
# include <OpenGLES/ES3/glext.h>
#elif defined(CDK_WINDOWS)
# include "GL/glew.h"
#else
#error "unknown platform"
#endif

#define GL_INVALID_THREAD   0xFFFF

#ifdef CS_OES_DEBUG

void __LOG_GL_ERROR(const char* tag, const char* func, int line);
# define LOG_GL_ERROR(tag)  __LOG_GL_ERROR(tag, __FUNCTION__, __LINE__)

int glGetErrorCS();

#else

# define LOG_GL_ERROR(tag);

# define glGetErrorCS()  glGetError()

#endif

extern unsigned int __glDrawCall;
static inline unsigned int glGetDrawCallCS() {
    unsigned int rtn = __glDrawCall;
    __glDrawCall = 0;
    return rtn;
}

#define glActiveTextureCS(a);                                   glActiveTexture(a); LOG_GL_ERROR("glActiveTexture");
#define glAttachShaderCS(a, b);                                 glAttachShader(a, b); LOG_GL_ERROR("glAttachShader");
#define glBindAttribLocationCS(a, b, c);                        glBindAttribLocation(a, b, c); LOG_GL_ERROR("glBindAttribLocation");
#define glBindBufferCS(a, b);                                   glBindBuffer(a, b); LOG_GL_ERROR("glBindBuffer");
#define glBindFramebufferCS(a, b);                              glBindFramebuffer(a, b); LOG_GL_ERROR("glBindFramebuffer");
#define glBindRenderbufferCS(a, b);                             glBindRenderbuffer(a, b); LOG_GL_ERROR("glBindRenderbuffer");
#define glBindTextureCS(a, b);                                  glBindTexture(a, b); LOG_GL_ERROR("glBindTexture");
#define glBindVertexArrayCS(a);									glBindVertexArray(a); LOG_GL_ERROR("glBindVertexArray");
#define glBlendColorCS(a, b, c, d);                             glBlendColor(a, b, c, d); LOG_GL_ERROR("glBlendColor");
#define glBlendEquationCS(a);                                   glBlendEquation(a); LOG_GL_ERROR("glBlendEquation");
#define glBlendEquationSeparateCS(a, b);                        glBlendEquationSeparate(a, b); LOG_GL_ERROR("glBlendEquationSeparate");
#define glBlendFuncCS(a, b);                                    glBlendFunc(a, b); LOG_GL_ERROR("glBlendFunc");
#define glBlendFuncSeparateCS(a, b, c, d);                      glBlendFuncSeparate(a, b, c, d); LOG_GL_ERROR("glBlendFuncSeparate");
#define glBlitFramebufferCS(a, b, c, d, e, f, g, h, i, j);      glBlitFramebuffer(a, b, c, d, e, f, g, h, i, j); LOG_GL_ERROR("glBlitFramebuffer");
#define glBufferDataCS(a, b, c, d);                             glBufferData(a, b, c, d); LOG_GL_ERROR("glBufferData");
#define glBufferSubDataCS(a, b, c, d);                          glBufferSubData(a, b, c, d); LOG_GL_ERROR("glBufferSubData");
#define glCheckFramebufferStatusCS(a)                           glCheckFramebufferStatus(a)
#define glClearCS(a);                                           glClear(a); LOG_GL_ERROR("glClear");
#define glClearColorCS(a, b, c, d);                             glClearColor(a, b, c, d); LOG_GL_ERROR("glClearColor");
#define glClearDepthfCS(a);                                     glClearDepthf(a); LOG_GL_ERROR("glClearDepthf");
#define glClearStencilCS(a);                                    glClearStencil(a); LOG_GL_ERROR("glClearStencil");
#define glColorMaskCS(a, b, c, d);                              glColorMask(a, b, c, d); LOG_GL_ERROR("glColorMask");
#define glCompileShaderCS(a);                                   glCompileShader(a); LOG_GL_ERROR("glCompileShader");
#define glCompressedTexImage2DCS(a, b, c, d, e, f, g, h);       glCompressedTexImage2D(a, b, c, d, e, f, g, h); LOG_GL_ERROR("glCompressedTexImage2D");
#define glCompressedTexSubImage2DCS(a, b, c, d, e, f, g, h);    glCompressedTexSubImage2D(a, b, c, d, e, f, g, h); LOG_GL_ERROR("glCompressedTexSubImage2D");
#define glCopyTexImage2DCS(a, b, c, d, e, f, g, h);             glCopyTexImage2D(a, b, c, d, e, f, g, h); LOG_GL_ERROR("glCopyTexSubImage2D");
#define glCopyTexSubImage2DCS(a, b, c, d, e, f, g, h);          glCopyTexSubImage2D(a, b, c, d, e, f, g, h); LOG_GL_ERROR("glCopyTexSubImage2D");
#define glCreateProgramCS()                                     glCreateProgram()
#define glCreateShaderCS(a)                                     glCreateShader(a)
#define glCullFaceCS(a);                                        glCullFace(a); LOG_GL_ERROR("glCullFace");
#define glDeleteBuffersCS(a, b);                                glDeleteBuffers(a, b); LOG_GL_ERROR("glDeleteBuffers");
#define glDeleteFramebuffersCS(a, b);                           glDeleteFramebuffers(a, b); LOG_GL_ERROR("glDeleteFramebuffers");
#define glDeleteProgramCS(a);                                   glDeleteProgram(a); LOG_GL_ERROR("glDeleteProgram");
#define glDeleteRenderbuffersCS(a, b);                          glDeleteRenderbuffers(a, b); LOG_GL_ERROR("glDeleteRenderbuffers");
#define glDeleteShaderCS(a);                                    glDeleteShader(a); LOG_GL_ERROR("glDeleteShader");
#define glDeleteTexturesCS(a, b);                               glDeleteTextures(a, b); LOG_GL_ERROR("glDeleteTextures");
#define glDeleteVertexArraysCS(a, b);							glDeleteVertexArrays(a, b); LOG_GL_ERROR("glDeleteVertexArrays");
#define glDepthFuncCS(a);                                       glDepthFunc(a); LOG_GL_ERROR("glDepthFunc");
#define glDepthMaskCS(a);                                       glDepthMask(a); LOG_GL_ERROR("glDepthMask");
#define glDepthRangefCS(a, b);                                  glDepthRangef(a, b); LOG_GL_ERROR("glDepthRangef");
#define glDetachShaderCS(a, b);                                 glDetachShader(a, b); LOG_GL_ERROR("glDetachShader");
#define glDisableCS(a);                                         glDisable(a); LOG_GL_ERROR("glDisable");
#define glDisableVertexAttribArrayCS(a);                        glDisableVertexAttribArray(a); LOG_GL_ERROR("glDisableVertexAttribArray");
#define glDrawArraysCS(a, b, c);                                glDrawArrays(a, b, c); __glDrawCall++; LOG_GL_ERROR("glDrawArrays");
#define glDrawElementsCS(a, b, c, d);                           glDrawElements(a, b, c, d); __glDrawCall++; LOG_GL_ERROR("glDrawElements");
#define glEnableCS(a);                                          glEnable(a); LOG_GL_ERROR("glEnable");
#define glEnableVertexAttribArrayCS(a);                         glEnableVertexAttribArray(a); LOG_GL_ERROR("glEnableVertexAttribArray");
#define glFinishCS();                                           glFinish(); LOG_GL_ERROR("glFinish");
#define glFlushCS();                                            glFlush(); LOG_GL_ERROR("glFlush");
#define glFramebufferRenderbufferCS(a, b, c, d);                glFramebufferRenderbuffer(a, b, c, d); LOG_GL_ERROR("glFramebufferRenderbuffer");
#define glFramebufferTexture2DCS(a, b, c, d, e);                glFramebufferTexture2D(a, b, c, d, e); LOG_GL_ERROR("glFramebufferTexture2D");
#define glFrontFaceCS(a);                                       glFrontFace(a); LOG_GL_ERROR("glFrontFace");
#define glGenBuffersCS(a, b);                                   glGenBuffers(a, b); LOG_GL_ERROR("glGenBuffers");
#define glGenFramebuffersCS(a, b);                              glGenFramebuffers(a, b); LOG_GL_ERROR("glStencilFunc");
#define glGenRenderbuffersCS(a, b);                             glGenRenderbuffers(a, b); LOG_GL_ERROR("glGenRenderbuffers");
#define glGenTexturesCS(a, b);                                  glGenTextures(a, b); LOG_GL_ERROR("glGenTextures");
#define glGenerateMipmapCS(a);                                  glGenerateMipmap(a); LOG_GL_ERROR("glGenerateMipmap");
#define glGenVertexArraysCS(a, b);								glGenVertexArrays(a, b); LOG_GL_ERROR("glGenVertexArrays");
#define glGetBooleanvCS(a, b);                                  glGetBooleanv(a, b); LOG_GL_ERROR("glGetBooleanv");
#define glGetFloatvCS(a, b);                                    glGetFloatv(a, b); LOG_GL_ERROR("glGetFloatv");
#define glGetIntegervCS(a, b);                                  glGetIntegerv(a, b); LOG_GL_ERROR("glGetIntegerv");
#define glGetActiveAttribCS(a, b, c, d, e, f, g);               glGetActiveAttrib(a, b, c, d, e, f, g); LOG_GL_ERROR("glGetActiveAttrib");
#define glGetActiveUniformCS(a, b, c, d, e, f, g);              glGetActiveUniform(a, b, c, d, e, f, g); LOG_GL_ERROR("glGetActiveUniform");
#define glGetAttachedShadersCS(a, b, c, d);                     glGetAttachedShaders(a, b, c, d); LOG_GL_ERROR("glGetAttachedShaders");
#define glGetAttribLocationCS(a, b)                             glGetAttribLocation(a, b)
#define glGetBufferParameterivCS(a, b, c);                      glGetBufferParameterivCS(a, b, c); LOG_GL_ERROR("glGetBufferParameteriv");
#define glGetFramebufferAttachmentParameterivCS(a, b, c, d);    glGetFramebufferAttachmentParameteriv(a, b, c, d); LOG_GL_ERROR("glGetFramebufferAttachmentParameteriv");
#define glGetProgramInfoLogCS(a, b, c, d);                      glGetProgramInfoLog(a, b, c, d); LOG_GL_ERROR("glGetProgramInfoLog");
#define glGetProgramivCS(a, b, c);                              glGetProgramiv(a, b, c); LOG_GL_ERROR("glGetProgramiv");
#define glGetRenderbufferParameterivCS(a, b, c);                glGetRenderbufferParameteriv(a, b, c); LOG_GL_ERROR("glGetRenderbufferParameteriv");
#define glGetShaderInfoLogCS(a, b, c, d);                       glGetShaderInfoLog(a, b, c, d); LOG_GL_ERROR("glGetShaderInfoLog");
#define glGetShaderPrecisionFormatCS(a, b, c, d);               glGetShaderPrecisionFormat(a, b, c, d); LOG_GL_ERROR("glGetShaderPrecisionFormat");
#define glGetShaderSourceCS(a, b, c, d);                        glGetShaderSource(a, b, c, d); LOG_GL_ERROR("glGetShaderSource");
#define glGetShaderivCS(a, b, c);                               glGetShaderiv(a, b, c); LOG_GL_ERROR("glGetShaderiv");
#define glGetStringCS(a)                                        glGetString(a)
#define glGetTexParamterfvCS(a, b, c);                          glGetTexParamterfv(a, b, c); LOG_GL_ERROR("glGetTexParamterfv");
#define glGetTexParamterivCS(a, b, c);                          glGetTexParamteriv(a, b, c); LOG_GL_ERROR("glGetTexParamteriv");
#define glGetUniformfvCS(a, b, c);                              glGetUniformfv(a, b, c); LOG_GL_ERROR("glGetUniformfv");
#define glGetUniformivCS(a, b, c);                              glGetUniformiv(a, b, c); LOG_GL_ERROR("glGetUniformiv");
#define glGetUniformLocationCS(a, b)                            glGetUniformLocation(a, b)
#define glGetVertexAttribfvCS(a, b, c);                         glGetVertexAttribfv(a, b, c); LOG_GL_ERROR("glGetVertexAttribfv");
#define glGetVertexAttribivCS(a, b, c);                         glGetVertexAttribiv(a, b, c); LOG_GL_ERROR("glGetVertexAttribiv");
#define glGetVertexAttribPointervCS(a, b, c);                   glGetVertexAttribPointerv(a, b, c); LOG_GL_ERROR("glGetVertexAttribPointerv");
#define glHintCS(a, b);                                         glHint(a, b); LOG_GL_ERROR("glHint");
#define glIsBufferCS(a)                                         glIsBuffer(a)
#define glIsEnabledCS(a)                                        glIsEnabled(a)
#define glIsFramebufferCS(a)                                    glIsFramebuffer(a)
#define glIsProgramCS(a)                                        glIsProgram(a)
#define glIsRenderbufferCS(a)                                   glIsRenderbuffer(a)
#define glIsShaderCS(a)                                         glIsShader(a)
#define glIsTextureCS(a)                                        glIsTexture(a)
#define glLineWidthCS(a);                                       glLineWidth(a); LOG_GL_ERROR("glLineWidth");
#define glLinkProgramCS(a);                                     glLinkProgram(a); LOG_GL_ERROR("glLinkProgram");
#define glPixelStoreiCS(a, b);                                  glPixelStorei(a, b); LOG_GL_ERROR("glPixelStorei");
#define glPolygonOffsetCS(a, b);                                glPolygonOffset(a, b); LOG_GL_ERROR("glPolygonOffset");
#define glReadPixelsCS(a, b, c, d, e, f, g);                    glReadPixels(a, b, c, d, e, f, g); LOG_GL_ERROR("glReadPixels");
#define glReleaseShaderCompilerCS();                            glReleaseShaderCompiler(); LOG_GL_ERROR("glReleaseShaderCompiler");
#define glRenderbufferStorageCS(a, b, c, d);                    glRenderbufferStorage(a, b, c, d); LOG_GL_ERROR("glRenderbufferStorage");
#define glSampleCoverageCS(a, b);                               glSampleCoverage(a, b); LOG_GL_ERROR("glSampleCoverage");
#define glScissorCS(a, b, c, d);                                glScissor(a, b, c, d); LOG_GL_ERROR("glScissor");
#define glShaderBinaryCS(a, b, c, d, e);                        glShaderBinary(a, b, c, d, e); LOG_GL_ERROR("glShaderBinary");
#define glShaderSourceCS(a, b, c, d);                           glShaderSource(a, b, c, d); LOG_GL_ERROR("glShaderSource");
#define glStencilFuncCS(a, b, c);                               glStencilFunc(a, b, c); LOG_GL_ERROR("glStencilFunc");
#define glStencilFuncSeperateCS(a, b, c, d);                    glStencilFuncSeperate(a, b, c, d); LOG_GL_ERROR("glStencilFuncSeperate");
#define glStencilMaskCS(a);                                     glStencilMask(a); LOG_GL_ERROR("glStencilMask");
#define glStencilMaskSeperateCS(a, b);                          glStencilMaskSeperate(a, b); LOG_GL_ERROR("glStencilMaskSeperate");
#define glStencilOpCS(a, b, c);                                 glStencilOp(a, b, c); LOG_GL_ERROR("glStencilOp");
#define glStencilOpSeperateCS(a, b, c, d);                      glStencilOpSeperate(a, b, c, d); LOG_GL_ERROR("glStencilOpSeperate");
#define glTexImage2DCS(a, b, c, d, e, f, g, h, i);              glTexImage2D(a, b, c, d, e, f, g, h, i); LOG_GL_ERROR("glTexImage2D");
#define glTexSubImage2DCS(a, b, c, d, e, f, g, h, i);           glTexSubImage2D(a, b, c, d, e, f, g, h, i); LOG_GL_ERROR("glTexSubImage2D");
#define glTexParameteriCS(a, b, c);                             glTexParameteri(a, b, c); LOG_GL_ERROR("glTexParameteri");
#define glUniform1fCS(a, b);                                    glUniform1f(a, b); LOG_GL_ERROR("glUniform1f");
#define glUniform2fCS(a, b, c);                                 glUniform2f(a, b, c); LOG_GL_ERROR("glUniform2f");
#define glUniform3fCS(a, b, c, d);                              glUniform3f(a, b, c, d); LOG_GL_ERROR("glUniform3f");
#define glUniform4fCS(a, b, c, d, e);                           glUniform4f(a, b, c, d, e); LOG_GL_ERROR("glUniform4f");
#define glUniform1iCS(a, b);                                    glUniform1i(a, b); LOG_GL_ERROR("glUniform1i");
#define glUniform2iCS(a, b, c);                                 glUniform2i(a, b, c); LOG_GL_ERROR("glUniform2i");
#define glUniform3iCS(a, b, c, d);                              glUniform3i(a, b, c, d); LOG_GL_ERROR("glUniform3i");
#define glUniform4iCS(a, b, c, d, e);                           glUniform4i(a, b, c, d, e); LOG_GL_ERROR("glUniform4i");
#define glUniform1fvCS(a, b, c);                                glUniform1fv(a, b, c); LOG_GL_ERROR("glUniform1fv");
#define glUniform2fvCS(a, b, c);                                glUniform2fv(a, b, c); LOG_GL_ERROR("glUniform2fv");
#define glUniform3fvCS(a, b, c);                                glUniform3fv(a, b, c); LOG_GL_ERROR("glUniform3fv");
#define glUniform4fvCS(a, b, c);                                glUniform4fv(a, b, c); LOG_GL_ERROR("glUniform4fv");
#define glUniform1ivCS(a, b, c);                                glUniform1iv(a, b); LOG_GL_ERROR("glUniform1iv");
#define glUniform2ivCS(a, b, c);                                glUniform2iv(a, b, c); LOG_GL_ERROR("glUniform2iv");
#define glUniform3ivCS(a, b, c);                                glUniform3iv(a, b, c, d); LOG_GL_ERROR("glUniform3iv");
#define glUniform4ivCS(a, b, c);                                glUniform4iv(a, b, c, d, e); LOG_GL_ERROR("glUniform4iv");
#define glUniformMatrix2fvCS(a, b, c, d);                       glUniformMatrix2fv(a, b, c, d); LOG_GL_ERROR("glUniformMatrix2fv");
#define glUniformMatrix3fvCS(a, b, c, d);                       glUniformMatrix3fv(a, b, c, d); LOG_GL_ERROR("glUniformMatrix3fv");
#define glUniformMatrix4fvCS(a, b, c, d);                       glUniformMatrix4fv(a, b, c, d); LOG_GL_ERROR("glUniformMatrix4fv");
#define glUseProgramCS(a);                                      glUseProgram(a); LOG_GL_ERROR("glUseProgram");
#define glValidateProgramCS(a);                                 glValidateProgram(a); LOG_GL_ERROR("glValidateProgram");
#define glVertexAttrib1fCS(a, b);                               glVertexAttrib1f(a, b); LOG_GL_ERROR("glVertexAttrib1f");
#define glVertexAttrib2fCS(a, b, c);                            glVertexAttrib2f(a, b, c); LOG_GL_ERROR("glVertexAttrib2f");
#define glVertexAttrib3fCS(a, b, c, d);                         glVertexAttrib3f(a, b, c, d); LOG_GL_ERROR("glVertexAttrib3f");
#define glVertexAttrib4fCS(a, b, c, d, e);                      glVertexAttrib4f(a, b, c, d, e); LOG_GL_ERROR("glVertexAttrib4f");
#define glVertexAttribPointerCS(a, b, c, d, e, f);              glVertexAttribPointer(a, b, c, d, e, f); LOG_GL_ERROR("glVertexAttribPointer");

#define glViewportCS(a, b, c, d);                               glViewport(a, b, c, d); LOG_GL_ERROR("glViewport");

bool glIsSupportedExtensionCS(const char* ext);

#endif

