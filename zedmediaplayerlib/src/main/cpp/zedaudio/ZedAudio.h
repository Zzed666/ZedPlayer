//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDAUDIO_H
#define ZEDPLAYER_ZEDAUDIO_H

#include "../queues/ZedQueue.h"
#include "../status/ZedStatus.h"
#include "../cjava/CCallJava.h"

class ZedAudio {
public:
    int audio_index = -1;
    int sample_rate = -1;
    int total_duration = 0;
    double last_time = 0;
    double now_time = 0;
    double clock_time = 0;
    uint8_t *out_buffer;
    AVRational audio_time_base;

    AVCodec *pAvCodec = nullptr;
    AVCodecContext *pAvCodecCtx = nullptr;
    AVPacket *pAvPacket = nullptr;
    AVFrame *pAvFrame = nullptr;
    SwrContext *pSwrCtx = nullptr;

    SLObjectItf engineObj = nullptr;
    SLObjectItf mixoutObj = nullptr;
    SLObjectItf playObj = nullptr;

    SLEngineItf engineEngine = nullptr;
    SLEnvironmentalReverbItf mixoutEnvironmentalReverb = nullptr;
    SLPlayItf playPlay = nullptr;
    SLAndroidSimpleBufferQueueItf androidSimpleBufferQueue = nullptr;

    ZedQueue *zedQueue = nullptr;
    ZedStatus *zedStatus = nullptr;
    CCallJava *cCallJava = nullptr;

    pthread_t play_thread;
public:
    ZedAudio(ZedStatus *zedStatus, CCallJava *cCallJava, int sample_rate);
    ~ZedAudio();
    void prepareOpenSELS();
    int resample();
    void play();
    void pause(bool is_pause);
    void stop();
    int getCurrentSampleRate(int sample_rate);
    void releaseTempSource();
    void release();
};


#endif //ZEDPLAYER_ZEDAUDIO_H
