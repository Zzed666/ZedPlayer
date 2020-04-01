//
// Created by sen.wan on 2020/3/31.
//

#include "ZedQueue.h"

ZedQueue::ZedQueue(ZedStatus *zedStatus) {
    this->zedStatus = zedStatus;
    pthread_mutex_init(&decode_thread_mutex, nullptr);
    pthread_cond_init(&decode_thread_cond, nullptr);
}

void ZedQueue::putPackets(AVPacket *avPacket) {
    pthread_mutex_lock(&decode_thread_mutex);
    zedQueue.push(avPacket);
    pthread_cond_signal(&decode_thread_cond);
    if (FFMPEG_LOG) {
        FFLOGI("解码一帧放入队列中,队列还有%d帧数据", zedQueue.size())
    }
    pthread_mutex_unlock(&decode_thread_mutex);
}

void ZedQueue::getPackets(AVPacket *avPacket) {
    pthread_mutex_lock(&decode_thread_mutex);
    while (zedStatus != nullptr && !zedStatus->exit) {
        if (!zedQueue.empty()) {
            AVPacket *packet = nullptr;
            packet = zedQueue.front();
            if (av_packet_ref(avPacket, packet) == 0) {
                zedQueue.pop();
            } else {
                if (FFMPEG_LOG) {
                    FFLOGE("av_packet_ref error");
                }
            }
            if (FFMPEG_LOG) {
                FFLOGI("从队列中取出一帧数据,还剩%d帧数据", zedQueue.size());
            }
            av_packet_free(&packet);
            av_free(packet);
            packet = nullptr;
            break;
        } else {
            pthread_cond_wait(&decode_thread_cond, &decode_thread_mutex);
        }
    }
    pthread_mutex_unlock(&decode_thread_mutex);
}

int ZedQueue::getPacketSize() {
    int size = 0;
    pthread_mutex_lock(&decode_thread_mutex);
    size = zedQueue.size();
    pthread_mutex_unlock(&decode_thread_mutex);
    return size;
}

ZedQueue::~ZedQueue() {
    pthread_mutex_destroy(&decode_thread_mutex);
    pthread_cond_destroy(&decode_thread_cond);
}
