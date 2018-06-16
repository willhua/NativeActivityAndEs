//
// Created by lisan on 2018/6/16.
//


#include "ShaderUtils.h"

GLuint loadShader(GLuint type, const char *src)
{
    GLuint shader;
    glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

