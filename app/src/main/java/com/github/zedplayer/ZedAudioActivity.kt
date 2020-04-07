package com.github.zedplayer

import android.annotation.SuppressLint
import android.os.*
import android.util.Log
import android.widget.SeekBar
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.audio.ZedAudioPlayer
import com.github.zedmediaplayerlib.audio.listener.*
import com.github.zedmediaplayerlib.commons.ZedTimeUtil
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.disposables.CompositeDisposable
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_zed_audio.*
import java.io.File

class ZedAudioActivity : AppCompatActivity() {
    private val disposables = CompositeDisposable()
    var position: Int = 0
    var isSeeking: Boolean = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zed_audio)
        initEvents()
    }

    private fun initEvents() {
        val zedAudioPlayer = ZedAudioPlayer()
        zedAudioPlayer.setOnLoadListener(object : OnLoadListener {
            override fun onLoad(load: Boolean) {
                if (load) {
                    Log.i("zzed", "media is loading...")
                } else Log.i("zzed", "media is playing...")
            }
        })
        zedAudioPlayer.setOnPrepareListener(object : OnPreparedListener {
            override fun onPrepared() {
                Log.i("zzed", "media is prepared to play")
                zedAudioPlayer.start()
            }
        })
        zedAudioPlayer.setOnPauseListener(object : OnPauseListener {
            override fun onPause(pause: Boolean) {
                if (pause) {
                    Log.i("zzed", "media is paused...")
                } else Log.i("zzed", "media is resume playing...")
            }
        })
        zedAudioPlayer.setOnSeekListener(object : OnSeekListener {
            override fun onSeek(seekTime: Int, totalTime: Int) {
                Log.i(
                    "zzed",
                    "media seeks to ${ZedTimeUtil.secdsToDateFormat(seekTime, totalTime)}"
                )
            }
        })
        zedAudioPlayer.setOnStopListener(object : OnStopListener {
            override fun onStop() {
                Log.i("zzed", "media is stopped!")
            }
        })
        zedAudioPlayer.setOnPlayTimeListener(object : OnPlayTimeListener {
            @SuppressLint("SetTextI18n")
            override fun onPlayTime(totalTime: Int, currentTime: Int) {
                Log.i(
                    "zzed",
                    "${ZedTimeUtil.secdsToDateFormat(
                        totalTime,
                        totalTime
                    )}/${ZedTimeUtil.secdsToDateFormat(currentTime, totalTime)}"
                )
                runOnUiThread(Runnable {
                    play_time.text = "${ZedTimeUtil.secdsToDateFormat(
                        totalTime,
                        totalTime
                    )}/${ZedTimeUtil.secdsToDateFormat(currentTime, totalTime)}"
                    if (!isSeeking) {
                        seekBar.progress = currentTime * 100 / totalTime
                    }
                })
            }
        })
        zedAudioPlayer.setOnErrorListener(object : OnErrorListener {
            override fun onError(errorCode: Int, errorMsg: String) {
                Log.e("zzed", "media error,error code is $errorCode,error message is $errorMsg!")
            }
        })
        zedAudioPlayer.setOnOnCompleteListener(object : OnCompleteListener {
            override fun onComplete() {
                Log.i("zzed", "media play completely!")
                zedAudioPlayer.stop()
            }
        })
        prepare.setOnClickListener {
            zedAudioPlayer.prepared(
                File(
                    Environment.getExternalStorageDirectory(),
                    "Yasuo.mp3"
                ).absolutePath
            )
        }
        pause.setOnClickListener {
            zedAudioPlayer.pause(true)
        }
        resume.setOnClickListener {
            zedAudioPlayer.pause(false)
        }
//        seek.setOnClickListener {
//            zedAudioPlayer.seek(270)
//        }
        stop.setOnClickListener {
            zedAudioPlayer.stop()
        }
        next.setOnClickListener {
            zedAudioPlayer.next(
                File(
                    Environment.getExternalStorageDirectory(),
                    "告白の夜.mp3"
                ).absolutePath
            )
        }
        seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                disposables.add(Observable.just(zedAudioPlayer.getDuration())
                    .subscribeOn(Schedulers.io())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe { totalDuration ->
                        if (totalDuration >= 0) {
                            position = totalDuration * progress / 100
                        } else Toast.makeText(
                            this@ZedAudioActivity,
                            "media source isn't prepare!",
                            Toast.LENGTH_SHORT
                        ).show()
                    })
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                isSeeking = true
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                isSeeking = false
                zedAudioPlayer.seek(position)
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        disposables.clear()
    }
}