//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDFFMPEG_H
#define ZEDPLAYER_ZEDFFMPEG_H

#include "../FFmpegDefine.h"
#include "../cjava/CCallJava.h"
#include "../status/ZedStatus.h"
#include "../zedaudio/ZedAudio.h"

class ZedFfmpeg {
public:
    AVFormatContext *pFormatCtx = nullptr;
    bool foundAudioStream = false;
    bool ffmpeg_load_exit = false;
    int total_duration = 0;
    int sample_rate = 44100;
    const char* mediaPath = nullptr;

    CCallJava *cCallJava = nullptr;
    ZedAudio *zedAudio = nullptr;
    ZedStatus *zedStatus = nullptr;

    pthread_t prepare_decode_thread;
    pthread_mutex_t load_thread_mutex;
    pthread_mutex_t seek_thread_mutex;
public:
    ZedFfmpeg(ZedStatus *zedStatus,CCallJava *cCallJava,const char* mediaPath);
    ~ZedFfmpeg();
    void prepareMedia();
    void prepareDecode();
    void startAudio();
    void pauseAudio(bool pause);
    void seekAudio(int64_t seek_time);
    void volumeAudio(int volume_percent);
    void muteAudio(int mute);
    void speedAudio(float speed);
    void pitchAudio(float pitch);
    void recordAudio(bool record);
    void stopAudio();
    bool cutPcm(float startTime, float endTime, bool showPcm);
    void release();
};


#endif //ZEDPLAYER_ZEDFFMPEG_H
