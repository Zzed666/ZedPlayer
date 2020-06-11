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

bool ffmpeg_stop_complete = true;
pthread_t ffmpeg_start_thread;

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
    if (zedFfmpeg == nullptr) {
        if (cCallJava == nullptr) {
            cCallJava = new CCallJava(javaVm, env, obj);
        }
        if (zedStatus == nullptr) {
            zedStatus = new ZedStatus();
        }
        zedFfmpeg = new ZedFfmpeg(zedStatus, cCallJava, mediaPath);
        zedFfmpeg->prepareMedia();
    }
//    env->ReleaseStringUTFChars(mediaPath_, mediaPath);
}

void *startAudioThread(void *data) {
    auto zedFfmpeg = (ZedFfmpeg *) data;
    zedFfmpeg->startAudio();
    pthread_exit(&ffmpeg_start_thread);
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1start(JNIEnv *env,
                                                                jobject obj) {
    if (zedFfmpeg != nullptr) {
//        zedFfmpeg->startAudio();
        pthread_create(&ffmpeg_start_thread, nullptr, startAudioThread, zedFfmpeg);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg start return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1pause(JNIEnv *env,
                                                                jobject obj,
                                                                jboolean is_pause) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->pauseAudio(is_pause);
    } else {
        if (FFMPEG_LOG) {
            if (is_pause) {
                FFLOGE("ffmpeg pause return because of it isn't initial!")
            } else {
                FFLOGE("ffmpeg resume return because of it isn't initial!")
            }
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1seek(JNIEnv *env,
                                                               jobject obj,
                                                               jint seek_time) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->seekAudio(seek_time);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg seek return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1volume(JNIEnv *env,
                                                                 jobject obj,
                                                                 jint volume_percent) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->volumeAudio(volume_percent);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg volume return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1mute(JNIEnv *env,
                                                               jobject obj,
                                                               jint mute_channel) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->muteAudio(mute_channel);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg mute return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1speed(JNIEnv *env,
                                                                jobject obj,
                                                                jfloat speed) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->speedAudio(speed);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg speed return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1pitch(JNIEnv *env,
                                                                jobject obj,
                                                                jfloat pitch) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->pitchAudio(pitch);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg pitch return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1record(JNIEnv *env,
                                                                 jobject obj,
                                                                 jboolean record) {
    if (zedFfmpeg != nullptr) {
        zedFfmpeg->recordAudio(record);
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg record return because of it isn't initial!")
            return;
        }
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1duration(JNIEnv *env,
                                                                   jobject obj) {
    if (zedFfmpeg != nullptr) {
        return zedFfmpeg->total_duration;
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg get duration return because of it isn't initial!")
        }
        return -1;
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1samplerate(JNIEnv *env,
                                                                     jobject obj) {
    if (zedFfmpeg != nullptr) {
        return zedFfmpeg->sample_rate;
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg get samplerate return because of it isn't initial!")
        }
        return -1;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1stop(JNIEnv *env,
                                                               jobject obj,
                                                               jboolean skipNext,
                                                               jstring nextMediaPath_) {
    if (zedFfmpeg != nullptr) {
        if (!ffmpeg_stop_complete) {
            FFLOGE("ffmpeg stop is not complete")
            return;
        }
        FFLOGI("ffmpeg stopping");
        ffmpeg_stop_complete = false;
        zedFfmpeg->stopAudio();
        if (zedStatus != nullptr) {
            delete (zedStatus);
            zedStatus = nullptr;
        }
        if (cCallJava != nullptr) {
            delete (cCallJava);
            cCallJava = nullptr;
        }
        delete (zedFfmpeg);
        zedFfmpeg = nullptr;
        ffmpeg_stop_complete = true;
        FFLOGE("ffmpeg is stopped");
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg stop return because of it isn't initial!")
        }
    }
    if (skipNext) {
        jclass claz = env->GetObjectClass(obj);
        jmethodID jnextmid = env->GetMethodID(claz, "cCallNextBack", "(Ljava/lang/String;)V");
        env->CallVoidMethod(obj, jnextmid, nextMediaPath_);
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_github_zedmediaplayerlib_audio_ZedAudioPlayer_n_1cutpcm(JNIEnv *env,
                                                                 jobject obj,
                                                                 jfloat startTime,
                                                                 jfloat endTime,
                                                                 jboolean showPcm) {
    if (zedFfmpeg != nullptr) {
        return static_cast<jboolean>(zedFfmpeg->cutPcm(startTime, endTime, showPcm));
    } else {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg cut pcm return because of it isn't initial!")
        }
        return static_cast<jboolean>(false);
    }
}