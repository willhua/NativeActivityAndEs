//
// Created by lisan on 2018/6/16.
//
#include <android/native_window.h>
#include "EsTest.h"
#include "ShaderUtils.h"
#include <malloc.h>
//#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>





int es_width;
int es_height;

EGLDisplay eglDisplay;
EGLSurface eglSurface;



std::vector<uint8_t> getRandomRGBA(const int width, const int height){
    std::vector<uint8_t > res(width * height * 4);
    for(int i = 0; i < width * height; ++i){
        res[i * 4] = i & 0xFF;
        res[i * 4] = (i >> 8)& 0xFF;
        res[i * 4 + 2] = (i >> 16)& 0xFF;
        res[i * 4 + 3] = i & 0xFF;
    }
    return res;
}




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


/*******************compute shader*****************************/



GLuint createComputeProgram(const char* pComputeSource) {
    GLuint computeShader = loadShader(GL_COMPUTE_SHADER, pComputeSource);
    if (!computeShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, computeShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    ALOG("Could not link program: %s", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void yuvToRgb()
{
    //   1  |   2
    //   3  |   4
    const char src[] = {
            "#version 320 es\n"
                    "layout(local_size_x = 1, local_size_y = 1) in;\n"
                    "layout(binding = 0) readonly buffer DataY {\n"
                    "   uint data[]; } datay;\n"
                    "layout(binding = 1) readonly buffer DataU {\n"
                    "   uint data[]; } datau;\n"
                    "layout(binding = 2) readonly buffer DataV {\n"
                    "   uint data[]; } datav;\n"
                    "layout(binding = 3)  buffer DataRgba{ \n"
                    "   uint data[]; } datargba; \n"
                    "uniform uint width; \n"     //这里的宽高都是以u图的尺寸为标准，同时由于一个int表示了4个u，所以图片的实际宽度为width*4*2
                    "uniform uint height; \n"    //图片的实际高度为height*2
                    " uint four = uint(4); \n"
                    " uint two =  uint(2); \n"
                    " uint one =  uint(1); \n"
                    " \n"
                    "void main() { \n"
                    "   uint index_uv = gl_GlobalInvocationID.y * width + gl_GlobalInvocationID.x; \n"
                    "   vec4 u = unpackUnorm4x8(datau.data[index_uv]); \n"
                    "   vec4 v = unpackUnorm4x8(datav.data[index_uv]); \n"
                    "   uint index_y1 = gl_GlobalInvocationID.y * width * four + gl_GlobalInvocationID.x * two; \n"
                    "   vec4 y1 = unpackUnorm4x8(datay.data[index_y1]); \n"
                    "   uint index_y2 = gl_GlobalInvocationID.y * width * four + gl_GlobalInvocationID.x * two + one; \n"
                    "   vec4 y2 = unpackUnorm4x8(datay.data[index_y2]); \n"
                    "   uint index_y3 = gl_GlobalInvocationID.y * width * four + two * width + gl_GlobalInvocationID.x * two; \n"
                    "   vec4 y3 = unpackUnorm4x8(datay.data[index_y3]); \n"
                    "   uint index_y4 = gl_GlobalInvocationID.y * width * four + two * width + gl_GlobalInvocationID.x * two + one; \n"
                    "   vec4 y4 = unpackUnorm4x8(datay.data[index_y4]); \n"
                    "   vec4 rgba[16]; \n"
                    "   vec4 ys[4]; \n"
                    "   ys[0] =y1; \n"
                    "   ys[1] =y2; \n"
                    "   ys[2] =y3; \n"
                    "   ys[3] =y4; \n"
                    "   for(int i = 0; i < 8; ++i){ \n"
                    "       rgba[i].x = ys[i / 4][i % 4] + 1.402 * (v[i / 2] - 0.5); \n"
                    "       rgba[i].y = ys[i / 4][i % 4] - 0.34414 * (u[i / 2] - 0.5) - 0.71414 * (v[i / 2] - 0.5); \n"
                    "       rgba[i].z = ys[i / 4][i % 4] + 1.772 * (u[i / 2] - 0.5); \n"
                    "       rgba[i].w = 1.0; \n"    //第一行处理结束
                    "       rgba[i+8].x = ys[i / 4+2][i % 4] + 1.402 * (v[i / 2] - 0.5); \n"
                    "       rgba[i+8].y = ys[i / 4+2][i % 4] - 0.34414 * (u[i / 2] - 0.5) - 0.71414 * (v[i / 2] - 0.5); \n"
                    "       rgba[i+8].z = ys[i / 4+2][i % 4] + 1.772 * (u[i / 2] - 0.5); \n"
                    "       rgba[i+8].w = 1.0; \n"      //第二行处理结束
                    "   }\n"
                    "   for(int i = 0; i < 8; ++i){ \n" //处理第一行
                    "       datargba.data[gl_GlobalInvocationID.y* two * width*four*two + gl_GlobalInvocationID.x * four * two + uint(i)] = packUnorm4x8(rgba[i]); \n"
                    "       datargba.data[(gl_GlobalInvocationID.y* two + one) * width*four*two +  gl_GlobalInvocationID.x * four * two + uint(i)] = packUnorm4x8(rgba[i+8]); \n"
                    "   } \n"
                    "}"
    };


    const char srcv[] = {
            "#version 320 es\n"
                    "layout(binding = 3)  buffer DataRgba{ \n"
                    "   uint data[]; } datargba; \n"
                    "layout(location = 0) in vec4 pos;\n"
                    "uniform int width;\n"
                    "uniform int height;\n"
                    "layout(location = 1) in vec2 posrgba; \n"
                    "out vec4 color; \n"
                    "void main(){\n"
                    "   gl_Position = pos;\n"
                    "   color = unpackUnorm4x8(datargba.data[uint(posrgba.y * float(height) * float(width) + float(width) * posrgba.x)]);\n"
                    "}"
    };
    const char srcf[] = {
            "#version 320 es\n"
                    "precision mediump float;"
                    "in vec4 color;\n"
                    "out vec4 fcolor;\n"
                    "void main(){fcolor = color;}"
    };


    esprogram1 = createComputeProgram(src);
    glUseProgram(esprogram1);
    CHECK();
    GLint x,y,z,invo;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &z);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,  &invo);
    ALOG("max work group %d %d %d %d", x,y,z, invo);
    CHECK();
//    return;

    const int img_width = 32*32;
    const int img_height = 32*32;
    std::vector<uint8_t > yuv_vec(img_width * img_height * 3/ 2 );
    for(int i = 0; i < yuv_vec.size(); ++i){
        yuv_vec[i] = 255;
    }
    uint8_t *yuv=yuv_vec.data();
//    cv::Mat img = cv::imread("/sdcard/in.jpg", 1);
//    cv::resize(img, img, cv::Size(32 * 32, 32 * 32 ));
//    cv::Mat yuv;
//    cv::cvtColor(img, yuv, CV_BGR2YUV_I420);
//    cv::imwrite("/sdcard/yuv.bmp", yuv);
//    ALOG("%d   yuv size w*h=%d*%d", yuv.isContinuous(), yuv.cols, yuv.rows);
    int width = img_width;
    int height = img_height;
    uint8_t *ptry = yuv;
    uint8_t *ptru = ptry + width * height;
    uint8_t *ptrv = ptru + width * height / 4;
    width = width / 2 / 4;
    height = height / 2 / 4;
    int length = img_width * img_height;
    CHECK();
    GLint locationw = glGetUniformLocation(esprogram1, "width");
    //GLint locationh = glGetUniformLocation(program, "height");
    ALOG("uniform location %d ", locationw );
    glUniform1ui(locationw, GLuint( width));
    //glUniform1i(locationh, height);
    CHECK();
    GLuint ssbos[4];
    glGenBuffers(4, ssbos);
    ALOG("bing data start");
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, length, ptry, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[0]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, length / 4, ptru, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[1]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, length / 4, ptrv, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbos[2]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[3]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint) * length, NULL, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbos[3]);
    ALOG("bind end ssbo");
    CHECK();
    glUseProgram(esprogram1);
    glDispatchCompute(32 * 32, 32*32 , 1);
    ALOG("dispatch compute");
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    CHECK();



/*
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[3]);
    ALOG("get ptr 0");
    uchar *ptrrgba = (uchar*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint) * length, GL_MAP_READ_BIT);
    ALOG("get ptr");
    cv::Mat result(img.rows, img.cols, CV_8UC4, ptrrgba);
    cv::cvtColor(result, result, CV_RGBA2BGR);
    ALOG("result r*c:%d %d", result.rows, result.cols);
    cv::imwrite("/sdcard/out.jpg", result);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
*/
    //glDeleteProgram(program);
    CHECK();
    ALOG("cs end");


    /*******show result********/
    esprogram1 = glCreateProgram();
    GLuint vshader = loadShader(GL_VERTEX_SHADER, srcv);
    GLuint fshader = loadShader(GL_FRAGMENT_SHADER, srcf);
    glAttachShader(esprogram1, vshader);
    glAttachShader(esprogram1, fshader);
    glLinkProgram(esprogram1);
    glUseProgram(esprogram1);
    CHECK();

    GLfloat vertices3[] = {
            -1.0,   0.5,    0.0,
            -1.0,   -0.5,    0.0,
            1.0,   -0.5,   0.0,
            1.0,    0.5,   0.0
    };

    GLfloat texture3[] = {
            0.0,    0.0,
            0.0,    1.0,
            1.0,    1.0,
            1.0,    0.0
    };

    GLushort indices3[] = {
            0,1,2,0,2,3
    };

    GLint lw = glGetUniformLocation(esprogram1, "width");
    GLint lh = glGetUniformLocation(esprogram1, "height");
    GLint ws = 32*32;
    GLint hs = 32*32;
    glUniform1i(lw, ws);
    glUniform1i(lh, hs);
    ALOG(" lw lh %d %d", lw, lh);
    CHECK();

    glViewport(0, 0, es_width, es_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices3);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texture3);
    glEnableVertexAttribArray(1);
    CHECK();
    ALOG("end all");
    CHECK();
   // glDrawElements(GL_TRIANGLE_STRIP, 6, GL_FLOAT, indices3);
}
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
    ALOG("native width width=%d height=%d", es_width, es_height);

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
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    glUseProgram(program);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    std::vector<uint8_t > ranrgba = getRandomRGBA(es_width, es_height);

    int width1, height1, nrChannels1;
    unsigned char *data1 = stbi_load("/sdcard/1.jpg", &width1, &height1, &nrChannels1, 0);
//    width1 = es_width;
//    height1 = es_height;
//    data1 = ranrgba.data();
    ALOG("es1, %dx%d", width1, height1);

    GLuint texture[2];
    glGenTextures(2, texture);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    ALOG("TEXTURE: %d %d", texture[0], texture[1]);

    glActiveTexture(GL_TEXTURE0);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    glBindTexture(GL_TEXTURE_2D, texture[0]);   //注意是GL_TEXTURE_2D或者其他类型，而不是GL_TEXTURE
    //glTexImage2D specifies the two-dimensional texture for the current texture unit, specified with glActiveTexture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint loc1 = glGetUniformLocation(program, "sample0");
    ALOG("loc1:%d", loc1);
    glUniform1i(loc1, 0);


    int width2 = 0, height2 = 0, nrChannels2 = 0;
    unsigned char *data2 = stbi_load("/sdcard/2.jpg", &width2, &height2, &nrChannels2, 0);
    ALOG("es2, %dx%d", width2, height2);
//    width2 = es_width;
//    height2 = es_height;
//    data2 = ranrgba.data();

    glActiveTexture(GL_TEXTURE1);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    //绑定图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());

    //设置对图像缩小或者放大的差值方式，比如产生mip图的时候
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint loc2 = glGetUniformLocation(program, "sample1");
    ALOG("loc2:%d", loc2);
    glUniform1i(loc2, 1);   //在使用这个之前，一定要保证program已经被glUseProgram
    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());
    return program;
}


void cstest()
{
    const char src[] = {
            "#version 320 es\n"
            "layout(layout_size_x = 16) in;\n"
            "uniform int width;\n"
            "uniform int height;\n"
            "uniform layout(binding=0, rgba8ui) uimage2D inimagedata;"
            "uniform layout(binding=0, rgba8ui) writeonly uimage2D outimagedata;"
            "void main(){\n"
            "   vec2 pos(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);\n"
            "   int left = clamp(pos.x - 5, 0, width - 1);\n"
            "   int right = clamp(pos.x + 5, 0, width - 1);\n"
            "   int top = clamp(pos.y - 5, 0, height - 1);\n"
            "   int bottom = clamp(pos.y + 5, 0, height - 1);\n"
            "   vec4 sum(0,0,0,0);\n"
            "   for(int i = left; i < right; ++i){\n"
            "       for(int j = top; j < bottom; ++j){\n"
            "           sum += imageLoad(inimagedata, ivec2(i, j));\n"
            "       }\n"
            "   }\n"
            "   sum /= ((right - left + 1) * (right - left + 1) + (bottom - top +1) * (bottom - top +1));\n"
            "   imageStore(outimagedata, pos, sum);\n"
            "}"
    };

    const char *source = src;


    GLuint csid = glCreateShader(GL_COMPUTE_SHADER);
    if (csid == 0){
        ALOG("create shader error");
    }
    glShaderSource(csid, 1, &source, NULL);
    glCompileShader(csid);

    GLint compliedok;
    glGetShaderiv(csid, GL_COMPILE_STATUS, &compliedok);
    if (compliedok == GL_FALSE){
        ALOG("erroe load shader");
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, csid);
    glLinkProgram(program);
    glUseProgram(program);


}

void initEsProgram()
{
    if(false) //cs test
    {
        yuvToRgb();
        CHECK();
        return;
    }

    if (true) {
        esprogram1 = textureTest();
        ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());

        glViewport(0, 0, es_width, es_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(esprogram1);
        ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices2);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texture2);
        glEnableVertexAttribArray(1);

        ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());

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
//    ALOG("draw one frame");


    glDrawArrays(GL_TRIANGLES, 0, 3);
//    ALOG("%s %d, err=%d", __func__, __LINE__, glGetError());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices2);
    eglSwapBuffers(eglDisplay, eglSurface); //如果没有这个，draw几次之后就会出现：read: unexpected EOF!错误
//    ALOG("draw one end");

}


