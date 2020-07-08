//
// Created by sen.wan on 2020/7/1.
//

#ifndef ZEDPLAYER_ZEDVIDEO_H
#define ZEDPLAYER_ZEDVIDEO_H

#include "../queues/ZedQueue.h"
#include "../status/ZedStatus.h"
#include "../cjava/CCallJava.h"

class ZedVideo {

public:
    int video_index = -1;
    bool hasExitDecodeThread = true;

    AVCodecContext *pMediaAvCodecCtx = nullptr;
    ZedStatus *zedMediaStatus = nullptr;
    CCallJava *cMediaCallJava = nullptr;
    ZedQueue *zedMediaQueue = nullptr;

    pthread_t play_media_thread;

public:
    ZedVideo(ZedStatus *zedStatus, CCallJava *cCallJava);
    void play();
    void decode();
    void release();
    ~ZedVideo();
};


#endif //ZEDPLAYER_ZEDVIDEO_H
