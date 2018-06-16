//
// Created by lisan on 2018/6/16.
//
#include <android/native_window.h>
#include "EsTest.h"
#include "ShaderUtils.h"

int es_width;
int es_height;

GLuint vshader1;
GLuint fshader1;
GLuint esprogram1;


const char vShaderSrc1[] = {
        "#version 300 es                        \n"
        "layout(location = 0) in vec4 vpos;     \n"
        "layout(location = 1) in vec4 vcolor;   \n"
        "out vec4 color1;                       \n"
        "void main()                            \n"
        "{                                      \n"
        "   gl_Position = vpos;                 \n"
        "   color = vcolor;                     \n"
        "}                                      \n"
};

const char fShaderSrc1[] = {
        "#version 300 es                        \n"
        "precision mediump float                \n"
        "in  vec4 color1;                       \n"
        "out vec4 fcolor1;                      \n"
        "void main() {                          \n"
        "   fcolor1 = color1;                   \n"
        "}                                      \n"
};

GLfloat vertices1[] = {
        0.0f,   0.5f,   0.0f,
        -0.5f,  0.5f,   0.0f,
        0.5f,   0.5f,   0.0f
};



void createEglWindow(ANativeWindow *window)
{
    const EGLint configAttributes[] = {
            EGL_RENDERABLE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    es_width = ANativeWindow_getWidth(window);
    es_height = ANativeWindow_getHeight(window);

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        ALOG("get egl displat erroe");
    }

    EGLint majorversion, minorversion;
    if(!eglInitialize(display, &majorversion, &minorversion)){
        ALOG("egl initialize fail");
    }

    EGLConfig config;
    EGLint numConfigs;
    if(!eglChooseConfig(display, configAttributes, &config, 1, &numConfigs)){
        ALOG("egl choose config fail");
    }

    EGLSurface  surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE){
        ALOG("egl no surface");
    }

    const EGLint contextAttributs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    EGLContext eglContext = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributs);
    if (eglContext == EGL_NO_CONTEXT){
        ALOG("egl no context");
    }


    if(!eglMakeCurrent(display, surface, surface, eglContext)){
        ALOG("egl make current fail");
    }
}

void initEsProgram()
{
    vshader1 = loadShader(GL_VERTEX_SHADER, vShaderSrc1);
    fshader1 = loadShader(GL_FRAGMENT_SHADER, fShaderSrc1);

    esprogram1 = glCreateProgram();
    glAttachShader(esprogram1, vshader1);
    glAttachShader(esprogram1, fshader1);

    glLinkProgram(esprogram1);
}


void destroyEsProgram()
{
    glDeleteProgram(esprogram1);
}

void drawOneFrame()
{
    ALOG("draw one frame");
    glViewport(0, 0, es_width, es_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(esprogram1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


