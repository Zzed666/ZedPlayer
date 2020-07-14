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
    jmethodID jdbmid = nullptr;
    jmethodID jpausemid = nullptr;
    jmethodID jseekmid = nullptr;
    jmethodID jstopmid = nullptr;
    jmethodID jerrormid = nullptr;
    jmethodID jcompletemid = nullptr;
    jmethodID jpcmtoaacmid = nullptr;
    jmethodID jpcminfomid = nullptr;
    jmethodID jrenderyuvmid = nullptr;
public:
    CCallJava(JavaVM *vm, JNIEnv *env, jobject obj);
    ~CCallJava();
    void callOnLoad(int cType, bool load);
    void callOnPrepare(int cType);
    void callOnPlayTime(int cType, int total, int current);
    void callOnDB(int cType, int db);
    void callOnPause(int cType, bool pause);
    void callOnSeek(int cType, int total, int seek);
    void callOnStop(int cType);
    void callOnError(int cType, int errorCode, char *errorMsg);
    void callOnComplete(int cType);
    void callOnPcmToAAC(int cType, void *buffer, int bufferSize);
    void callOnPcmInfo(int cType, void *buffer, int bufferSize);
    void callOnRenderYUV(int cType, int width, int height, uint8_t *y, uint8_t *u, uint8_t *v);
};


#endif //ZEDPLAYER_CCALLJAVA_H
