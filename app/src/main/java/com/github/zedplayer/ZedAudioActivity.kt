package com.github.zedplayer

import android.os.*
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.github.zedmediaplayerlib.audio.ZedAudioPlayer
import com.github.zedmediaplayerlib.audio.listener.OnLoadListener
import com.github.zedmediaplayerlib.audio.listener.OnPreparedListener
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
        prepare.setOnClickListener {
            zedAudioPlayer.prepared(
                File(
                    Environment.getExternalStorageDirectory(),
                    "Yasuo.mp3"
                ).absolutePath
            )
        }
    }
}