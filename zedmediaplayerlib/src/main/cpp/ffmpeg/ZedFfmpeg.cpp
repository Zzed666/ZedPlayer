//
// Created by sen.wan on 2020/3/31.
//

#include "ZedFfmpeg.h"

ZedFfmpeg::ZedFfmpeg(ZedStatus *zedStatus, CCallJava *cCallJava, const char *mediaPath) {
    this->zedStatus = zedStatus;
    this->cCallJava = cCallJava;
    this->mediaPath = mediaPath;
    pthread_mutex_init(&load_thread_mutex, nullptr);
    pthread_mutex_init(&seek_thread_mutex, nullptr);
    pthread_mutex_init(&status_thread_mutex, nullptr);
}

int formatInterruptCallBack(void *context) {
    auto zedFfmpeg = (ZedFfmpeg *) context;
    if (zedFfmpeg->zedStatus->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

void *prepareMediaThread(void *data) {
    auto zedFfmpeg = (ZedFfmpeg *) data;
    zedFfmpeg->prepareDecode();
    pthread_exit(&zedFfmpeg->prepare_decode_thread);
}

void ZedFfmpeg::prepareMedia() {
    pthread_create(&prepare_decode_thread, nullptr, prepareMediaThread, this);
}

void ZedFfmpeg::prepareDecode() {
    cCallJava->callOnLoad(CTHREADTYPE_CHILD, true);
    pthread_mutex_lock(&load_thread_mutex);
    //初始化网络
    avformat_network_init();
    //初始化avformatcontext分配空间
    pFormatCtx = avformat_alloc_context();
    pFormatCtx->interrupt_callback.callback = formatInterruptCallBack;
    pFormatCtx->interrupt_callback.opaque = this;
    //根据路径解封装
    if (avformat_open_input(&pFormatCtx, mediaPath, nullptr, nullptr) != 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Cloudn't open the media:%s", mediaPath);
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 101, "Cloudn't open the media");
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //获取流
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Cloudn't get the media stream.");
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 102, "Cloudn't get the media stream.");
        pthread_mutex_unlock(&load_thread_mutex);
        return;
    }
    //循环遍历流
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (zedAudio == nullptr) {
                zedAudio = new ZedAudio(zedStatus, cCallJava,
                                        pFormatCtx->streams[i]->codecpar->sample_rate);
            }
            foundAudioStream = true;
            zedAudio->audio_index = i;
            total_duration = pFormatCtx->duration / AV_TIME_BASE;
            sample_rate = pFormatCtx->streams[i]->codecpar->sample_rate;
            zedAudio->audio_time_base = pFormatCtx->streams[zedAudio->audio_index]->time_base;
            zedAudio->total_duration = total_duration;
//            break;
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (zedVideo == nullptr) {
                zedVideo = new ZedVideo(zedStatus, cCallJava);
            }
            foundVideoStream = true;
            zedVideo->video_index = i;
//            total_duration = pFormatCtx->duration / AV_TIME_BASE;
            zedVideo->video_time_base = pFormatCtx->streams[zedVideo->video_index]->time_base;
            int num = pFormatCtx->streams[zedVideo->video_index]->avg_frame_rate.num;
            int den = pFormatCtx->streams[zedVideo->video_index]->avg_frame_rate.den;
            if (num != 0 && den != 0) {
                zedVideo->delay_time_default = den * 1.0 / num;
            }
//            zedAudio->total_duration = total_duration;
//            break;
        }
    }
    if (zedAudio != nullptr) {
        if (!foundAudioStream) {
            if (FFMPEG_LOG) {
                FFLOGE("Couldn't find audio.");
            }
            ffmpeg_load_exit = true;
            cCallJava->callOnError(CTHREADTYPE_CHILD, 103, "Couldn't find audio.");
            pthread_mutex_unlock(&load_thread_mutex);
            return;
        }
        getAVCodecContext(pFormatCtx->streams[zedAudio->audio_index]->codecpar,&zedAudio->pAvCodecCtx);
    }
    if (zedVideo != nullptr) {
        if (!foundVideoStream) {
            if (FFMPEG_LOG) {
                FFLOGE("Couldn't find video.");
            }
            ffmpeg_load_exit = true;
            cCallJava->callOnError(CTHREADTYPE_CHILD, 104, "Couldn't find video.");
            pthread_mutex_unlock(&load_thread_mutex);
            return;
        }
        getAVCodecContext(pFormatCtx->streams[zedVideo->video_index]->codecpar,&zedVideo->pMediaAvCodecCtx);
    }
    ffmpeg_load_exit = true;
    pthread_mutex_unlock(&load_thread_mutex);
    if (cCallJava != nullptr) {
        cCallJava->callOnPrepare(CTHREADTYPE_CHILD);
    }
}

int ZedFfmpeg::getAVCodecContext(AVCodecParameters *codecpar, AVCodecContext **pAvCodecCtx) {
    //获取解码器
    AVCodec *pAvCodec = avcodec_find_decoder(codecpar->codec_id);
    if (!pAvCodec) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't get codec.");
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 105, "Couldn't get codec.");
        pthread_mutex_unlock(&load_thread_mutex);
        return -1;
    }
    //分配空间给解码器上下文
    *pAvCodecCtx = avcodec_alloc_context3(pAvCodec);
    if (!*pAvCodecCtx) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't alloc codecCtx.");
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 106, "Couldn't alloc codecCtx.");
        pthread_mutex_unlock(&load_thread_mutex);
        return -1;
    }
    //给解码器上下文赋值参数
    if (avcodec_parameters_to_context(*pAvCodecCtx, codecpar) < 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't params to codecCtx.");
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 107, "Couldn't params to codecCtx.");
        pthread_mutex_unlock(&load_thread_mutex);
        return -1;
    }
    //打开解码器
    if (avcodec_open2(*pAvCodecCtx, pAvCodec, nullptr) != 0) {
        if (FFMPEG_LOG) {
            FFLOGE("Couldn't open codec.");
        }
        ffmpeg_load_exit = true;
        cCallJava->callOnError(CTHREADTYPE_CHILD, 108, "Couldn't open codec.");
        pthread_mutex_unlock(&load_thread_mutex);
        return -1;
    }
    return 0;
}

void ZedFfmpeg::startMedia() {
    pthread_mutex_lock(&load_thread_mutex);
    if (foundAudioStream && zedAudio == nullptr) {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg startMedia return because of zedAudio is null");
        }
        return;
    } else if (foundVideoStream && zedVideo == nullptr) {
        if (FFMPEG_LOG) {
            FFLOGE("ffmpeg startMedia return because of zedVideo is null");
        }
        return;
    }
    pthread_mutex_lock(&status_thread_mutex);
    if (zedAudio != nullptr) {
        zedAudio->play();
    }
    pthread_mutex_unlock(&status_thread_mutex);
    pthread_mutex_lock(&status_thread_mutex);
    if (zedVideo != nullptr) {
        zedVideo->zedMediaAudio = zedAudio;
        zedVideo->play();
    }
    pthread_mutex_unlock(&status_thread_mutex);
    while (zedStatus != nullptr && !zedStatus->exit) {
        if (zedStatus->seeking) {
            if (FFMPEG_LOG) {
                FFLOGI("ffmpeg is seeking,continue to decode media");
            }
            av_usleep(1000 * 100);
            continue;
        }
        /**
         * 当文件不大，解码速度很快的时候即av_read_frame!=0，当seek的时候clear queue的AvPacket，
         * while循环中判断getAvPacketSize为0，即PlayStatus->exit设置为true,所以就跳出此循环，seek功能也因clear queue取不到AvPacket而失效。
         * 所以我们设置队列为固定大小，>某个值（此处为40）的时候continue,这样就能保证我们只保存此大小的AvPacket队列，
         * 即使seek的时候clear queue中AvPacket也只是清除此数值大小的队列数据，av_read_frame=0依然可以读到下一帧数据
         * 另外还可以减少内存的占用
         * (ps:.ape文件可能一个AVPacket包含多个AVFrame,所以当设置的缓存队列过大时,seek的时候clear queue的AvPacket,有可能缓存数据已经完毕,从而造成未seek到结尾就播放完,所以此时缓存队列
         * 应该设置足够小,才不会出现这种情况,所以一般设为 1)
         * */
//        if (zedAudio != nullptr && zedAudio->zedQueue != nullptr &&
//            zedAudio->zedQueue->getPacketSize() > 100) {
//            av_usleep(1000 * 100);
//            continue;
//        }
        //读取数据到AVPacket
        AVPacket *pPacket = av_packet_alloc();
        pthread_mutex_lock(&seek_thread_mutex);
        if (pFormatCtx == nullptr) {
            if (FFMPEG_LOG) {
                FFLOGE("ffmpeg startMedia return because of pFormatCtx is null");
            }
            return;
        }
        int ret = av_read_frame(pFormatCtx, pPacket);
        pthread_mutex_unlock(&seek_thread_mutex);
        if (ret == 0) {
            if (pPacket->stream_index == zedAudio->audio_index) {
                if (zedAudio != nullptr && zedAudio->zedQueue != nullptr) {
                    zedAudio->zedQueue->putPackets(pPacket);
                }
            } else if (pPacket->stream_index == zedVideo->video_index) {
                if (zedVideo != nullptr && zedVideo->zedMediaQueue != nullptr) {
                    zedVideo->zedMediaQueue->putPackets(pPacket);
                }
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
                if ((zedAudio != nullptr && zedAudio->zedQueue->getPacketSize() == 0)
                    && (zedVideo != nullptr && zedVideo->zedMediaQueue->getPacketSize() == 0)) {
                    zedStatus->exit = true;
                    break;
                } else {
                    av_usleep(1000 * 100);
                    continue;
                }
            }
            break;
        }
    }

    pthread_mutex_unlock(&load_thread_mutex);

    if (cCallJava != nullptr) {
        FFLOGI("callOnComplete");
        cCallJava->callOnComplete(CTHREADTYPE_CHILD);
    }

    if (FFMPEG_LOG) {
        FFLOGI("解码播放完成");
    }

}

void ZedFfmpeg::pauseMedia(bool is_pause) {
    zedAudio->pause(is_pause);
}

void ZedFfmpeg::seekMedia(int64_t seek_time) {
    if (total_duration <= 0 || seek_time < 0 || seek_time > total_duration) {
        if (FFMPEG_LOG) {
            FFLOGI("ffmpeg total_duration <= 0 or seek time < 0 or > total duration");
        }
        return;
    }
    zedStatus->seeking = true;
    if (zedAudio != nullptr) {
        zedAudio->zedQueue->clearAvPackets();
        //由于.ape等文件可能一个AVPacket包含多个AVFrame,所以在seek的时候可能会出现还有缓存的AVFrame的情况,也就是不会立即播放当前位置，而是把缓存frame播放完再播放当前位置
        //所以seek的时候还要flush缓存
        avcodec_flush_buffers(zedAudio->pAvCodecCtx);
        zedAudio->last_time = 0;
        zedAudio->clock_time = 0;
        pthread_mutex_lock(&seek_thread_mutex);
        int64_t seeks = seek_time * AV_TIME_BASE;
        if (avformat_seek_file(pFormatCtx, -1, INT64_MIN, seeks, INT64_MAX, 0) >=
            0) {
            cCallJava->callOnSeek(CTHREADTYPE_CHILD, total_duration, seek_time);
        } else {
            if (FFMPEG_LOG) {
                FFLOGE("Audio seek error");
            }
            cCallJava->callOnError(CTHREADTYPE_CHILD, 109, "Audio seek error");
        }
        pthread_mutex_unlock(&seek_thread_mutex);
    }
    zedStatus->seeking = false;
}

void ZedFfmpeg::volumeMedia(int volume_percent) {
    if (zedAudio != nullptr) {
        zedAudio->volume(volume_percent);
    }
}

void ZedFfmpeg::muteMedia(int mute_channel) {
    if (zedAudio != nullptr) {
        zedAudio->mute(mute_channel);
    }
}

void ZedFfmpeg::speedMedia(float audio_speed) {
    if (zedAudio != nullptr) {
        zedAudio->speed(audio_speed);
    }
}

void ZedFfmpeg::pitchMedia(float audio_pitch) {
    if (zedAudio != nullptr) {
        zedAudio->pitch(audio_pitch);
    }
}

void ZedFfmpeg::recordMedia(bool audio_record) {
    if (zedAudio != nullptr) {
        zedAudio->setRecord(audio_record);
    }
}

void ZedFfmpeg::setAllowSplitPcm(bool allow_split) {
    if (zedAudio != nullptr) {
        zedAudio->setSplitPcmBuffer(allow_split);
    }
}

bool ZedFfmpeg::cutPcm(float startTime, float endTime, bool showPcm) {
    if (startTime >= 0 && endTime > startTime
        && endTime <= total_duration && zedAudio != nullptr) {
        zedAudio->setCutPcm(startTime,endTime,showPcm);
        seekMedia(static_cast<int64_t>(startTime));
        return true;
    }
    return false;
}

void ZedFfmpeg::stopMedia() {
//    //为了防止av_read_frame = 0之后，判断getPacketSize = 0，zedStatus->exit设置为true的时候再调用stopAudio会直接return，所以这段注释掉
//    if (zedStatus->exit) {
//        if (FFMPEG_LOG) {
//            FFLOGI("ffmpeg has already exited");
//        }
//        return;
//    }
    if (zedStatus != nullptr) {
        zedStatus->exit = true;
    }
    int sleep_count = 0;
    pthread_mutex_lock(&load_thread_mutex);
    while (!ffmpeg_load_exit) {
        if (sleep_count > 1000) ffmpeg_load_exit = true;
        if (FFMPEG_LOG) {
            FFLOGI("wait ffmpeg load time:%d", sleep_count);
        }
        sleep_count++;
        av_usleep(10 * 1000);
    }
    if (zedAudio != nullptr) {
        zedAudio->stop();
    }
//    cCallJava->callOnStop(CTHREADTYPE_CHILD);
    release();
    pthread_mutex_unlock(&load_thread_mutex);
}

void ZedFfmpeg::release() {
    if (zedVideo != nullptr) {
        zedVideo->release();
        delete (zedVideo);
        zedVideo = nullptr;
    }
    if (zedAudio != nullptr) {
        zedAudio->release();
        delete (zedAudio);
        zedAudio = nullptr;
    }
    if (pFormatCtx != nullptr) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
    }
    if (zedStatus != nullptr) {
        zedStatus = nullptr;
    }
    if (cCallJava != nullptr) {
        cCallJava->callOnStop(CTHREADTYPE_CHILD);
        cCallJava = nullptr;
    }
}

ZedFfmpeg::~ZedFfmpeg() {
    pthread_mutex_destroy(&load_thread_mutex);
    pthread_mutex_destroy(&seek_thread_mutex);
    pthread_mutex_destroy(&status_thread_mutex);
}
