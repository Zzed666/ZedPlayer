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
    ZedQueue *zedQueue = nullptr;
    ZedStatus *zedStatus = nullptr;
public:
    ZedAudio(ZedStatus *zedStatus);
    ~ZedAudio();
};


#endif //ZEDPLAYER_ZEDAUDIO_H
