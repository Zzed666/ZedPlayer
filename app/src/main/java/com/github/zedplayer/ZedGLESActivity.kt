package com.github.zedplayer

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity

class ZedGLESActivity : AppCompatActivity(){
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_zed_gles)
        init()
    }

    private fun init() {

    }

    override fun onPause() {
        super.onPause()
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onDestroy() {
        super.onDestroy()
    }
}