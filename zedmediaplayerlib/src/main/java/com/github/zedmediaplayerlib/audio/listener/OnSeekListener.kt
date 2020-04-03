package com.github.zedmediaplayerlib.audio.listener

interface OnSeekListener {
    fun onSeek(seekTime: Int,totalTime: Int)
}