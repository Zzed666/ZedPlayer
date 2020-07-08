package com.github.zedplayer

import android.annotation.SuppressLint
import android.content.Intent
import android.os.*
import android.util.Log
import android.widget.SeekBar
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.listener.*
import com.github.zedmediaplayerlib.commons.ZedMediaStatus
import com.github.zedmediaplayerlib.commons.ZedMuteEnum
import com.github.zedmediaplayerlib.commons.ZedTimeUtil
import com.github.zedmediaplayerlib.media.ZedMediaPlayer
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.disposables.CompositeDisposable
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_zed_media.*
import java.io.File

class ZedMediaActivity : AppCompatActivity() {
    var zedMediaPlayer: ZedMediaPlayer? = null
    private val disposables = CompositeDisposable()
    var mediaPath: String = ""
    var position: Int = 0
    var isSeekBar: Boolean = false
    var volumeValue = 50
    var muteValue = ZedMuteEnum.MUTE_CENTER.muteValue
    var statusValue = ZedMediaStatus.STATUS_IDLE.statusValue
    var speedValue = 1.0f
    var pitchValue = 1.0f
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zed_media)
        initDatas()
        initEvents()
    }

    private fun initDatas() {
        volume_value.text = "音量：$volumeValue%"
        volume_bar.progress = volumeValue
        zedMediaPlayer = ZedMediaPlayer()
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    private fun initEvents() {
        zedMediaPlayer?.setOnLoadListener(object : OnLoadListener {
            override fun onLoad(load: Boolean) {
                statusValue = if (load) {
                    Log.i("zzed", "media is loading...")
                    ZedMediaStatus.STATUS_LOADING.statusValue
                } else {
                    Log.i("zzed", "media is playing...")
                    ZedMediaStatus.STATUS_PLAY.statusValue
                }
            }
        })
        zedMediaPlayer?.setOnPrepareListener(object : OnPreparedListener {
            override fun onPrepared() {
                Log.i("zzed", "media is prepared to play")
                zedMediaPlayer?.speed(speedValue)
                zedMediaPlayer?.pitch(pitchValue)
                zedMediaPlayer?.volume(volumeValue)
                zedMediaPlayer?.mute(muteValue)
                statusValue = ZedMediaStatus.STATUS_PREPARED.statusValue
                zedMediaPlayer?.splitPcm(true)
                zedMediaPlayer?.start()
            }
        })
        zedMediaPlayer?.setOnPauseListener(object : OnPauseListener {
            override fun onPause(pause: Boolean) {
                statusValue = if (pause) {
                    Log.i("zzed", "media is paused...")
                    ZedMediaStatus.STATUS_PAUSE.statusValue
                } else {
                    Log.i("zzed", "media is resume playing...")
                    ZedMediaStatus.STATUS_PLAY.statusValue
                }
            }
        })
        zedMediaPlayer?.setOnSeekListener(object : OnSeekListener {
            override fun onSeek(seekTime: Int, totalTime: Int) {
                Log.i(
                    "zzed",
                    "media seeks to ${ZedTimeUtil.secdsToDateFormat(seekTime, totalTime)}"
                )
            }
        })
        zedMediaPlayer?.setOnNextListener(object : OnNextListener {
            override fun onNext(path: String) {
                if (statusValue == ZedMediaStatus.STATUS_IDLE.statusValue) {
                    Log.e("zzed", "media is next!")
                    zedMediaPlayer?.prepared(path)
                }
            }
        })
        zedMediaPlayer?.setOnStopListener(object : OnStopListener {
            override fun onStop() {
                Log.i("zzed", "media is stopped!")
                statusValue = ZedMediaStatus.STATUS_IDLE.statusValue
            }
        })
        zedMediaPlayer?.setOnPlayTimeListener(object : OnPlayTimeListener {
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
        zedMediaPlayer?.setOnDBListener(object : OnDBListener {
            override fun onDB(db: Int) {
//                Log.i("zzed", "media db is $db")
            }
        })
        zedMediaPlayer?.setOnErrorListener(object : OnErrorListener {
            override fun onError(errorCode: Int, errorMsg: String) {
                Log.e("zzed", "media error,error code is $errorCode,error message is $errorMsg!")
            }
        })
        zedMediaPlayer?.setOnOnCompleteListener(object : OnCompleteListener {
            override fun onComplete() {
                Log.i("zzed", "media play completely!")
                zedMediaPlayer?.stop()
            }
        })
        zedMediaPlayer?.setOnOnRecordListener(object : OnRecordListener {
            override fun onRecord(time: Int) {
                Log.i("zzed", "media record time is $time.")
            }
        })
        prepare.setOnClickListener {
            if (statusValue == ZedMediaStatus.STATUS_IDLE.statusValue) {
                mediaPath = File(
                    Environment.getExternalStorageDirectory(),
                    "mojito.mp4"
                ).absolutePath
                zedMediaPlayer?.prepared(mediaPath)
//            zedAudioPlayer?.prepared("http://fs.ios.kugou.com/202004101153/93a93051133616d6866fc9557cce9118/G153/M04/13/14/OYcBAFz3fF6AbF0fADS_2OPt0ag626.mp3")
            }
        }
        pause.setOnClickListener {
            if (statusValue == ZedMediaStatus.STATUS_PLAY.statusValue)
                zedMediaPlayer?.pause(true)
        }
        resume.setOnClickListener {
            if (statusValue == ZedMediaStatus.STATUS_PAUSE.statusValue)
                zedMediaPlayer?.pause(false)
        }
//        seek.setOnClickListener {
//            zedAudioPlayer.seek(270)
//        }
        stop.setOnClickListener {
            if (statusValue != ZedMediaStatus.STATUS_IDLE.statusValue && statusValue != ZedMediaStatus.STATUS_STOPING.statusValue)
                statusValue = ZedMediaStatus.STATUS_STOPING.statusValue
            zedMediaPlayer?.stop()
        }
        next.setOnClickListener {
            if (statusValue != ZedMediaStatus.STATUS_STOPING.statusValue) {
                if (statusValue != ZedMediaStatus.STATUS_IDLE.statusValue) {
                    statusValue = ZedMediaStatus.STATUS_STOPING.statusValue
                }
                mediaPath = File(
                    Environment.getExternalStorageDirectory(),
                    "告白の夜.mp3"
                ).absolutePath
                zedMediaPlayer?.next(mediaPath)
//            zedAudioPlayer?.next("http://fs.ios.kugou.com/202004101026/7443d218bdfccf501004e288efeb8485/G151/M07/0E/19/d5QEAFz2MN-AJrB_ALDykLp1gS4802.mp3")
            }
        }
        seek_bar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                disposables.add(Observable.just(zedMediaPlayer?.getDuration() ?: -1)
                    .subscribeOn(Schedulers.io())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe { totalDuration ->
                        if (totalDuration >= 0 && isSeekBar) {
                            position = totalDuration * progress / 100
                        } else if (totalDuration < 0) {
                            Toast.makeText(
                                this@ZedMediaActivity,
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
                zedMediaPlayer?.seek(position)
            }
        })
        volume_bar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            @SuppressLint("SetTextI18n")
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                Log.i("zzed", "media volume is $progress%")
                zedMediaPlayer?.volume(progress)
                volume_value.text = "音量：$progress%"
                volumeValue = progress
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        mute_left.setOnClickListener {
            zedMediaPlayer?.mute(ZedMuteEnum.MUTE_LEFT.muteValue)
            muteValue = ZedMuteEnum.MUTE_LEFT.muteValue
        }
        mute_right.setOnClickListener {
            zedMediaPlayer?.mute(ZedMuteEnum.MUTE_RIGHT.muteValue)
            muteValue = ZedMuteEnum.MUTE_RIGHT.muteValue
        }
        mute_center.setOnClickListener {
            zedMediaPlayer?.mute(ZedMuteEnum.MUTE_CENTER.muteValue)
            muteValue = ZedMuteEnum.MUTE_CENTER.muteValue
        }
        speed_half.setOnClickListener {
            zedMediaPlayer?.speed(0.5f)
            speedValue = 0.5f
        }
        speed_original.setOnClickListener {
            zedMediaPlayer?.speed(1.0f)
            speedValue = 1.0f
        }
        speed_one_and_half.setOnClickListener {
            zedMediaPlayer?.speed(1.5f)
            speedValue = 1.5f
        }
        pitch_half.setOnClickListener {
            zedMediaPlayer?.pitch(0.5f)
            pitchValue = 0.5f
        }
        pitch_original.setOnClickListener {
            zedMediaPlayer?.pitch(1.0f)
            pitchValue = 1.0f
        }
        pitch_one_and_half.setOnClickListener {
            zedMediaPlayer?.pitch(1.5f)
            pitchValue = 1.5f
        }
        start_record.setOnClickListener {
            zedMediaPlayer?.startRecord(
                File(
                    Environment.getExternalStorageDirectory(),
                    "test.aac"
                )
            )
        }
        pause_record.setOnClickListener {
            zedMediaPlayer?.pauseRecord()
        }
        resume_record.setOnClickListener {
            zedMediaPlayer?.resumeRecord()
        }
        stop_record.setOnClickListener {
            zedMediaPlayer?.stopRecord()
        }
        jump_cut_pcm.setOnClickListener {
            if (statusValue != ZedMediaStatus.STATUS_IDLE.statusValue && statusValue != ZedMediaStatus.STATUS_STOPING.statusValue)
                statusValue = ZedMediaStatus.STATUS_STOPING.statusValue
            zedMediaPlayer?.stop()
            jumpToCutPcm()
        }
    }

    private fun jumpToCutPcm() {
        val intent = Intent(this, ZedCutAudioToPcmActivity::class.java)
        intent.putExtra("media_path",mediaPath)
        intent.putExtra("media_player",zedMediaPlayer)
        intent.putExtra("media_status",statusValue)
        startActivity(intent)
    }

    override fun onDestroy() {
        super.onDestroy()
        disposables.clear()
    }
}