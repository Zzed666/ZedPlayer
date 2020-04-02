//
// Created by sen.wan on 2020/3/31.
//

#include "CCallJava.h"

CCallJava::CCallJava(JavaVM *vm, JNIEnv *env, jobject obj) {
    jvm = vm;
    jEnv = env;
    jobj = obj;
    jobj = jEnv->NewGlobalRef(obj);
    jclass claz = jEnv->GetObjectClass(obj);
    if (!claz) {
        if (FFMPEG_LOG) {
            FFLOGE("Cloudn't get jclass");
        }
        return;
    }
    jloadmid = jEnv->GetMethodID(claz, "cCallLoadBack", "(Z)V");
    jpreparemid = jEnv->GetMethodID(claz, "cCallPreparedBack", "()V");
    jpausemid = jEnv->GetMethodID(claz, "cCallPauseBack", "(Z)V");
}

CCallJava::~CCallJava() {

}

void CCallJava::callOnLoad(int cType, bool load) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jloadmid,load);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jloadmid,load);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnPrepare(int cType) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jpreparemid);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jpreparemid);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnPause(int cType, bool pause) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jpausemid,pause);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jpausemid,pause);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}
