//
// Created by sen.wan on 2020/7/1.
//

#ifndef ZEDPLAYER_ZEDVIDEO_H
#define ZEDPLAYER_ZEDVIDEO_H

#include "../queues/ZedQueue.h"
#include "../status/ZedStatus.h"
#include "../cjava/CCallJava.h"
#include "../zedaudio/ZedAudio.h"

class ZedVideo {

public:
    int video_index = -1;
    double clock_time = 0;
    double delay_time_default = 0.04;
    double delay_time = 0;
    AVRational video_time_base;
    bool hasExitDecodeThread = true;

    AVCodecContext *pMediaAvCodecCtx = nullptr;
    AVPacket *pMediaAvPacket = nullptr;
    AVFrame *pMediaAvFrame = nullptr;
    SwsContext *pSwsCtx = nullptr;

    ZedAudio *zedMediaAudio = nullptr;
    ZedStatus *zedMediaStatus = nullptr;
    CCallJava *cMediaCallJava = nullptr;
    ZedQueue *zedMediaQueue = nullptr;

    pthread_t play_media_thread;

public:
    ZedVideo(ZedStatus *zedStatus, CCallJava *cCallJava);
    void play();
    void decode();
    double getAVFrameDiffTime(AVFrame *pAVFrame);
    double getDelayTime(double diff);
    void resetDelayTime();
    int renderYUV();
    void releaseTempSource(bool is_release_avpacket);
    void release();
    ~ZedVideo();
};


#endif //ZEDPLAYER_ZEDVIDEO_H
