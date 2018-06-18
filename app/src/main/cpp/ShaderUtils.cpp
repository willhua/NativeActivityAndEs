//
// Created by lisan on 2018/6/16.
//


#include <malloc.h>
#include "ShaderUtils.h"
#include "JniUtils.h"

GLuint loadShader(GLuint type, const char *src)
{
    GLuint shader = glCreateShader(type);   //这里要注意，要把create函数的返回值赋给shader。
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


GLuint loadProgram(const char *vshaderSrc, const char *fshaderSrc)
{
    GLuint vshader = loadShader(GL_VERTEX_SHADER, vshaderSrc);
    GLuint fshader = loadShader(GL_FRAGMENT_SHADER, fshaderSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    GLint linkedok;
    glGetProgramiv(program, GL_LINK_STATUS, &linkedok);
    if (linkedok == GL_FALSE){
        GLint  infolen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolen);
        ALOG("link erroe:%d  %d",linkedok, infolen);
        if(infolen > 1){
            char *infoLog = (char*)malloc(sizeof(char) * infolen);
            glGetProgramInfoLog(program, infolen, NULL, infoLog);
            ALOG("link info:%s", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program);
        program = GL_FALSE;
    }

    glDeleteProgram(vshader);
    glDeleteProgram(fshader);

    return program;
}


GLuint loadTexture (char *fileName )
{
    int width, height, nChannels;
    unsigned char *data = stbi_load(fileName, &width, &height, &nChannels, 0);

    GLuint texId;

    if ( data == NULL )
    {
        ALOG("LoadTexture   stv load %s fail", fileName);
        return 0;
    }

    glGenTextures ( 1, &texId );
    glBindTexture ( GL_TEXTURE_2D, texId );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    free ( data );

    return texId;
}

