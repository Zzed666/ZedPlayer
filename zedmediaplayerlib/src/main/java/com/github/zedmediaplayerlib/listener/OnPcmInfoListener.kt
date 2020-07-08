package com.github.zedmediaplayerlib.listener

interface OnPcmInfoListener {
    fun onPcmSmapleRate(pcmSampleRate: Int)
    fun onPcmInfo(pcmBuffer: ByteArray, pcmBufferSize: Int)
}