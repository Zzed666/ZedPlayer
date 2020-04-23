package com.github.zedplayer

import android.annotation.SuppressLint
import android.os.*
import android.util.Log
import android.widget.SeekBar
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.audio.ZedAudioPlayer
import com.github.zedmediaplayerlib.audio.listener.*
import com.github.zedmediaplayerlib.commons.ZedMuteEnum
import com.github.zedmediaplayerlib.commons.ZedTimeUtil
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.disposables.CompositeDisposable
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_zed_audio.*
import java.io.File

class ZedAudioActivity : AppCompatActivity() {
    var zedAudioPlayer: ZedAudioPlayer? = null
    private val disposables = CompositeDisposable()
    var position: Int = 0
    var isSeekBar: Boolean = false
    var volumeValue = 50
    var muteValue = ZedMuteEnum.MUTE_CENTER.muteValue
    var speedValue = 1.0f
    var pitchValue = 1.0f
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zed_audio)
        initDatas()
        initEvents()
    }

    private fun initDatas() {
        volume_value.text = "音量：$volumeValue%"
        volume_bar.progress = volumeValue
        zedAudioPlayer = ZedAudioPlayer()
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    private fun initEvents() {
        zedAudioPlayer?.setOnLoadListener(object : OnLoadListener {
            override fun onLoad(load: Boolean) {
                if (load) {
                    Log.i("zzed", "media is loading...")
                } else Log.i("zzed", "media is playing...")
            }
        })
        zedAudioPlayer?.setOnPrepareListener(object : OnPreparedListener {
            override fun onPrepared() {
                Log.i("zzed", "media is prepared to play")
                zedAudioPlayer?.speed(speedValue)
                zedAudioPlayer?.pitch(pitchValue)
                zedAudioPlayer?.volume(volumeValue)
                zedAudioPlayer?.mute(muteValue)
                zedAudioPlayer?.start()
            }
        })
        zedAudioPlayer?.setOnPauseListener(object : OnPauseListener {
            override fun onPause(pause: Boolean) {
                if (pause) {
                    Log.i("zzed", "media is paused...")
                } else Log.i("zzed", "media is resume playing...")
            }
        })
        zedAudioPlayer?.setOnSeekListener(object : OnSeekListener {
            override fun onSeek(seekTime: Int, totalTime: Int) {
                Log.i(
                    "zzed",
                    "media seeks to ${ZedTimeUtil.secdsToDateFormat(seekTime, totalTime)}"
                )
            }
        })
        zedAudioPlayer?.setOnStopListener(object : OnStopListener {
            override fun onStop() {
                Log.i("zzed", "media is stopped!")
            }
        })
        zedAudioPlayer?.setOnPlayTimeListener(object : OnPlayTimeListener {
            @SuppressLint("SetTextI18n")
            override fun onPlayTime(totalTime: Int, currentTime: Int) {
//                Log.i(
//                    "zzed",
//                    "${ZedTimeUtil.secdsToDateFormat(
//                        totalTime,
//                        totalTime
//                    )}/${ZedTimeUtil.secdsToDateFormat(currentTime, totalTime)}"
//                )
                runOnUiThread(Runnable {
                    play_time.text = "${ZedTimeUtil.secdsToDateFormat(
                        totalTime,
                        totalTime
                    )}/${ZedTimeUtil.secdsToDateFormat(currentTime, totalTime)}"
                    if (!isSeekBar) {
                        seek_bar.progress = currentTime * 100 / totalTime
                    }
                })
            }
        })
        zedAudioPlayer?.setOnDBListener(object : OnDBListener {
            override fun onDB(db: Int) {
                Log.i("zzed", "media db is $db")
            }
        })
        zedAudioPlayer?.setOnErrorListener(object : OnErrorListener {
            override fun onError(errorCode: Int, errorMsg: String) {
                Log.e("zzed", "media error,error code is $errorCode,error message is $errorMsg!")
            }
        })
        zedAudioPlayer?.setOnOnCompleteListener(object : OnCompleteListener {
            override fun onComplete() {
                Log.i("zzed", "media play completely!")
                zedAudioPlayer?.stop()
            }
        })
        prepare.setOnClickListener {
            zedAudioPlayer?.prepared(
                File(
                    Environment.getExternalStorageDirectory(),
                    "Yasuo.mp3"
                ).absolutePath
            )
//            zedAudioPlayer?.prepared("http://fs.ios.kugou.com/202004101153/93a93051133616d6866fc9557cce9118/G153/M04/13/14/OYcBAFz3fF6AbF0fADS_2OPt0ag626.mp3")
        }
        pause.setOnClickListener {
            zedAudioPlayer?.pause(true)
        }
        resume.setOnClickListener {
            zedAudioPlayer?.pause(false)
        }
//        seek.setOnClickListener {
//            zedAudioPlayer.seek(270)
//        }
        stop.setOnClickListener {
            zedAudioPlayer?.stop()
        }
        next.setOnClickListener {
            zedAudioPlayer?.next(
                File(
                    Environment.getExternalStorageDirectory(),
                    "告白の夜.mp3"
                ).absolutePath
            )
//            zedAudioPlayer?.next("http://fs.ios.kugou.com/202004101026/7443d218bdfccf501004e288efeb8485/G151/M07/0E/19/d5QEAFz2MN-AJrB_ALDykLp1gS4802.mp3")
        }
        seek_bar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                disposables.add(Observable.just(zedAudioPlayer?.getDuration() ?: -1)
                    .subscribeOn(Schedulers.io())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe { totalDuration ->
                        if (totalDuration >= 0 && isSeekBar) {
                            position = totalDuration * progress / 100
                        } else if (totalDuration < 0) {
                            Toast.makeText(
                                this@ZedAudioActivity,
                                "media source isn't prepare!",
                                Toast.LENGTH_SHORT
                            ).show()
                        }
                    })
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                isSeekBar = true
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                isSeekBar = false
                zedAudioPlayer?.seek(position)
            }
        })
        volume_bar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            @SuppressLint("SetTextI18n")
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                Log.i("zzed", "media volume is $progress%")
                zedAudioPlayer?.volume(progress)
                volume_value.text = "音量：$progress%"
                volumeValue = progress
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        mute_left.setOnClickListener {
            zedAudioPlayer?.mute(ZedMuteEnum.MUTE_LEFT.muteValue)
            muteValue = ZedMuteEnum.MUTE_LEFT.muteValue
        }
        mute_right.setOnClickListener {
            zedAudioPlayer?.mute(ZedMuteEnum.MUTE_RIGHT.muteValue)
            muteValue = ZedMuteEnum.MUTE_RIGHT.muteValue
        }
        mute_center.setOnClickListener {
            zedAudioPlayer?.mute(ZedMuteEnum.MUTE_CENTER.muteValue)
            muteValue = ZedMuteEnum.MUTE_CENTER.muteValue
        }
        speed_half.setOnClickListener {
            zedAudioPlayer?.speed(0.5f)
            speedValue = 0.5f
        }
        speed_original.setOnClickListener {
            zedAudioPlayer?.speed(1.0f)
            speedValue = 1.0f
        }
        speed_one_and_half.setOnClickListener {
            zedAudioPlayer?.speed(1.5f)
            speedValue = 1.5f
        }
        pitch_half.setOnClickListener {
            zedAudioPlayer?.pitch(0.5f)
            pitchValue = 0.5f
        }
        pitch_original.setOnClickListener {
            zedAudioPlayer?.pitch(1.0f)
            pitchValue = 1.0f
        }
        pitch_one_and_half.setOnClickListener {
            zedAudioPlayer?.pitch(1.5f)
            pitchValue = 1.5f
        }
        start_record.setOnClickListener {
            zedAudioPlayer?.startRecord(
                File(
                    Environment.getExternalStorageDirectory(),
                    "test.aac"
                )
            )
        }
        pause_record.setOnClickListener {
            zedAudioPlayer?.pauseRecord()
        }
        resume_record.setOnClickListener {
            zedAudioPlayer?.resumeRecord()
        }
        stop_record.setOnClickListener {
            zedAudioPlayer?.stopRecord()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        disposables.clear()
    }
}