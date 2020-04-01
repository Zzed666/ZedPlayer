//
// Created by sen.wan on 2020/3/31.
//

#include "ZedAudio.h"

ZedAudio::ZedAudio(ZedStatus *zedStatus) {
    this->zedStatus = zedStatus;
    zedQueue = new ZedQueue(zedStatus);
}

ZedAudio::~ZedAudio() {

}
