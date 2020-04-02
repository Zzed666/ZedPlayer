//
// Created by sen.wan on 2020/3/31.
//

#include "ZedFfmpeg.h"

ZedFfmpeg::ZedFfmpeg(ZedStatus *zedStatus, CCallJava *cCallJava) {
    this->zedStatus = zedStatus;
    this->cCallJava = cCallJava;
    pthread_mutex_init(&load_thread_mutex, nullptr);
}

void ZedFfmpeg::prepareMedia(const char *mediaPath) {
    cCallJava->callOnLoad(CTHREADTYPE_CHILD,true);
    pthread_mutex_lock(&load_thread_mutex);
    //初始化网络
    avformat_network_init();
    //初始化avformatcontext分配空间
    pFormatCtx = avformat_alloc_context();
    //根据路径解封装
    if (avformat_open_input(&pFormatCtx, mediaPath, nullptr, nullptr) != 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Cloudn't open the media:%s", mediaPath);
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //获取流
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Cloudn't get the media stream.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //循环遍历流
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (zedAudio == nullptr) {
                zedAudio = new ZedAudio(zedStatus,cCallJava);
            }
            foundAudioStream = true;
            zedAudio->audio_index = i;
            break;
        }
    }
    if (!foundAudioStream) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't find audio.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //获取解码器
    pCodec = avcodec_find_decoder(pFormatCtx->streams[zedAudio->audio_index]->codecpar->codec_id);
    if (!pCodec) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't get codec.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //分配空间给解码器上下文
    zedAudio->pAvCodecCtx = avcodec_alloc_context3(pCodec);
    if (!zedAudio->pAvCodecCtx) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't alloc codecCtx.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //给解码器上下文赋值参数
    if (avcodec_parameters_to_context(zedAudio->pAvCodecCtx,
                                      pFormatCtx->streams[zedAudio->audio_index]->codecpar) < 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't params to codecCtx.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //打开解码器
    if (avcodec_open2(zedAudio->pAvCodecCtx, pCodec, nullptr) != 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't open codec.");
        }
        ffmpeg_load_exit = true;
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    cCallJava->callOnPrepare(CTHREADTYPE_CHILD);
    ffmpeg_load_exit = true;
    pthread_mutex_unlock(&load_thread_mutex);
    startDecodeAudio();
}

void ZedFfmpeg::startDecodeAudio() {
    zedAudio->play();
    while (zedStatus != nullptr && !zedStatus->exit) {
        //读取数据到AVPacket
        AVPacket *pPacket = av_packet_alloc();
        if (av_read_frame(pFormatCtx, pPacket) == 0) {
            if (pPacket->stream_index == zedAudio->audio_index) {
                zedAudio->zedQueue->putPackets(pPacket);
            } else {
                av_packet_free(&pPacket);
                av_free(pPacket);
                pPacket = nullptr;
            }
        } else {
            if (FFMPEG_LOG) {
                FFLOGE("decode finished");
            }
            av_packet_free(&pPacket);
            av_free(pPacket);
            pPacket = nullptr;
            while (zedStatus != nullptr && !zedStatus->exit) {
                if (zedAudio->zedQueue->getPacketSize() == 0) {
                    zedStatus->exit = true;
                    break;
                } else continue;
            }
            break;
        }
    }

    if (FFMPEG_LOG) {
        FFLOGI("解码完成");
    }

}

void ZedFfmpeg::pauseAudio(bool is_pause) {
    zedAudio->pause(is_pause);
}

ZedFfmpeg::~ZedFfmpeg() {
    pthread_mutex_destroy(&load_thread_mutex);
}
