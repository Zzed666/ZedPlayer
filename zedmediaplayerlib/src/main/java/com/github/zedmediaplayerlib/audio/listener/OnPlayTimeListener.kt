package com.github.zedmediaplayerlib.audio.listener

interface OnPlayTimeListener {
    fun onPlayTime(totalTime: Int, currentTime: Int)
}