package com.github.zedmediaplayerlib.audio.listener

interface OnPcmInfoListener {
    fun onPcmSmapleRate(pcmSampleRate: Int)
    fun onPcmInfo(pcmBuffer: ByteArray, pcmBufferSize: Int)
}