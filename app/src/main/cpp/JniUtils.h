//
// Created by lisan on 2018/6/16.
//

#ifndef NATIVEACTIVITYANDES_JNIUTILS_H
#define NATIVEACTIVITYANDES_JNIUTILS_H

#include <android/log.h>
#include <jni.h>
//#include <thread>
#include "stb/stb_image.h"


#define ALOG(...) __android_log_print(ANDROID_LOG_DEBUG, "willhua", __VA_ARGS__)

#define CHECK() \
{\
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) \
    {\
       ALOG("%s %d glGetError returns %d\n", __func__, __LINE__, err); \
    }\
    else \
    {\
        ; \
    }\
}

#endif //NATIVEACTIVITYANDES_JNIUTILS_H
