package com.github.zedmediaplayerlib.commons

enum class ZedMuteEnum constructor(val muteName: String, val muteValue: Int = 2) {
    MUTE_RIGHT("RIGHT", 0),
    MUTE_LEFT("LEFT", 1),
    MUTE_CENTER("CENTER", 2)
}
