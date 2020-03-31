//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDFFMPEG_H
#define ZEDPLAYER_ZEDFFMPEG_H

#include "../FFmpegDefine.h"
#include "../cjava/CCallJava.h"

class ZedFfmpeg {
public:
    AVFormatContext *pFormatCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    bool foundAudioStream = false;
    int audio_index = -1;

    CCallJava *cCallJava = nullptr;
public:
    ZedFfmpeg(CCallJava *cCallJava);
    ~ZedFfmpeg();
    void prepareMedia(const char* mediaPath);
};


#endif //ZEDPLAYER_ZEDFFMPEG_H
