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
    jplaytimemid = jEnv->GetMethodID(claz, "cCallPlayTimeBack", "(II)V");
    jdbmid = jEnv->GetMethodID(claz, "cCallDBBack", "(I)V");
    jpausemid = jEnv->GetMethodID(claz, "cCallPauseBack", "(Z)V");
    jseekmid = jEnv->GetMethodID(claz, "cCallSeekBack", "(II)V");
    jstopmid = jEnv->GetMethodID(claz, "cCallStopBack", "()V");
    jerrormid = jEnv->GetMethodID(claz, "cCallErrorBack", "(ILjava/lang/String;)V");
    jcompletemid = jEnv->GetMethodID(claz, "cCallCompleteBack", "()V");
    jpcmtoaacmid = jEnv->GetMethodID(claz, "cCallPcmToAACBack", "([BI)V");
}

CCallJava::~CCallJava() {

}

void CCallJava::callOnLoad(int cType, bool load) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jloadmid, load);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jloadmid, load);
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

void CCallJava::callOnPlayTime(int cType, int total, int current) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jplaytimemid, total, current);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jplaytimemid, total, current);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnDB(int cType, int db) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jdbmid, db);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jdbmid, db);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnPause(int cType, bool pause) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jpausemid, pause);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jpausemid, pause);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnSeek(int cType, int total, int seek) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jseekmid, total, seek);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jseekmid, total, seek);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnStop(int cType) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jstopmid);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jstopmid);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnError(int cType, int errorCode, char *errorMsg) {
    if (cType == CTHREADTYPE_MAIN) {
        jstring error_msg = jEnv->NewStringUTF(errorMsg);
        jEnv->CallVoidMethod(jobj, jerrormid, errorCode, error_msg);
        jEnv->DeleteLocalRef(error_msg);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jstring error_msg = jniEnv->NewStringUTF(errorMsg);
        jniEnv->CallVoidMethod(jobj, jerrormid, errorCode, error_msg);
        jniEnv->DeleteLocalRef(error_msg);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnComplete(int cType) {
    if (cType == CTHREADTYPE_MAIN) {
        jEnv->CallVoidMethod(jobj, jcompletemid);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jniEnv->CallVoidMethod(jobj, jcompletemid);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}

void CCallJava::callOnPcmToAAC(int cType, void *buffer, int bufferSize) {
    if (cType == CTHREADTYPE_MAIN) {
        jbyteArray bufferArray = jEnv->NewByteArray(bufferSize);
        jEnv->SetByteArrayRegion(bufferArray, 0, bufferSize, static_cast<const jbyte *>(buffer));
        jEnv->CallVoidMethod(jobj, jpcmtoaacmid,bufferArray,bufferSize);
        jEnv->DeleteLocalRef(bufferArray);
    } else if (cType == CTHREADTYPE_CHILD) {
        JNIEnv *jniEnv;
        bool isAttached = false;
        if ((jvm->GetEnv((void **) &jniEnv, JNI_VERSION_1_6)) < 0) {
            if (jvm->AttachCurrentThread(&jniEnv, 0)) {
                return;
            }
            isAttached = true;
        }
        jbyteArray bufferArray = jniEnv->NewByteArray(bufferSize);
        jniEnv->SetByteArrayRegion(bufferArray, 0, bufferSize, static_cast<const jbyte *>(buffer));
        jniEnv->CallVoidMethod(jobj, jpcmtoaacmid,bufferArray,bufferSize);
        jniEnv->DeleteLocalRef(bufferArray);
        if (isAttached) {
            jvm->DetachCurrentThread();
        }
    }
}
