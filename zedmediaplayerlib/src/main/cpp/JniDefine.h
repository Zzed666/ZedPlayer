//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_JNIDEFINE_H
#define ZEDPLAYER_JNIDEFINE_H

#include "android/log.h"

#define FFMPEG_LOG true

#define FFLOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"ffmpeg",FORMAT,##__VA_ARGS__);
#define FFLOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"ffmpeg",FORMAT,##__VA_ARGS__);
#define FFLOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"ffmpeg",FORMAT,##__VA_ARGS__);

#endif //ZEDPLAYER_JNIDEFINE_H
