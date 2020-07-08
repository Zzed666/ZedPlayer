//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDFFMPEG_H
#define ZEDPLAYER_ZEDFFMPEG_H

#include "../FFmpegDefine.h"
#include "../cjava/CCallJava.h"
#include "../status/ZedStatus.h"
#include "../zedaudio/ZedAudio.h"
#include "../zedvideo/ZedVideo.h"

class ZedFfmpeg {
public:
    AVFormatContext *pFormatCtx = nullptr;
    bool foundAudioStream = false;
    bool foundVideoStream = false;
    bool ffmpeg_load_exit = false;
    int total_duration = 0;
    int sample_rate = 44100;
    const char* mediaPath = nullptr;

    CCallJava *cCallJava = nullptr;
    ZedAudio *zedAudio = nullptr;
    ZedVideo *zedVideo = nullptr;
    ZedStatus *zedStatus = nullptr;

    pthread_t prepare_decode_thread;
    pthread_mutex_t load_thread_mutex;
    pthread_mutex_t seek_thread_mutex;
    pthread_mutex_t status_thread_mutex;
public:
    ZedFfmpeg(ZedStatus *zedStatus,CCallJava *cCallJava,const char* mediaPath);
    ~ZedFfmpeg();
    void prepareMedia();
    void prepareDecode();
    int getAVCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecCtx);
    void startMedia();
    void pauseMedia(bool pause);
    void seekMedia(int64_t seek_time);
    void volumeMedia(int volume_percent);
    void muteMedia(int mute);
    void speedMedia(float speed);
    void pitchMedia(float pitch);
    void recordMedia(bool record);
    void stopMedia();
    void setAllowSplitPcm(bool allow_split);
    bool cutPcm(float startTime, float endTime, bool showPcm);
    void release();
};


#endif //ZEDPLAYER_ZEDFFMPEG_H
