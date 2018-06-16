//
// Created by lisan on 2018/6/16.
//


#include "ShaderUtils.h"
#include "JniUtils.h"

GLuint loadShader(GLuint type, const char *src)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0){
        ALOG("create shader error");
    }
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint compliedok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compliedok);
    if (compliedok == GL_FALSE){
        ALOG("erroe load shader");
    }
    return shader;
}

