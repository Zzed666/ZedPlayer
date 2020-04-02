//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDAUDIO_H
#define ZEDPLAYER_ZEDAUDIO_H

#include "../queues/ZedQueue.h"
#include "../status/ZedStatus.h"

class ZedAudio {
public:
    int audio_index = -1;
    uint8_t *out_buffer;

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

    pthread_t play_thread;
public:
    ZedAudio(ZedStatus *zedStatus);
    ~ZedAudio();
    void play();
    void prepareOpenSELS();
    int resample();
    void releaseTempSource();
};


#endif //ZEDPLAYER_ZEDAUDIO_H
