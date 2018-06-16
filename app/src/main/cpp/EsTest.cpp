//
// Created by lisan on 2018/6/16.
//
#include <android/native_window.h>
#include "EsTest.h"
#include "ShaderUtils.h"
#include <malloc.h>

int es_width;
int es_height;

EGLDisplay eglDisplay;
EGLSurface eglSurface;

GLuint vshader1;
GLuint fshader1;
GLuint esprogram1;


const char vShaderSrc1[] = {
        "#version 300 es                        \n"
        "layout(location = 0) in vec4 vpos;     \n"
        "//layout(location = 1) in vec4 vcolor;   \n"
        "//out vec4 color1;                       \n"
        "void main()                            \n"
        "{                                      \n"
        "   gl_Position = vpos;                 \n"
        "   //color = vcolor;                     \n"
        "}                                      \n"
};

const char fShaderSrc1[] = {
        "#version 300 es                        \n"
        "precision mediump float;                \n"
        "//in  vec4 color1;                       \n"
        "out vec4 fcolor1;                      \n"
        "void main() {                          \n"
        "   fcolor1 = vec4(1.0, 0.0, 0.0, 1.0);                   \n"
        "}                                      \n"
};

GLfloat vertices1[] = {
        0.0f,   0.5f,   0.0f,
        -0.5f,  -0.5f,   0.0f,
        0.5f,   -0.5f,   0.0f
};



void createEglWindow(ANativeWindow *window)
{
    const EGLint configAttributes[] = {
            EGL_RENDERABLE_TYPE, EGL_WINDOW_BIT,        //默认值为EGL_OPENGL_ES_BIT，经过测试，这两个（EGL_WINDOW_BIT）都可以
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
    eglDisplay = display;
    ALOG("display:%p", &eglDisplay);

    EGLint majorversion, minorversion;
    if(!eglInitialize(display, &majorversion, &minorversion)){
        ALOG("egl initialize fail");
    }

    EGLConfig config;
    EGLint numConfigs;
    if(!eglChooseConfig(display, configAttributes, &config, 1, &numConfigs)){
        ALOG("egl choose config fail");
    }

    /*
    * 在另外一个作者的实现中有这段代码
    EGLint format = 0;
    eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);*/

    EGLSurface  surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE){
        ALOG("egl no surface");
    }
    eglSurface = surface;
    ALOG("eglSurface:%p", &eglSurface);

    const EGLint contextAttributs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributs);
    if (context == EGL_NO_CONTEXT){
        ALOG("egl no context");
    }

    if(!eglMakeCurrent(display, surface, surface, context)){
        ALOG("egl make current fail");
    }
}

void initEsProgram()
{
    ALOG("initEsProgram");
    vshader1 = loadShader(GL_VERTEX_SHADER, vShaderSrc1);
    fshader1 = loadShader(GL_FRAGMENT_SHADER, fShaderSrc1);

    esprogram1 = glCreateProgram();
    if(esprogram1 == 0){
        ALOG("glCreateProgram error");
    }
    glAttachShader(esprogram1, vshader1);
    glAttachShader(esprogram1, fshader1);

    glLinkProgram(esprogram1);

    GLint linkedok;
    glGetProgramiv(esprogram1, GL_LINK_STATUS, &linkedok);
    if (linkedok == GL_FALSE){
        GLint  infolen = 0;
        glGetProgramiv(esprogram1, GL_INFO_LOG_LENGTH, &infolen);
        ALOG("link erroe:%d  %d",linkedok, infolen);
        if(infolen > 1){
            char *infoLog = (char*)malloc(sizeof(char) * infolen);
            glGetProgramInfoLog(esprogram1, infolen, NULL, infoLog);
            ALOG("link info:%s", infoLog);
            free(infoLog);
        }
        glDeleteProgram(esprogram1);
    }

}


void destroyEsProgram()
{
    glDeleteProgram(esprogram1);
}

void drawOneFrame()
{
    ALOG("draw one frame");

    glViewport(0, 0, es_width, es_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(esprogram1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices1);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    eglSwapBuffers(eglDisplay, eglSurface); //如果没有这个，draw几次之后就会出现：read: unexpected EOF!错误
    ALOG("draw one end");

}


