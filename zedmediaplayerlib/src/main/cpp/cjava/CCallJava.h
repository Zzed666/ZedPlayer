//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_CCALLJAVA_H
#define ZEDPLAYER_CCALLJAVA_H

#include <jni.h>
#include <string>
#include "../JniDefine.h"

#define CTHREADTYPE_MAIN 0
#define CTHREADTYPE_CHILD 1

class CCallJava {

public:
    JavaVM *jvm = nullptr;
    JNIEnv *jEnv = nullptr;
    jobject jobj = nullptr;
    jmethodID jloadmid = nullptr;
    jmethodID jpreparemid = nullptr;
    jmethodID jplaytimemid = nullptr;
    jmethodID jpausemid = nullptr;
    jmethodID jstopmid = nullptr;
public:
    CCallJava(JavaVM *vm, JNIEnv *env, jobject obj);
    ~CCallJava();
    void callOnLoad(int cType, bool load);
    void callOnPrepare(int cType);
    void callOnPlayTime(int cType, int total, int current);
    void callOnPause(int cType, bool pause);
    void callOnStop(int cType);
};


#endif //ZEDPLAYER_CCALLJAVA_H
