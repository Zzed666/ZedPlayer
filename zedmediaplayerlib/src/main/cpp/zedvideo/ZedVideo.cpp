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
    auto *zedVideo = (ZedVideo *) data;
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
        pMediaAvPacket = av_packet_alloc();
        if (zedMediaQueue != nullptr && zedMediaQueue->getPackets(pMediaAvPacket) != 0) {
            releaseTempSource(true);
            continue;
        }
        if (avcodec_send_packet(pMediaAvCodecCtx, pMediaAvPacket) != 0) {
            releaseTempSource(true);
            continue;
        }
        pMediaAvFrame = av_frame_alloc();
        if (avcodec_receive_frame(pMediaAvCodecCtx, pMediaAvFrame) != 0) {
            releaseTempSource(true);
            continue;
        }

        if (renderYUV() != 0) continue;

        releaseTempSource(true);
    }
    hasExitDecodeThread = true;
}

int ZedVideo::renderYUV() {
    if (pMediaAvFrame != nullptr) {
        if (pMediaAvFrame->format == AV_PIX_FMT_YUV420P) {
            if (cMediaCallJava != nullptr) {
                cMediaCallJava->callOnRenderYUV(CTHREADTYPE_CHILD,
                        pMediaAvCodecCtx->width,
                        pMediaAvCodecCtx->height,
                        pMediaAvFrame->data[0],
                        pMediaAvFrame->data[1],
                        pMediaAvFrame->data[2]);
            }
        } else {
            AVFrame *pAvFrameYUV420P = av_frame_alloc();
            int nums = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                pMediaAvCodecCtx->width,
                                                pMediaAvCodecCtx->height,
                                                1);
            auto *buffers = static_cast<uint8_t *>(av_malloc(nums * sizeof(uint8_t)));
            av_image_fill_arrays(pAvFrameYUV420P->data, pAvFrameYUV420P->linesize,
                                 buffers,
                                 AV_PIX_FMT_YUV420P, pMediaAvCodecCtx->width,
                                 pMediaAvCodecCtx->height,
                                 1);
            pSwsCtx = sws_getContext(pMediaAvCodecCtx->width,
                                                pMediaAvCodecCtx->height,
                                                pMediaAvCodecCtx->pix_fmt,
                                                pMediaAvCodecCtx->width,
                                                pMediaAvCodecCtx->height,
                                                AV_PIX_FMT_YUV420P,
                                                SWS_BICUBIC, nullptr,
                                                nullptr, nullptr);
            if (!pSwsCtx) {
                FFLOGE("renderYUV get SwsContext error")
                av_frame_free(&pAvFrameYUV420P);
                av_free(pAvFrameYUV420P);
                pAvFrameYUV420P = nullptr;
                av_free(buffers);
                releaseTempSource(true);
                return -1;
            }
            sws_scale(pSwsCtx,
                      pMediaAvFrame->data,
                      pMediaAvFrame->linesize, 0, pMediaAvFrame->height,
                      pAvFrameYUV420P->data, pAvFrameYUV420P->linesize);

            if (cMediaCallJava != nullptr) {
                cMediaCallJava->callOnRenderYUV(CTHREADTYPE_CHILD,
                                                pMediaAvCodecCtx->width,
                                                pMediaAvCodecCtx->height,
                                                pAvFrameYUV420P->data[0],
                                                pAvFrameYUV420P->data[1],
                                                pAvFrameYUV420P->data[2]);
            }

            av_frame_free(&pAvFrameYUV420P);
            av_free(pAvFrameYUV420P);
            pAvFrameYUV420P = nullptr;
            av_free(buffers);
        }
    }
    return 0;
}

void ZedVideo::releaseTempSource(bool is_release_avpacket) {
    if (pMediaAvPacket != nullptr && is_release_avpacket) {
        av_packet_free(&pMediaAvPacket);
        av_free(pMediaAvPacket);
        pMediaAvPacket = nullptr;
    }
    if (pMediaAvFrame != nullptr) {
        av_frame_free(&pMediaAvFrame);
        av_free(pMediaAvFrame);
        pMediaAvFrame = nullptr;
    }
    if (pSwsCtx != nullptr) {
        sws_freeContext(pSwsCtx);
        av_free(pSwsCtx);
        pSwsCtx = nullptr;
    }
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
