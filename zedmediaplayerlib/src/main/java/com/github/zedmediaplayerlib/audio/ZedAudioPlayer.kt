package com.github.zedmediaplayerlib.audio

import com.github.zedmediaplayerlib.audio.listener.*

class ZedAudioPlayer {
    private var onLoadListener: OnLoadListener? = null
    private var onPreparedListener: OnPreparedListener? = null
    private var onPauseListener: OnPauseListener? = null
    private var onSeekListener: OnSeekListener? = null
    private var onStopListener: OnStopListener? = null
    private var onPlayTimeListener: OnPlayTimeListener? = null
    private var onErrorListener: OnErrorListener? = null
    private var onCompleteListener: OnCompleteListener? = null

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
    private external fun n_seek(seekTime: Int)
    private external fun n_stop()

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

    /**-------------------------------------------seek---------------------------------------*/
    fun setOnSeekListener(onSeekListener: OnSeekListener) {
        this.onSeekListener = onSeekListener
    }

    fun seek(seekTime: Int) {
        Thread(Runnable {
            n_seek(seekTime)
        }).start()
    }

    fun cCallSeekBack(totalTime: Int, seekTime: Int) {
        onSeekListener?.onSeek(seekTime, totalTime)
    }
    /**-------------------------------------------seek---------------------------------------*/

    /**-------------------------------------------stop---------------------------------------*/
    fun setOnStopListener(onStopListener: OnStopListener) {
        this.onStopListener = onStopListener
    }

    fun stop() {
        Thread(Runnable {
            n_stop()
        }).start()
    }

    fun cCallStopBack() {
        onStopListener?.onStop()
    }
    /**-------------------------------------------stop---------------------------------------*/

    /**-------------------------------------------play time---------------------------------------*/
    fun setOnPlayTimeListener(onPlayTimeListener: OnPlayTimeListener) {
        this.onPlayTimeListener = onPlayTimeListener
    }

    fun cCallPlayTimeBack(totalTime: Int, currentTime: Int) {
        onPlayTimeListener?.onPlayTime(totalTime, currentTime)
    }
    /**-------------------------------------------play time---------------------------------------*/

    /**-------------------------------------------error---------------------------------------*/
    fun setOnErrorListener(onErrorListener: OnErrorListener) {
        this.onErrorListener = onErrorListener
    }

    fun cCallErrorBack(errorCode: Int, errorMsg: String) {
        stop()
        onErrorListener?.onError(errorCode, errorMsg)
    }
    /**-------------------------------------------error---------------------------------------*/

    /**-------------------------------------------complete---------------------------------------*/
    fun setOnOnCompleteListener(onCompleteListener: OnCompleteListener) {
        this.onCompleteListener = onCompleteListener
    }

    fun cCallCompleteBack() {
        onCompleteListener?.onComplete()
    }
    /**-------------------------------------------complete---------------------------------------*/
}