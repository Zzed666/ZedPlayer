package com.github.zedmediaplayerlib.commons

enum class ZedMediaStatus constructor(val statusName: String, val statusValue: Int = 100){
    STATUS_IDLE("IDLE", 100),
    STATUS_LOADING("LOADING", 101),
    STATUS_PREPARED("PREPARED", 102),
    STATUS_PLAY("PLAY", 103),
    STATUS_PAUSE("PAUSE", 104),
    STATUS_STOPING("STOPING", 105)
}