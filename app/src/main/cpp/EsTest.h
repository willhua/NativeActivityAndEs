//
// Created by lisan on 2018/6/16.
//

#ifndef NATIVEACTIVITYANDES_ESTEST_H
#define NATIVEACTIVITYANDES_ESTEST_H

#include "JniUtils.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>

#include <stdint.h>




void createEglWindow(EGLNativeWindowType window);
void initEsProgram();
void destroyEsProgram();
void drawOneFrame();




#endif //NATIVEACTIVITYANDES_ESTEST_H
