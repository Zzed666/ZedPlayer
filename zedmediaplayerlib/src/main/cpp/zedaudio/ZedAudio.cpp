//
// Created by sen.wan on 2020/3/31.
//

#include "ZedAudio.h"

ZedAudio::ZedAudio(ZedStatus *zedStatus, CCallJava *cCallJava, int sample_rate) {
    this->zedStatus = zedStatus;
    this->cCallJava = cCallJava;
    this->sample_rate = sample_rate;
    zedQueue = new ZedQueue(zedStatus);
    out_buffer = (uint8_t *) (av_malloc(sample_rate * 2 * 2));

    soundTouch = new SoundTouch();
    //soundtouch设置采样率
    soundTouch->setSampleRate(sample_rate);
    //soundtouch设置声道数
    soundTouch->setChannels(2);
    //给soundtouch samplebuffer分配空间
    sound_touch_buffer_16bit = (SAMPLETYPE *) (av_malloc(sample_rate * 2 * 2));
    //soundtouch设置初始的speed和pitch
    soundTouch->setTempo(speed_init);
    soundTouch->setPitch(pitch_init);
}

void *playThreadCallBack(void *data) {
    auto *zedAudio = (ZedAudio *) data;
    zedAudio->prepareOpenSELS();
    pthread_exit(&zedAudio->play_thread);
}

void *splitPcmCallBack(void *data) {
    auto *zedAudio = (ZedAudio *) data;
    if (zedAudio != nullptr) {
        zedAudio->splitPcmBuffer();
    }
    pthread_exit(&zedAudio->split_pcm_thread);
}

void openSLESBufferQueueCallBack(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    auto *zedAudio = (ZedAudio *) context;
    if (zedAudio != nullptr) {
        int size = zedAudio->getSoundTouchData();
        zedAudio->clock_time += size / (zedAudio->sample_rate * 2 * 2);
        if (zedAudio->clock_time - zedAudio->last_time >= 0.1) {
            zedAudio->last_time = zedAudio->clock_time;
        }
        if (zedAudio->cCallJava != nullptr) {
            zedAudio->cCallJava->callOnPlayTime(CTHREADTYPE_CHILD, zedAudio->total_duration,
                                                zedAudio->clock_time);
            zedAudio->cCallJava->callOnDB(CTHREADTYPE_CHILD, zedAudio->getDB(
                    reinterpret_cast<char *>(zedAudio->sound_touch_buffer_16bit), size * 2 * 2));
        }
        zedAudio->zedPcmBufferQueue->putPcmBuffer(zedAudio->sound_touch_buffer_16bit, size * 2 * 2);
        //将重采样后的数据压入OpenSLES中的播放队列中
        if (size > 0) {
            (*zedAudio->androidSimpleBufferQueue)->Enqueue(zedAudio->androidSimpleBufferQueue,
                                                           zedAudio->sound_touch_buffer_16bit,
                                                           size * 2 * 2);
        }
        //裁剪
        if (zedAudio->is_allow_cut_pcm) {
            if (zedAudio->clock_time > zedAudio->end_cut_time) {
                if (zedAudio->zedStatus != nullptr) {
                    zedAudio->zedStatus->exit = true;
                }
            }
        }
    }
}

void ZedAudio::play() {
    pthread_create(&play_thread, nullptr, playThreadCallBack, this);
    if (is_allow_split_pcm) {
        pthread_create(&split_pcm_thread, nullptr, splitPcmCallBack, this);
    }
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
            static_cast<SLuint32>(getCurrentSampleRate(sample_rate)),
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
    //解决不同采样率变调卡顿->SL_IID_PLAYBACKRATE
    const SLInterfaceID playItfs[4] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO,
                                       SL_IID_PLAYBACKRATE};
    const SLboolean playBools[4] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
                                    SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine,
                                       &playObj,
                                       &pAudioSrc,
                                       &pAudioSnk,
                                       3,
                                       playItfs,
                                       playBools);
    (*playObj)->Realize(playObj, SL_BOOLEAN_FALSE);
    (*playObj)->GetInterface(playObj, SL_IID_PLAY, &playPlay);
    //设置音量
    (*playObj)->GetInterface(playObj, SL_IID_VOLUME, &volumeVolume);
    volume(volume_init);
    //设置声道
    (*playObj)->GetInterface(playObj, SL_IID_MUTESOLO, &muteMute);
    mute(mute_init);
    //设置缓冲队列和回调函数
    (*playObj)->GetInterface(playObj, SL_IID_BUFFERQUEUE, &androidSimpleBufferQueue);
    (*androidSimpleBufferQueue)->RegisterCallback(androidSimpleBufferQueue,
                                                  &openSLESBufferQueueCallBack, this);
    //设置播放状态
    (*playPlay)->SetPlayState(playPlay, SL_PLAYSTATE_PLAYING);
    //启用回调函数
    openSLESBufferQueueCallBack(androidSimpleBufferQueue, this);
}

int ZedAudio::getSoundTouchData() {
    while (zedStatus != nullptr && !zedStatus->exit) {
        /**
             * FFmpge解码出来的pcm数据是8bit的(uint8),而SoundTouch中最低的是16bit的(16 bit integer samples),所以我们需要将8bit转换成16bit再交给SoundTouch处理
             * 由于PCM数据在内存中是顺序排列的,所以先将第一个8bit的数据复制到16bit内存的前8位
             * 然后后8bit的数据复制到16bit内存中的后8bit,就能把16bit的内存填满
             * 然后循环复制。(循环次数为data_size/2 + 1,+1是因为有除不尽的情况)
             * 图解见res/drawable-hdpi/bit8_revert_bit16.jpg
             * */
        int original_resample_size = 0;
        int sound_touch_sample_size = 0;
        sound_touch_buffer_8bit = nullptr;

        if (sound_touch_sample_finished) {
            sound_touch_sample_finished = false;
            original_resample_size = resample();
            if (original_resample_size > 0) {
                for (int i = 0; i < original_resample_size; i++) {
                    sound_touch_buffer_16bit[i] = (sound_touch_buffer_8bit[i * 2] |
                                                   (sound_touch_buffer_8bit[i * 2 + 1] << 8));
                }
                if (soundTouch != nullptr) {
                    soundTouch->putSamples(sound_touch_buffer_16bit, resample_nbs);
                    sound_touch_sample_size = soundTouch->receiveSamples(sound_touch_buffer_16bit,
                                                                         original_resample_size /
                                                                         2 /
                                                                         2);
                }
            } else {
                if (soundTouch != nullptr) {
                    soundTouch->flush();
                }
            }
        }
        if (sound_touch_sample_size == 0) {
            sound_touch_sample_finished = true;
            continue;
        } else {
            if (sound_touch_buffer_8bit == nullptr) {
                sound_touch_sample_size = soundTouch->receiveSamples(sound_touch_buffer_16bit,
                                                                     original_resample_size / 2 /
                                                                     2);
                if (sound_touch_sample_size == 0) {
                    sound_touch_sample_finished = true;
                    continue;
                }
            }
            return sound_touch_sample_size;
        }
    }
    return 0;
}

int ZedAudio::getDB(char *pcm_data, size_t data_size) {
    /**
     * 将一段时间的分贝值求和再求平均值得到平均的分贝值
     * 16bit=2byte=2char，在c语言中是用short int表示，将16bit提取前2个char转化成short int就可以了，然后相加求平均值
     * */
    int db = 0;
    short int previous_value = 0;
    double sum = 0;
    for (int i = 0; i < data_size; i += 2) {
        memcpy(&previous_value, pcm_data + i, 2);
        sum += abs(previous_value);
    }
    if (sum > 0) {
        sum = sum / (data_size / 2);
        db = (int) 20.0 * log10(sum);
    }
    //todo db < 0
    db = db < 0 ? 0 : db;
    return db;
}

int ZedAudio::resample() {
    int ret = -1;
    int resample_size = 0;
    while (zedStatus != nullptr && !zedStatus->exit) {
        if (zedQueue != nullptr && zedQueue->getPacketSize() == 0) {
            if (!zedStatus->load) {
                zedStatus->load = true;
                cCallJava->callOnLoad(CTHREADTYPE_CHILD, true);
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (zedStatus->load) {
                zedStatus->load = false;
                cCallJava->callOnLoad(CTHREADTYPE_CHILD, false);
            }
        }
        //.ape 文件1个AVPacket包含多个AVframe,所以加一个flag循环遍历,当read frame != 0的时候，才说明AVPacket里的AVFrame已经读取完了,再去获取新的AVPacket
        if (isavpacketfinished) {
            pAvPacket = av_packet_alloc();
            if (zedQueue != nullptr && zedQueue->getPackets(pAvPacket) != 0) {
                releaseTempSource(true);
                continue;
            }
            ret = avcodec_send_packet(pAvCodecCtx, pAvPacket);
            if (ret != 0) {
                if (FFMPEG_LOG) {
                    FFLOGE("resample:avcodec send packet error");
                }
                releaseTempSource(true);
                continue;
            }
        }
        pAvFrame = av_frame_alloc();
        ret = avcodec_receive_frame(pAvCodecCtx, pAvFrame);
        if (ret != 0) {
            if (FFMPEG_LOG) {
                FFLOGE("resample:avcodec receive frame error");
            }
            isavpacketfinished = true;
            releaseTempSource(true);
            continue;
        }
        isavpacketfinished = false;
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
            isavpacketfinished = true;
            releaseTempSource(true);
            continue;
        }
        resample_nbs = swr_convert(pSwrCtx, &out_buffer, pAvFrame->nb_samples,
                                   (const uint8_t **) (pAvFrame->data), pAvFrame->nb_samples);
        int channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        resample_size = resample_nbs * channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        if (FFMPEG_LOG) {
            FFLOGI("resample:resample_size is %d", resample_size);
        }
        now_time = pAvFrame->pts * av_q2d(audio_time_base);
        if (now_time < clock_time) {
            now_time = clock_time;
        }
        clock_time = now_time;
        sound_touch_buffer_8bit = out_buffer;
        releaseTempSource(false);
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

void ZedAudio::volume(int percent) {
    volume_init = percent;
    if (volumeVolume != nullptr) {
        if (FFMPEG_LOG) {
            FFLOGI("audio volume is : %d", percent);
        }
        if (percent > 30) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -20);
        } else if (percent > 25) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -22);
        } else if (percent > 20) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -25);
        } else if (percent > 15) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -28);
        } else if (percent > 10) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -30);
        } else if (percent > 5) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -34);
        } else if (percent > 3) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -37);
        } else if (percent > 0) {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -40);
        } else {
            (*volumeVolume)->SetVolumeLevel(volumeVolume, (100 - percent) * -100);
        }
    }
}

void ZedAudio::mute(int mute_channel) {
    mute_init = mute_channel;
    if (muteMute != nullptr) {
        if (FFMPEG_LOG) {
            FFLOGI("audio mute_channel is : %d", mute_channel);
        }
        if (mute_channel == 0) {//right
            (*muteMute)->SetChannelMute(muteMute,
                                        0,
                                        true);
            (*muteMute)->SetChannelMute(muteMute,
                                        1,
                                        false);
        } else if (mute_channel == 1) {//left
            (*muteMute)->SetChannelMute(muteMute,
                                        0,
                                        false);
            (*muteMute)->SetChannelMute(muteMute,
                                        1,
                                        true);
        } else if (mute_channel == 2) {//center
            (*muteMute)->SetChannelMute(muteMute,
                                        0,
                                        false);
            (*muteMute)->SetChannelMute(muteMute,
                                        1,
                                        false);
        }
    }
}

void ZedAudio::speed(float audio_speed) {
    speed_init = audio_speed;
    if (soundTouch != nullptr) {
        if (FFMPEG_LOG) {
            FFLOGI("resample:audio_speed is %d", audio_speed);
        }
        soundTouch->setTempo(audio_speed);
    }
}

void ZedAudio::pitch(float audio_pitch) {
    pitch_init = audio_pitch;
    if (soundTouch != nullptr) {
        if (FFMPEG_LOG) {
            FFLOGI("resample:audio_pitch is %d", audio_pitch);
        }
        soundTouch->setPitch(audio_pitch);
    }
}

void ZedAudio::setSplitPcmBuffer(bool split) {
    is_allow_split_pcm = split;
}

void ZedAudio::splitPcmBuffer() {
    zedPcmBufferQueue = new ZedBufferQueue(zedStatus);
    while (zedStatus != nullptr && !zedStatus->exit) {
        ZedPcmBean *zedPcmBean = nullptr;
        zedPcmBufferQueue->getPcmBuffer(&zedPcmBean);
        if (zedPcmBean == nullptr) {
            continue;
        } else {
            if (zedPcmBean->pcmBufferSize <= default_pcm_buffer_size) {
                record(zedPcmBean->pcmBuffer, zedPcmBean->pcmBufferSize);
                cutPcm(zedPcmBean->pcmBuffer, zedPcmBean->pcmBufferSize);
            } else {
                int split_pcm_num =
                        zedPcmBean->pcmBufferSize / default_pcm_buffer_size;
                int split_pcm_surplus_size =
                        zedPcmBean->pcmBufferSize % default_pcm_buffer_size;
                for (int i = 0; i < split_pcm_num; i++) {
                    char *bf = static_cast<char *>(malloc(default_pcm_buffer_size));
                    memcpy(bf, zedPcmBean->pcmBuffer + i * default_pcm_buffer_size,
                           default_pcm_buffer_size);
                    record(bf, default_pcm_buffer_size);
                    cutPcm(bf, default_pcm_buffer_size);
                    free(bf);
                    bf = nullptr;
                }
                if (split_pcm_surplus_size > 0) {
                    char *bf = static_cast<char *>(malloc(split_pcm_surplus_size));
                    memcpy(bf, zedPcmBean->pcmBuffer +
                               split_pcm_num * default_pcm_buffer_size,
                           split_pcm_surplus_size);
                    record(bf, split_pcm_surplus_size);
                    cutPcm(bf, split_pcm_surplus_size);
                    free(bf);
                    bf = nullptr;
                }
            }
        }
        delete (zedPcmBean);
        zedPcmBean = nullptr;
    }
}

void ZedAudio::setRecord(bool audio_record) {
    isrecord = audio_record;
}

void ZedAudio::record(char *buffer, int bufferSize) {
    if (isrecord) {
        cCallJava->callOnPcmToAAC(CTHREADTYPE_CHILD,
                                  buffer,
                                  bufferSize);
    }
}

void ZedAudio::setCutPcm(float start_time, float end_time, bool show_pcm) {
    is_allow_cut_pcm = true;
    is_allow_show_pcm = show_pcm;
    this->start_cut_time = start_time;
    this->end_cut_time = end_time;
}

void ZedAudio::cutPcm(char *buffer, int bufferSize) {
    //裁剪
    if (is_allow_cut_pcm && is_allow_show_pcm && cCallJava != nullptr) {
//        if (clock_time > end_cut_time) {
//            if (zedStatus != nullptr) {
//                zedStatus->exit = true;
//            }
//        }
        cCallJava->callOnPcmInfo(CTHREADTYPE_CHILD,
                                 buffer,
                                 bufferSize);
    }
}

int ZedAudio::getCurrentSampleRate(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void ZedAudio::releaseTempSource(bool isReleaseAVPacket) {
    if (pAvPacket != nullptr && isReleaseAVPacket) {
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
    if (zedPcmBufferQueue != nullptr) {
        zedPcmBufferQueue->noticeThread();
        pthread_join(split_pcm_thread, nullptr);
        zedPcmBufferQueue->release();
        delete(zedPcmBufferQueue);
        zedPcmBufferQueue = nullptr;
    }
    if (zedQueue != nullptr) {
        delete (zedQueue);
        zedQueue = nullptr;
    }
//    if (soundTouch != nullptr) {
//        delete (soundTouch);
//        soundTouch = nullptr;
//    }
    if (playObj != nullptr) {
        (*playObj)->Destroy(playObj);
        playObj = nullptr;
        playPlay = nullptr;
        androidSimpleBufferQueue = nullptr;
        volumeVolume = nullptr;
        muteMute = nullptr;
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
    if (sound_touch_buffer_8bit != nullptr) {
        //sound_touch_buffer_8bit指向out_buffer的内存地址，所以out_buffer free了,sound_touch_buffer_8bit就不能再free了
//        free(sound_touch_buffer_8bit);
        sound_touch_buffer_8bit = nullptr;
    }
    if (sound_touch_buffer_16bit != nullptr) {
        free(sound_touch_buffer_16bit);
        sound_touch_buffer_16bit = nullptr;
    }
    if (soundTouch != nullptr) {
        delete (soundTouch);
        soundTouch = nullptr;
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
