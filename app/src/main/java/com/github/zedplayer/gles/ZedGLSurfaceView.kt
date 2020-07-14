package com.github.zedplayer.gles

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class ZedGLSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs) {

    private val mContext: Context = context

    init {
        setEGLContextClientVersion(2)
        setRenderer(ZedRender(mContext))
        renderMode = RENDERMODE_WHEN_DIRTY
    }
}