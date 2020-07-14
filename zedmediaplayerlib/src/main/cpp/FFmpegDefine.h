//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_FFMPEGDEFINE_H
#define ZEDPLAYER_FFMPEGDEFINE_H

#include "JniDefine.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

#endif //ZEDPLAYER_FFMPEGDEFINE_H
