package com.github.zedplayer

import android.Manifest
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.github.zedpermissionlib.PermissionsActivity
import com.github.zedpermissionlib.PermissionsChecker
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    private val RESULT_CODE = 0 // 请求码
    var permissionListTmp = arrayOf<String>(
        Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO, Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.INTERNET, Manifest.permission.MODIFY_AUDIO_SETTINGS
    )// 所需的全部权限

    private var mPermissionsChecker: PermissionsChecker? = null // 权限检测器

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initEvents()
    }

    override fun onResume() {
        super.onResume()
        if (mPermissionsChecker!!.lacksPermissions(*permissionListTmp)) {
            startPermissionsActivity()
        }
    }

    private fun startPermissionsActivity() {
        PermissionsActivity.startActivityForResult(this, RESULT_CODE, *permissionListTmp)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == RESULT_CODE && resultCode == PermissionsActivity.PERMISSIONS_DENIED) {
            finish()
        }
    }

    private fun initEvents() {
        mPermissionsChecker = PermissionsChecker(this)
        zed_audio.setOnClickListener {
            startActivity(Intent(this,ZedAudioActivity::class.java))
        }
    }
}
