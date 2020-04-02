package com.github.zedplayer

import android.os.*
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.audio.ZedAudioPlayer
import com.github.zedmediaplayerlib.audio.listener.OnLoadListener
import com.github.zedmediaplayerlib.audio.listener.OnPauseListener
import com.github.zedmediaplayerlib.audio.listener.OnPreparedListener
import com.github.zedmediaplayerlib.audio.listener.OnStopListener
import kotlinx.android.synthetic.main.activity_zed_audio.*
import java.io.File

class ZedAudioActivity : AppCompatActivity() {

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
            }
        })
        zedAudioPlayer.setOnPauseListener(object : OnPauseListener {
            override fun onPause(pause: Boolean) {
                if (pause) {
                    Log.i("zzed", "media is paused...")
                } else Log.i("zzed", "media is resume playing...")
            }
        })
        zedAudioPlayer.setOnStopListener(object : OnStopListener {
            override fun onStop() {
                Log.i("zzed", "media is stopped!")
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
        stop.setOnClickListener {
            zedAudioPlayer.stop()
        }
    }
}