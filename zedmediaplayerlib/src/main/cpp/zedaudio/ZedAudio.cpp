//
// Created by sen.wan on 2020/3/31.
//

#include "ZedAudio.h"

ZedAudio::ZedAudio(ZedStatus *zedStatus, CCallJava *cCallJava) {
    this->zedStatus = zedStatus;
    this->cCallJava = cCallJava;
    zedQueue = new ZedQueue(zedStatus);
    out_buffer = (uint8_t *) (av_malloc(44100 * 2 * 2));
}

void *playThreadCallBack(void *data) {
    auto *zedAudio = (ZedAudio *) data;
    zedAudio->prepareOpenSELS();
    pthread_exit(&zedAudio->play_thread);
}

void openSLESBufferQueueCallBack(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    auto *zedAudio = (ZedAudio *) context;
    if (zedAudio != nullptr) {
        int size = zedAudio->resample();
        //将重采样后的数据压入OpenSLES中的播放队列中
        if (size > 0) {
            (*zedAudio->androidSimpleBufferQueue)->Enqueue(zedAudio->androidSimpleBufferQueue,
                                                           zedAudio->out_buffer,
                                                           size);
        }
    }
}

void ZedAudio::play() {
    pthread_create(&play_thread, nullptr, playThreadCallBack, this);
}

void ZedAudio::prepareOpenSELS() {
    //创建引擎
    slCreateEngine(&engineObj, 0, 0, 0, 0, 0);
    (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engineEngine);
    //创建混音器
    const SLInterfaceID mixoutItfs[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mixoutBools[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine,
                                     &mixoutObj,
                                     1,
                                     mixoutItfs,
                                     mixoutBools);
    (*mixoutObj)->Realize(mixoutObj, SL_BOOLEAN_FALSE);
    (*mixoutObj)->GetInterface(mixoutObj, SL_IID_ENVIRONMENTALREVERB, &mixoutEnvironmentalReverb);
    const SLEnvironmentalReverbSettings pProperties = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*mixoutEnvironmentalReverb)->SetEnvironmentalReverbProperties(mixoutEnvironmentalReverb,
                                                                   &pProperties);
    //创建播放器
    SLDataFormat_PCM dataFormatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                          2};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixoutObj};
    SLDataSource pAudioSrc = {&bufferQueue, &dataFormatPcm};
    SLDataSink pAudioSnk = {&outputMix, 0};
    const SLInterfaceID playItfs[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean playBools[1] = {SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine,
                                       &playObj,
                                       &pAudioSrc,
                                       &pAudioSnk,
                                       1,
                                       playItfs,
                                       playBools);
    (*playObj)->Realize(playObj, SL_BOOLEAN_FALSE);
    (*playObj)->GetInterface(playObj, SL_IID_PLAY, &playPlay);
    //设置缓冲队列和回调函数
    (*playObj)->GetInterface(playObj, SL_IID_BUFFERQUEUE, &androidSimpleBufferQueue);
    (*androidSimpleBufferQueue)->RegisterCallback(androidSimpleBufferQueue,
                                                  &openSLESBufferQueueCallBack, this);
    //设置播放状态
    (*playPlay)->SetPlayState(playPlay, SL_PLAYSTATE_PLAYING);
    //启用回调函数
    openSLESBufferQueueCallBack(androidSimpleBufferQueue, this);
}

int ZedAudio::resample() {
    int ret = -1;
    int resample_size = 0;
    while (zedStatus != nullptr && !zedStatus->exit) {
        if (zedQueue->getPacketSize() == 0) {
            if (!zedStatus->load) {
                zedStatus->load = true;
                cCallJava->callOnLoad(CTHREADTYPE_CHILD, true);
            }
            continue;
        } else {
            if (zedStatus->load) {
                zedStatus->load = false;
                cCallJava->callOnLoad(CTHREADTYPE_CHILD, false);
            }
        }
        pAvPacket = av_packet_alloc();
        if (zedQueue->getPackets(pAvPacket) != 0) {
            releaseTempSource();
            continue;
        }
        ret = avcodec_send_packet(pAvCodecCtx, pAvPacket);
        if (ret != 0) {
            if (FFMPEG_LOG) {
                FFLOGE("resample:avcodec send packet error");
            }
            releaseTempSource();
            continue;
        }
        pAvFrame = av_frame_alloc();
        ret = avcodec_receive_frame(pAvCodecCtx, pAvFrame);
        if (ret != 0) {
            if (FFMPEG_LOG) {
                FFLOGE("resample:avcodec receive frame error");
            }
            releaseTempSource();
            continue;
        }
        if (pAvFrame->channel_layout == 0 && pAvFrame->channels > 0) {
            pAvFrame->channel_layout = av_get_default_channel_layout(pAvFrame->channels);
        } else if (pAvFrame->channel_layout > 0 && pAvFrame->channels == 0) {
            pAvFrame->channels = av_get_channel_layout_nb_channels(pAvFrame->channel_layout);
        }
        pSwrCtx = swr_alloc_set_opts(NULL,
                                     AV_CH_LAYOUT_STEREO,
                                     AV_SAMPLE_FMT_S16,
                                     pAvFrame->sample_rate,
                                     pAvFrame->channel_layout,
                                     static_cast<AVSampleFormat>(pAvFrame->format),
                                     pAvFrame->sample_rate,
                                     NULL, NULL);
        if (pSwrCtx == nullptr || swr_init(pSwrCtx)) {
            if (FFMPEG_LOG) {
                FFLOGE("resample:SwrContext init error");
            }
            releaseTempSource();
            continue;
        }
        int nbs = swr_convert(pSwrCtx, &out_buffer, pAvFrame->nb_samples,
                              (const uint8_t **) (pAvFrame->data), pAvFrame->nb_samples);
        int channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        resample_size = nbs * channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        if (FFMPEG_LOG) {
            FFLOGI("resample:resample_size is %d", resample_size);
        }
        releaseTempSource();
        break;
    }
    return resample_size;
}

void ZedAudio::pause(bool is_pause) {
    if (is_pause) {
        if (playPlay != nullptr) {
            (*playPlay)->SetPlayState(playPlay, SL_PLAYSTATE_PAUSED);
            cCallJava->callOnPause(CTHREADTYPE_CHILD, true);
        }
    } else {
        if (playPlay != nullptr) {
            (*playPlay)->SetPlayState(playPlay, SL_PLAYSTATE_PLAYING);
            cCallJava->callOnPause(CTHREADTYPE_CHILD, false);
        }
    }
}

void ZedAudio::stop() {
    if (playPlay != nullptr) {
        (*playPlay)->SetPlayState(playPlay, SL_PLAYSTATE_STOPPED);
    }
}

void ZedAudio::releaseTempSource() {
    if (pAvPacket != nullptr) {
        av_packet_free(&pAvPacket);
        av_free(pAvPacket);
        pAvPacket = nullptr;
    }
    if (pAvFrame != nullptr) {
        av_frame_free(&pAvFrame);
        av_free(pAvFrame);
        pAvFrame = nullptr;
    }
    if (pSwrCtx != nullptr) {
        swr_free(&pSwrCtx);
        av_free(pSwrCtx);
        pSwrCtx = nullptr;
    }
}

void ZedAudio::release() {
    if (zedQueue != nullptr) {
        delete (zedQueue);
        zedQueue = nullptr;
    }
    if (playObj != nullptr) {
        (*playObj)->Destroy(playObj);
        playObj = nullptr;
        playPlay = nullptr;
        androidSimpleBufferQueue = nullptr;
    }
    if (mixoutObj != nullptr) {
        (*mixoutObj)->Destroy(mixoutObj);
        mixoutObj = nullptr;
        mixoutEnvironmentalReverb = nullptr;
    }
    if (engineObj != nullptr) {
        (*engineObj)->Destroy(engineObj);
        engineObj = nullptr;
        engineEngine = nullptr;
    }
    if (out_buffer != nullptr) {
        free(out_buffer);
        out_buffer = nullptr;
    }
    if (pAvCodecCtx != nullptr) {
        avcodec_close(pAvCodecCtx);
        avcodec_free_context(&pAvCodecCtx);
        pAvCodecCtx = nullptr;
    }
    if (zedStatus != nullptr) {
        zedStatus = nullptr;
    }
    if (cCallJava != nullptr) {
        cCallJava = nullptr;
    }
}

ZedAudio::~ZedAudio() {

}
