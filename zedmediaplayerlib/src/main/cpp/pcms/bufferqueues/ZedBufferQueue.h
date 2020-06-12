//
// Created by sen.wan on 2020/6/11.
//

#ifndef ZEDPLAYER_ZEDBUFFERQUEUE_H
#define ZEDPLAYER_ZEDBUFFERQUEUE_H

#include "deque"
#include "../../status/ZedStatus.h"
#include "../../pcms/beans/ZedPcmBean.h"
#include <pthread.h>
#include "../../JniDefine.h"
#include "../../FFmpegDefine.h"

class ZedBufferQueue {
public:
    std::deque<ZedPcmBean *> zedPcmQueueBuffer;
    pthread_mutex_t zed_pcm_buffer_mutex;
    pthread_cond_t zed_pcm_buffer_cond;
    ZedStatus *zedStatus = nullptr;
public:
    ZedBufferQueue(ZedStatus *zedStatus);
    ~ZedBufferQueue();
    int putPcmBuffer(SAMPLETYPE *pcmBuffer, int pcmBufferSize);
    int getPcmBuffer(ZedPcmBean **zedPcmBean);
    int getPcmBufferSize();
    int clearPcmBuffer();
    int noticeThread();
    void release();
};


#endif //ZEDPLAYER_ZEDBUFFERQUEUE_H
