package com.github.zedmediaplayerlib.audio.listener

interface OnErrorListener {
    fun onError(errorCode: Int, errorMsg: String)
}