package com.github.zedmediaplayerlib.listener

interface OnRenderYUVListener {
    fun onRenderYUV(width: Int, height: Int, yArray: ByteArray, uArray: ByteArray, vArray: ByteArray)
}