package com.github.zedmediaplayerlib.opengles

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class ZedMediaSurfaceView@JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs) {
    private val mContext: Context = context
    private var mZedMediaRender: ZedMediaRender? = null

    init {
        setEGLContextClientVersion(3)
        setRenderer(ZedMediaRender(mContext).also { mZedMediaRender = it })
        renderMode = RENDERMODE_WHEN_DIRTY
    }

    fun setYUVData(width: Int, height: Int, yArray: ByteArray, uArray: ByteArray, vArray: ByteArray) {
        mZedMediaRender?.setYUVData(width, height, yArray, uArray, vArray)
        requestRender()
    }

}