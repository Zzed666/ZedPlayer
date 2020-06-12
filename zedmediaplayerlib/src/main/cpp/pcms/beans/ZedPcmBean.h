//
// Created by sen.wan on 2020/6/11.
//

#ifndef ZEDPLAYER_ZEDPCMBEAN_H
#define ZEDPLAYER_ZEDPCMBEAN_H

#include <jni.h>
#include <string>
#include "../../soundtouch/include/SoundTouch.h"

using namespace soundtouch;

class ZedPcmBean {
public:
    char *pcmBuffer = nullptr;
    int pcmBufferSize;
public:
    ZedPcmBean(SAMPLETYPE *buffer, int size);

    ~ZedPcmBean();
};

#endif //ZEDPLAYER_ZEDPCMBEAN_H
