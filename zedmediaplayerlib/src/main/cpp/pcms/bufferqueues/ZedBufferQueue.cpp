//
// Created by sen.wan on 2020/6/11.
//

#include "ZedBufferQueue.h"

ZedBufferQueue::ZedBufferQueue(ZedStatus *zedStatus) {
    this->zedStatus = zedStatus;
    pthread_mutex_init(&zed_pcm_buffer_mutex, nullptr);
    pthread_cond_init(&zed_pcm_buffer_cond, nullptr);
}

int ZedBufferQueue::putPcmBuffer(SAMPLETYPE *pcmBuffer, int pcmBufferSize) {
    pthread_mutex_lock(&zed_pcm_buffer_mutex);
    ZedPcmBean *zedPcmBean = new ZedPcmBean(pcmBuffer,pcmBufferSize);
    zedPcmQueueBuffer.push_back(zedPcmBean);
    pthread_cond_signal(&zed_pcm_buffer_cond);
    pthread_mutex_unlock(&zed_pcm_buffer_mutex);
    return 0;
}

int ZedBufferQueue::getPcmBuffer(ZedPcmBean **zedPcmBean) {
    pthread_mutex_lock(&zed_pcm_buffer_mutex);
    while (zedStatus != nullptr && !zedStatus->exit) {
        if (zedPcmQueueBuffer.size() > 0) {
           *zedPcmBean = zedPcmQueueBuffer.front();
           zedPcmQueueBuffer.pop_front();
           break;
        } else {
            if (!zedStatus->exit) {
                pthread_cond_wait(&zed_pcm_buffer_cond,&zed_pcm_buffer_mutex);
            }
        }
    }
    pthread_mutex_unlock(&zed_pcm_buffer_mutex);
    return 0;
}

int ZedBufferQueue::getPcmBufferSize() {
    int size = 0;
    pthread_mutex_lock(&zed_pcm_buffer_mutex);
    size = zedPcmQueueBuffer.size();
    pthread_mutex_unlock(&zed_pcm_buffer_mutex);
    return size;
}

int ZedBufferQueue::clearPcmBuffer() {
    pthread_cond_signal(&zed_pcm_buffer_cond);
    pthread_mutex_lock(&zed_pcm_buffer_mutex);
    while (!zedPcmQueueBuffer.empty()) {
        ZedPcmBean *zedPcmBean = zedPcmQueueBuffer.front();
        zedPcmQueueBuffer.pop_front();
        delete(zedPcmBean);
    }
    pthread_mutex_unlock(&zed_pcm_buffer_mutex);
    return 0;
}

int ZedBufferQueue::noticeThread() {
    pthread_cond_signal(&zed_pcm_buffer_cond);
    return 0;
}

void ZedBufferQueue::release() {
    if (FFMPEG_LOG) {
        FFLOGI("ZedBufferQueue release.")
    }
    noticeThread();
    clearPcmBuffer();
}

ZedBufferQueue::~ZedBufferQueue() {
    if (zedStatus != nullptr) {
        zedStatus = nullptr;
    }
    pthread_mutex_destroy(&zed_pcm_buffer_mutex);
    pthread_cond_destroy(&zed_pcm_buffer_cond);
}
