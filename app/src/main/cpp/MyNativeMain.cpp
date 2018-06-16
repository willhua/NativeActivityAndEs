//
// Created by lisan on 2018/6/16.
//


#include <android/log.h>
#include <pthread.h>
#include "MyNativeMain.h"
#include "EsTest.h"
#include "JniUtils.h"


/*
 * android-manifest中：
这里activity的名称是固定的android.app.NativeActivity，meta-data以key-value形式描述该Activity参数，
比如name="android.app.lib_name"和value="ndk-lib"指明了这个NativeActivity所在的本地库的库名为ndk-lib，
另外，该Activity必须被指定为MainActivity且需要自动启动。

*/


//事件处理
static bool isLoop = false;
static pthread_t loopID;

void *looper(void *args) {
    ANativeActivity *activity = (ANativeActivity *) args;
    AInputQueue *queue = (AInputQueue *) activity->instance;
    AInputEvent *event = NULL;
    while (isLoop) {
        if (!AInputQueue_hasEvents(queue))//判断队列中是否有未处理事件
            continue;
        AInputQueue_getEvent(queue, &event);//从队列中获取一个事件
        switch (AInputEvent_getType(event)) {//判断事件类型
            case AINPUT_EVENT_TYPE_MOTION: {//触摸事件类型
                switch (AMotionEvent_getAction(event)) {
                    case AMOTION_EVENT_ACTION_DOWN:{//触摸按下事件
                        float x = AMotionEvent_getX(event, 0);//获得x坐标
                        float y = AMotionEvent_getY(event, 0);//获得y坐标
                        ALOG("X:%f,Y:%f", x, y);//输出坐标
                        break;
                    }
                    case AMOTION_EVENT_ACTION_UP:{//触摸抬起事件
                        break;
                    }
                }
                break;
            }
            case AINPUT_EVENT_TYPE_KEY: {//按键事件类型
                switch (AKeyEvent_getAction(event)) {
                    case AKEY_EVENT_ACTION_DOWN: {//按键按下事件
                        switch (AKeyEvent_getKeyCode(event)) {
                            case AKEYCODE_BACK: {//返回键
                                ANativeActivity_finish(activity);//退出Activity
                                break;
                            }
                        }
                        break;
                    }
                    case AKEY_EVENT_ACTION_UP: {//按键抬起事件
                        break;
                    }
                }
            }
        }
        AInputQueue_finishEvent(queue, event, 1);//将事件从事件队列中移除
    }
    return args;
}



//绑定生命周期函数到activity
void bindLifeCycle(ANativeActivity *activity) {
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
}

//事件队列创建完成, 开启事件线程
void onInputQueueCreated(ANativeActivity *activity, AInputQueue *queue) {
    ALOG("onInputQueueCreated");
    isLoop = true;
    activity->instance = (void *) queue;
    pthread_create(&loopID, NULL, looper, activity);
}

//事件队列销毁前 退出线程循环
void onInputQueueDestroyed(ANativeActivity *activity, AInputQueue *queue) {
    ALOG("onInputQueueDestroyed");
    isLoop = false;
}


//相当于Java中的onCreate()方法,就是程序的入口点
void ANativeActivity_onCreate(ANativeActivity *activity, void *savedState, size_t savedStateSize) {
    ALOG("native on create");
    bindLifeCycle(activity);
}


void onStart(ANativeActivity *activity) {

}

void onResume(ANativeActivity *activity) {

}

void *onSaveInstanceState(ANativeActivity *activity, size_t *outSize) {

}

void onPause(ANativeActivity *activity) {

}

void onStop(ANativeActivity *activity) {

}

void onDestroy(ANativeActivity *activity) {

}

void onWindowFocusChanged(ANativeActivity *activity, int hasFocus) {

}

static bool isShow;
static pthread_t triangleID;

void * esView(void *args) {
    createEglWindow((ANativeWindow*)args);
    initEsProgram();
    while (isShow)
    {
        drawOneFrame();
    }
    destroyEsProgram();
    return args;
}

//在这里开始创建ES环境
void onNativeWindowCreated(ANativeActivity *activity, ANativeWindow *window) {
    isShow = true;
    int ok = pthread_create(&triangleID, NULL, esView, window);
    if (0 != ok){
        ALOG("create es thread fail");
    }
}

void onNativeWindowDestroyed(ANativeActivity *activity, ANativeWindow *window) {
    isShow = false;
}


void onConfigurationChanged(ANativeActivity *activity) {

}

void onLowMemory(ANativeActivity *activity) {

}


