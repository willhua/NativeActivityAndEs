//
// Created by lisan on 2018/6/16.
//

#ifndef NATIVEACTIVITYANDES_SHADERUTILS_H
#define NATIVEACTIVITYANDES_SHADERUTILS_H

#include <GLES3/gl32.h>

GLuint loadShader(GLuint type, const char *src);

GLuint loadProgram(const char *vshaderSrc, const char *fshaderSrc);

GLuint loadTexture (char *fileName );



#endif //NATIVEACTIVITYANDES_SHADERUTILS_H
