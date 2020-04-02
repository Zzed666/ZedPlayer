//
// Created by sen.wan on 2020/3/31.
//

#include "ZedAudio.h"

ZedAudio::ZedAudio(ZedStatus *zedStatus) {
    this->zedStatus = zedStatus;
    zedQueue = new ZedQueue(zedStatus);
    out_buffer = (uint8_t *) (av_malloc(44100 * 2 * 2));
}

void *playThreadCallBack(void *data) {
    auto *zedAuzio = (ZedAudio *) data;
    zedAuzio->resample();
    pthread_exit(&zedAuzio->play_thread);
}

void ZedAudio::play() {
    pthread_create(&play_thread, nullptr, playThreadCallBack, this);
}

int ZedAudio::resample() {
    int ret = -1;
    int resample_size = 0;
    while (zedStatus != nullptr && !zedStatus->exit) {
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
            FFLOGI("resample:resample_size is %d",resample_size);
        }
        releaseTempSource();
        break;
    }
    return resample_size;
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

ZedAudio::~ZedAudio() {

}
