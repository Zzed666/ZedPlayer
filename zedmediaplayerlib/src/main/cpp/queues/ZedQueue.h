//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDQUEUE_H
#define ZEDPLAYER_ZEDQUEUE_H

#include "../FFmpegDefine.h"
#include "../status/ZedStatus.h"
#include <queue>
#include <pthread.h>

class ZedQueue {
public:
    pthread_mutex_t decode_thread_mutex;
    pthread_cond_t decode_thread_cond;
    std::queue<AVPacket *> zedQueue;

    ZedStatus *zedStatus = nullptr;
public:
    ZedQueue(ZedStatus *zedStatus);
    ~ZedQueue();
    void putPackets(AVPacket *avPacket);
    int getPackets(AVPacket *avPacket);
    int getPacketSize();
};


#endif //ZEDPLAYER_ZEDQUEUE_H
