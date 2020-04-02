package com.github.zedmediaplayerlib.audio

import com.github.zedmediaplayerlib.audio.listener.OnLoadListener
import com.github.zedmediaplayerlib.audio.listener.OnPauseListener
import com.github.zedmediaplayerlib.audio.listener.OnPreparedListener

class ZedAudioPlayer {
    private var onLoadListener: OnLoadListener? = null
    private var onPreparedListener: OnPreparedListener? = null
    private var onPauseListener: OnPauseListener? = null

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
    private external fun n_pause(pause: Boolean)

    /**-------------------------------------------load---------------------------------------*/
    fun setOnLoadListener(onLoadListener: OnLoadListener) {
        this.onLoadListener = onLoadListener
    }

    fun cCallLoadBack(load: Boolean) {
        onLoadListener?.onLoad(load)
    }
    /**-------------------------------------------load---------------------------------------*/

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

    /**-------------------------------------------pause or resume---------------------------------------*/
    fun setOnPauseListener(onPauseListener: OnPauseListener) {
        this.onPauseListener = onPauseListener
    }

    fun pause(isPause: Boolean) {
        Thread(Runnable {
            n_pause(isPause)
        }).start()
    }

    fun cCallPauseBack(pause: Boolean) {
        onPauseListener?.onPause(pause)
    }
    /**-------------------------------------------pause or resume---------------------------------------*/
}