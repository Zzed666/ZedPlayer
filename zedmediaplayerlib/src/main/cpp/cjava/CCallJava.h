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
    jmethodID jpreparemid = nullptr;
public:
    CCallJava(JavaVM *vm,JNIEnv *env,jobject obj);
    ~CCallJava();
    void callOnPrepare(int cType);
};


#endif //ZEDPLAYER_CCALLJAVA_H
