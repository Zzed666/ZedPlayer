//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDAUDIO_H
#define ZEDPLAYER_ZEDAUDIO_H

#include "../queues/ZedQueue.h"
#include "../status/ZedStatus.h"
#include "../cjava/CCallJava.h"
#include "../soundtouch/include/SoundTouch.h"

using namespace soundtouch;

class ZedAudio {
public:
    int audio_index = -1;
    int sample_rate = -1;
    int resample_nbs = 0;
    int total_duration = 0;
    double last_time = 0;
    double now_time = 0;
    double clock_time = 0;
    int volume_init = 100;
    int mute_init = 2;
    float speed_init = 1.0f;
    float pitch_init = 1.0f;
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
    SLVolumeItf volumeVolume = nullptr;
    SLMuteSoloItf muteMute = nullptr;

    ZedQueue *zedQueue = nullptr;
    ZedStatus *zedStatus = nullptr;
    CCallJava *cCallJava = nullptr;

    SoundTouch *soundTouch = nullptr;
    SAMPLETYPE *sound_touch_buffer_16bit = nullptr;
    uint8_t *sound_touch_buffer_8bit = nullptr;
    bool sound_touch_sample_finished = true;

    pthread_t play_thread;
public:
    ZedAudio(ZedStatus *zedStatus, CCallJava *cCallJava, int sample_rate);
    ~ZedAudio();
    void prepareOpenSELS();
    int getSoundTouchData();
    int getDB(char *pcm_data, size_t data_size);
    int resample();
    void play();
    void pause(bool is_pause);
    void stop();
    void volume(int percent);
    void mute(int mute_channel);
    void speed(float audio_speed);
    void pitch(float audio_pitch);
    int getCurrentSampleRate(int sample_rate);
    void releaseTempSource();
    void release();
};


#endif //ZEDPLAYER_ZEDAUDIO_H
