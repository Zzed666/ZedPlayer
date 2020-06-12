//
// Created by sen.wan on 2020/6/11.
//

#include "ZedPcmBean.h"

ZedPcmBean::ZedPcmBean(SAMPLETYPE *buffer, int size) {
    this->pcmBuffer = static_cast<char *>(malloc(size));
    this->pcmBufferSize = size;
    memcpy(this->pcmBuffer,buffer,size);
}

ZedPcmBean::~ZedPcmBean() {
    free(pcmBuffer);
    pcmBuffer = nullptr;
}

