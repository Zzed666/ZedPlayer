package com.github.zedmediaplayerlib.media

import android.annotation.SuppressLint
import android.os.Build
import android.os.Parcelable
import android.util.Log
import androidx.annotation.RequiresApi
import com.github.zedmediaplayerlib.listener.*
import com.github.zedmediaplayerlib.commons.ZedMediaHelper
import kotlinx.android.parcel.Parcelize
import java.io.File

@SuppressLint("ParcelCreator")
@Parcelize
class ZedMediaPlayer : Parcelable {
    private var onLoadListener: OnLoadListener? = null
    private var onPreparedListener: OnPreparedListener? = null
    private var onPauseListener: OnPauseListener? = null
    private var onSeekListener: OnSeekListener? = null
    private var onNextListener: OnNextListener? = null
    private var onStopListener: OnStopListener? = null
    private var onPlayTimeListener: OnPlayTimeListener? = null
    private var onDBListener: OnDBListener? = null
    private var onErrorListener: OnErrorListener? = null
    private var onCompleteListener: OnCompleteListener? = null
    private var onRecordListener: OnRecordListener? = null
    private var onPcmInfoListener: OnPcmInfoListener? = null

    private var zedMediaHelper: ZedMediaHelper? = null

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
    private external fun n_samplerate(): Int
    private external fun n_mute(muteChannle: Int)
    private external fun n_speed(speed: Float)
    private external fun n_pitch(pitch: Float)
    private external fun n_record(record: Boolean)
    private external fun n_setsplitpcm(allowSplit: Boolean)
    private external fun n_cutpcm(startTime: Float, endTime: Float, showPcm: Boolean): Boolean

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
        n_prepared(mediaPath)
    }

    fun cCallPreparedBack() {
        onPreparedListener?.onPrepared()
    }
    /**-------------------------------------------prepared---------------------------------------*/

    /**-------------------------------------------start---------------------------------------*/
    fun start() {
        n_start()
    }
    /**-------------------------------------------start---------------------------------------*/

    /**-------------------------------------------pause or resume---------------------------------------*/
    fun setOnPauseListener(onPauseListener: OnPauseListener) {
        this.onPauseListener = onPauseListener
    }

    fun pause(isPause: Boolean) {
        n_pause(isPause)
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
//        Thread(Runnable {
        n_seek(seekTime)
//        }).start()
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
        stopRecord()
        n_stop(false, "")
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
        n_stop(true, nextMediaPath)
    }

    fun setOnNextListener(onNextListener: OnNextListener) {
        this.onNextListener = onNextListener
    }

    fun cCallNextBack(nextMediaPath: String) {
//        prepared(nextMediaPath)
        onNextListener?.onNext(nextMediaPath)
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

    /**-------------------------------------------sample rate---------------------------------------*/
    fun getSampleRate(): Int {
        return n_samplerate()
    }
    /**-------------------------------------------sample rate---------------------------------------*/

    /**-------------------------------------------record---------------------------------------*/
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun startRecord(outFile: File) {
        getSampleRate().takeIf { it > 0 }?.let { sampleRate ->
            if (zedMediaHelper == null) {
                zedMediaHelper = ZedMediaHelper(onRecordListener)
            }
            zedMediaHelper?.takeIf { !it.isInitMediaCodec }?.run {
                initMediaCodec(sampleRate, outFile)
                n_record(true)
            }
        }
    }

    fun pauseRecord() {
        n_record(false)
    }

    fun resumeRecord() {
        n_record(true)
    }

    fun stopRecord() {
        zedMediaHelper?.takeIf { it.isInitMediaCodec }?.run {
            n_record(false)
            releaseMediaCodec()
        }
    }

    fun setOnOnRecordListener(onRecordListener: OnRecordListener) {
        this.onRecordListener = onRecordListener
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun cCallPcmToAACBack(srcBuffer: ByteArray, srcBufferSize: Int) {
        zedMediaHelper?.encodePcmToAAC(srcBuffer, srcBufferSize)
    }
    /**-------------------------------------------record---------------------------------------*/

    /**-------------------------------------------cut to pcm---------------------------------------*/
    fun cutAudioToPcm(startTime: Float, endTime: Float, isShowPcm: Boolean) {
        if (n_cutpcm(startTime,endTime,isShowPcm)) {
            onPcmInfoListener?.onPcmSmapleRate(getSampleRate())
            start()
        } else {
            stop()
        }
    }

    fun setOnPcmInfoListener(onPcmInfoListener: OnPcmInfoListener) {
        this.onPcmInfoListener = onPcmInfoListener
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun cCallPcmInfoBack(pcmBuffer: ByteArray, pcmBufferSize: Int) {
        onPcmInfoListener?.onPcmInfo(pcmBuffer, pcmBufferSize)
    }
    /**-------------------------------------------cut to pcm---------------------------------------*/

    /**-------------------------------------------set split pcm---------------------------------------*/
    fun splitPcm(allowSplit: Boolean) {
        n_setsplitpcm(allowSplit)
    }
    /**-------------------------------------------set split pcm---------------------------------------*/

    /**-------------------------------------------render yuv---------------------------------------*/
    fun cCallRenderYUVBack(width: Int, height: Int, yArray: ByteArray, uArray: ByteArray, vArray: ByteArray) {
        Log.e("wszed","cCallRenderYUVBack:width:$width,height:$height")
    }
    /**-------------------------------------------render yuv---------------------------------------*/
}