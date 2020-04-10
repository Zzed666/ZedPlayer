package com.github.zedmediaplayerlib.audio

import com.github.zedmediaplayerlib.audio.listener.*

class ZedAudioPlayer {
    private var onLoadListener: OnLoadListener? = null
    private var onPreparedListener: OnPreparedListener? = null
    private var onPauseListener: OnPauseListener? = null
    private var onSeekListener: OnSeekListener? = null
    private var onStopListener: OnStopListener? = null
    private var onPlayTimeListener: OnPlayTimeListener? = null
    private var onDBListener: OnDBListener? = null
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
    private external fun n_start()
    private external fun n_pause(pause: Boolean)
    private external fun n_seek(seekTime: Int)
    private external fun n_volume(volumePercent: Int)
    private external fun n_stop(skipNext: Boolean, nextMediaPath: String)
    private external fun n_duration(): Int
    private external fun n_mute(muteChannle: Int)
    private external fun n_speed(speed: Float)
    private external fun n_pitch(pitch: Float)

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

    /**-------------------------------------------start---------------------------------------*/
    fun start() {
        Thread(Runnable {
            n_start()
        }).start()
    }
    /**-------------------------------------------start---------------------------------------*/

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
            n_stop(false, "")
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

    fun getDuration(): Int {
        return n_duration()
    }

    fun cCallPlayTimeBack(totalTime: Int, currentTime: Int) {
        onPlayTimeListener?.onPlayTime(totalTime, currentTime)
    }
    /**-------------------------------------------play time---------------------------------------*/

    /**-------------------------------------------db---------------------------------------*/
    fun setOnDBListener(onDBListener: OnDBListener) {
        this.onDBListener = onDBListener
    }

    fun cCallDBBack(db: Int) {
        onDBListener?.onDB(db)
    }
    /**-------------------------------------------db---------------------------------------*/

    /**-------------------------------------------volume---------------------------------------*/
    fun volume(volumePercent: Int) {
        Thread(Runnable {
            n_volume(volumePercent)
        }).start()
    }
    /**-------------------------------------------volume---------------------------------------*/

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
//        stop()
        onCompleteListener?.onComplete()
    }
    /**-------------------------------------------complete---------------------------------------*/

    /**-------------------------------------------next---------------------------------------*/
    fun next(nextMediaPath: String) {
        Thread(Runnable {
            n_stop(true, nextMediaPath)
        }).start()
    }

    fun cCallNextBack(nextMediaPath: String) {
        prepared(nextMediaPath)
    }
    /**-------------------------------------------next---------------------------------------*/

    /**-------------------------------------------mute---------------------------------------*/
    fun mute(muteChannle: Int) {
        Thread(Runnable {
            n_mute(muteChannle)
        }).start()
    }
    /**-------------------------------------------mute---------------------------------------*/

    /**-------------------------------------------speed---------------------------------------*/
    fun speed(speed: Float) {
        Thread(Runnable {
            n_speed(speed)
        }).start()
    }
    /**-------------------------------------------speed---------------------------------------*/

    /**-------------------------------------------pitch---------------------------------------*/
    fun pitch(pitch: Float) {
        Thread(Runnable {
            n_pitch(pitch)
        }).start()
    }
    /**-------------------------------------------pitch---------------------------------------*/
}