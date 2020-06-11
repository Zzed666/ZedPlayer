package com.github.zedplayer

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.audio.ZedAudioPlayer
import com.github.zedmediaplayerlib.audio.listener.*
import com.github.zedmediaplayerlib.commons.ZedMediaStatus
import com.github.zedmediaplayerlib.commons.ZedTimeUtil
import kotlinx.android.synthetic.main.activity_zed_cut_audio_to_pcm.*

class ZedCutAudioToPcmActivity : AppCompatActivity(){
    private lateinit var mediaPath : String
    private var zedAudioPlayer : ZedAudioPlayer? = null
    private var zedAudioStatus: Int = ZedMediaStatus.STATUS_IDLE.statusValue

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zed_cut_audio_to_pcm)
        initDatas()
        initEvents()
    }

    private fun initDatas() {
        mediaPath = intent?.getStringExtra("media_path") ?: ""
        zedAudioPlayer = intent?.getParcelableExtra("audio_player")
        zedAudioStatus = intent?.getIntExtra("audio_status",ZedMediaStatus.STATUS_IDLE.statusValue) ?: ZedMediaStatus.STATUS_IDLE.statusValue
    }

    private fun initEvents() {
        zedAudioPlayer?.setOnPrepareListener(object : OnPreparedListener {
            override fun onPrepared() {
                Log.i("zzed", "cut:media is prepared to play")
                zedAudioStatus = ZedMediaStatus.STATUS_PREPARED.statusValue
                zedAudioPlayer?.cutAudioToPcm(30.0f,40.0f,true)
            }
        })
        zedAudioPlayer?.setOnSeekListener(object : OnSeekListener {
            override fun onSeek(seekTime: Int, totalTime: Int) {
                Log.i(
                    "zzed",
                    "cut:media seeks to ${ZedTimeUtil.secdsToDateFormat(seekTime, totalTime)}"
                )
            }
        })
        zedAudioPlayer?.setOnPlayTimeListener(object : OnPlayTimeListener {
            @SuppressLint("SetTextI18n")
            override fun onPlayTime(totalTime: Int, currentTime: Int) {
                Log.i(
                    "zzed",
                    "${ZedTimeUtil.secdsToDateFormat(
                        totalTime,
                        totalTime
                    )}/${ZedTimeUtil.secdsToDateFormat(currentTime, totalTime)}"
                )
            }
        })
        zedAudioPlayer?.setOnPcmInfoListener(object : OnPcmInfoListener {
            override fun onPcmSmapleRate(pcmSampleRate: Int) {
                Log.i("zzed","cut:media pcm sample rate is : $pcmSampleRate")
            }

            override fun onPcmInfo(pcmBuffer: ByteArray, pcmBufferSize: Int) {
                Log.i("zzed","cut:media pcm buffer size is : $pcmBufferSize")
            }
        })
        zedAudioPlayer?.setOnOnCompleteListener(object : OnCompleteListener {
            override fun onComplete() {
                Log.i("zzed", "cut:media play completely!")
                zedAudioPlayer?.stop()
            }
        })
        zedAudioPlayer?.setOnStopListener(object : OnStopListener {
            override fun onStop() {
                Log.i("zzed", "cut:media is stopped!")
                zedAudioStatus = ZedMediaStatus.STATUS_IDLE.statusValue
            }
        })
        btn_cut_pcm.setOnClickListener {
            if (zedAudioStatus == ZedMediaStatus.STATUS_IDLE.statusValue) {
                zedAudioPlayer?.prepared(mediaPath)
            }
        }
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onDestroy() {
        super.onDestroy()
    }
}