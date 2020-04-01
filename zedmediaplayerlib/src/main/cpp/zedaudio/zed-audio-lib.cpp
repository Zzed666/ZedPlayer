//
// Created by sen.wan on 2020/3/31.
//

#include <jni.h>
#include <string>

#include "../JniDefine.h"
#include "../ffmpeg/ZedFfmpeg.h"
#include "../status/ZedStatus.h"

JavaVM *javaVm = nullptr;
ZedFfmpeg *zedFfmpeg = nullptr;
ZedStatus *zedStatus = nullptr;
CCallJava *cCallJava = nullptr;

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reversed) {
    javaVm = vm;
    JNIEnv *jniEnv;
    if (vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1prepared(JNIEnv *env,
                                                                   jobject obj,
                                                                   jstring mediaPath_) {
    const char *mediaPath = env->GetStringUTFChars(mediaPath_, 0);
    if (!mediaPath) {
        if (FFMPEG_LOG) {
            FFLOGE("media path is not exist");
        }
        return;
    }
    if (cCallJava == nullptr) {
        cCallJava = new CCallJava(javaVm, env, obj);
    }
    if(zedStatus == nullptr){
        zedStatus = new ZedStatus();
    }
    if (zedFfmpeg == nullptr) {
        zedFfmpeg = new ZedFfmpeg(zedStatus,cCallJava);
    }
    zedFfmpeg->prepareMedia(mediaPath);
    env->ReleaseStringUTFChars(mediaPath_, mediaPath);
}