//
// Created by sen.wan on 2020/7/1.
//

#include "ZedVideo.h"

ZedVideo::ZedVideo(ZedStatus *zedStatus, CCallJava *cCallJava) {
    this->zedMediaStatus = zedStatus;
    this->cMediaCallJava = cCallJava;
    this->zedMediaQueue = new ZedQueue(zedStatus);
}

void *playCallBack(void *data) {
    auto *zedVideo = (ZedVideo *)data;
    zedVideo->decode();
    pthread_exit(&zedVideo->play_media_thread);
}

void ZedVideo::play() {
    pthread_create(&play_media_thread, nullptr, playCallBack, this);
}

void ZedVideo::decode() {
    hasExitDecodeThread = false;
    while (zedMediaStatus != nullptr && !zedMediaStatus->exit) {
        if (zedMediaStatus->seeking) {
            av_usleep(1000 * 100);
            continue;
        }
        if (zedMediaQueue != nullptr && zedMediaQueue->getPacketSize() == 0) {
            if (!zedMediaStatus->load) {
                cMediaCallJava->callOnLoad(CTHREADTYPE_CHILD, true);
                zedMediaStatus->load = true;
            }
//            sleep的过程中，video release，所以status设置为null没用，video release完之后就开始delete，等到delete完之后，sleep刚好结束，重新判断while循环条件时就会crash;所以注释掉此处的sleep
//            av_usleep(1000 * 100);
            continue;
        } else {
            if (zedMediaStatus->load) {
                cMediaCallJava->callOnLoad(CTHREADTYPE_CHILD, false);
                zedMediaStatus->load = false;
            }
        }
        AVPacket *avPacket = av_packet_alloc();
        if (zedMediaQueue != nullptr && zedMediaQueue->getPackets(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = nullptr;
            continue;
        }
        if (avcodec_send_packet(pMediaAvCodecCtx, avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = nullptr;
            continue;
        }
        AVFrame *avFrame = av_frame_alloc();
        if (avcodec_receive_frame(pMediaAvCodecCtx, avFrame) != 0) {
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = nullptr;
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = nullptr;
            continue;
        }
        if (FFMPEG_LOG) {
            FFLOGI("zedVideo get a frame");
        }
        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = nullptr;
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = nullptr;
    }
    hasExitDecodeThread = true;
}

void ZedVideo::release() {
    if (zedMediaQueue != nullptr) {
        delete (zedMediaQueue);
        zedMediaQueue = nullptr;
    }
    if (pMediaAvCodecCtx != nullptr) {
        avcodec_close(pMediaAvCodecCtx);
        avcodec_free_context(&pMediaAvCodecCtx);
        pMediaAvCodecCtx = nullptr;
    }
    if (zedMediaStatus != nullptr) {
        zedMediaStatus = nullptr;
    }
    if (cMediaCallJava != nullptr) {
        cMediaCallJava = nullptr;
    }
}

ZedVideo::~ZedVideo() {

}
