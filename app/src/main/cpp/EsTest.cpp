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


/*****************三角形************************/


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
        "   color1 = vcolor;                    \n"
        "}                                      \n"
};

const char fShaderSrc1[] = {
        "#version 300 es                        \n"
        "precision mediump float;               \n"
        "in  vec4 color1;                       \n"
        "out vec4 fcolor1;                      \n"
        "void main() {                          \n"
        "   fcolor1 = color1;                   \n"
        "}                                      \n"
};

GLfloat vertices1[] = {
        0.0f,   0.5f,   0.0f,
        -0.5f,  -0.5f,   0.0f,
        0.5f,   -0.5f,   0.0f
};

GLfloat vcolors1[] = {
        1.0,    0.0,    0.0,    1.0,
        0.0,    1.0,    0.0,    1.0,
        0.0,    0.0,    1.0,    1.0
};

/***************纹理********************************/
const char vShader2[] = {
        "#version 300 es                        \n"
        "out vec2 vcoord;                       \n"
        "layout(location = 0) in vec4 vpos;     \n"
        "layout(location = 1) in vec2 text_pos; \n"
        "void main(){                           \n"
        "   gl_Position = vpos;                 \n"
        "   vcoord = text_pos;                  \n"
        "}                                      \n"
};

const char fShader2[] = {

                "#version 300 es                                     \n"
                "precision mediump float;                            \n"
                "in vec2 vcoord;                                 \n"
                "layout(location = 0) out vec4 outColor;             \n"
                "uniform sampler2D sample0;                        \n"
                "uniform sampler2D sample1;                       \n"
                "void main()                                         \n"
                "{                                                   \n"
                "  vec4 baseColor;                                   \n"
                "  vec4 lightColor;                                  \n"
                "                                                    \n"
                "  baseColor = texture( sample0, vcoord );     \n"
                "  lightColor = texture( sample1, vcoord );   \n"
                "  outColor = baseColor * (lightColor  + 0.5);       \n"
                "}       "
};

GLfloat vertices2[] = {
        -1.0,   0.5,    0.0,
        -1.0,   -0.5,    0.0,
        1.0,   -0.5,   0.0,
        1.0,    0.5,   0.0
};

GLfloat texture2[] = {
        0.0,    0.0,
        0.0,    1.0,
        1.0,    1.0,
        1.0,    0.0
};

GLushort indices2[] = {
        0,1,2,0,2,3
};


/**************************************************/

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


GLuint textureTest()
{
    GLuint program = loadProgram(vShader2, fShader2);
    glUseProgram(program);

    int width1, height1, nrChannels1;
    unsigned char *data1 = stbi_load("/sdcard/es1.jpg", &width1, &height1, &nrChannels1, 0);
    ALOG("es1, %dx%d", width1, height1);

    GLuint texture[2];
    glGenTextures(2, texture);
    ALOG("TEXTURE: %d %d", texture[0], texture[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   //注意是GL_TEXTURE_2D或者其他类型，而不是GL_TEXTURE
    //glTexImage2D specifies the two-dimensional texture for the current texture unit, specified with glActiveTexture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint loc1 = glGetUniformLocation(program, "sample0");
    ALOG("loc1:%d", loc1);
    glUniform1i(loc1, 0);
    free(data1);


    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load("/sdcard/es2.png", &width2, &height2, &nrChannels2, 0);
    ALOG("es2, %dx%d", width2, height2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    //绑定图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    //设置对图像缩小或者放大的差值方式，比如产生mip图的时候
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint loc2 = glGetUniformLocation(program, "sample1");
    ALOG("loc2:%d", loc2);
    glUniform1i(loc2, 1);   //在使用这个之前，一定要保证program已经被glUseProgram
    free(data2);
    return program;
}

void initEsProgram()
{
    if (true) {
        esprogram1 = textureTest();

        glViewport(0, 0, es_width, es_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(esprogram1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices2);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texture2);
        glEnableVertexAttribArray(1);
    } else {

        esprogram1 = loadProgram(vShaderSrc1, fShaderSrc1);

        glViewport(0, 0, es_width, es_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(esprogram1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, vcolors1);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

}


void destroyEsProgram()
{
    glDeleteProgram(esprogram1);
}

void drawOneFrame()
{
    //ALOG("draw one frame");


    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices2);
    eglSwapBuffers(eglDisplay, eglSurface); //如果没有这个，draw几次之后就会出现：read: unexpected EOF!错误
    //ALOG("draw one end");

}


