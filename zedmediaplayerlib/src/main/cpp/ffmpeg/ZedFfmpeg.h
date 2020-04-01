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
    AVCodec *pCodec = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    bool foundAudioStream = false;

    CCallJava *cCallJava = nullptr;
    ZedAudio *zedAudio = nullptr;
    ZedStatus *zedStatus = nullptr;

public:
    ZedFfmpeg(ZedStatus *zedStatus,CCallJava *cCallJava);
    ~ZedFfmpeg();
    void prepareMedia(const char* mediaPath);
    void start();
};


#endif //ZEDPLAYER_ZEDFFMPEG_H
