package com.github.zedmediaplayerlib.audio

import com.github.zedmediaplayerlib.audio.listener.OnLoadListener
import com.github.zedmediaplayerlib.audio.listener.OnPreparedListener

class ZedAudioPlayer {
    private var onLoadListener: OnLoadListener? = null
    private var onPreparedListener: OnPreparedListener? = null

    companion object {
        init {
            System.loadLibrary("jni-lib")
            System.loadLibrary("avcodec")
            System.loadLibrary("avdevice")
            System.loadLibrary("avfilter")
            System.loadLibrary("avformat")
            System.loadLibrary("avutil")
            System.loadLibrary("swresample")
            System.loadLibrary("swscale")
        }
    }

    private external fun n_prepared(path: String)

    /**-------------------------------------------load---------------------------------------*/
    fun setOnLoadListener(onLoadListener: OnLoadListener) {
        this.onLoadListener = onLoadListener
    }

    fun cCallLoadBack(load: Boolean) {
        onLoadListener?.onLoad(load)
    }
    /**-------------------------------------------prepared---------------------------------------*/

    /**-------------------------------------------prepared---------------------------------------*/
    fun setOnPrepareListener(onPreparedListener: OnPreparedListener) {
        this.onPreparedListener = onPreparedListener
    }

    fun prepared(mediaPath: String) {
        Thread(Runnable {
            n_prepared(mediaPath)
        }).start()
    }

    fun cCallPreparedBack() {
        onPreparedListener?.onPrepared()
    }
    /**-------------------------------------------prepared---------------------------------------*/
}