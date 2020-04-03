//
// Created by sen.wan on 2020/3/31.
//

#ifndef ZEDPLAYER_ZEDSTATUS_H
#define ZEDPLAYER_ZEDSTATUS_H


class ZedStatus {
public:
    bool load;
    bool seeking;
    bool exit;
public:
    ZedStatus();
    ~ZedStatus();
};


#endif //ZEDPLAYER_ZEDSTATUS_H
