package com.github.zedmediaplayerlib.listener

interface OnSeekListener {
    fun onSeek(seekTime: Int,totalTime: Int)
}