package com.github.zedplayer.gles

import android.content.Context
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import com.github.zedplayer.R
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class ZedRender constructor(private val context: Context) : GLSurfaceView.Renderer {
    //1.绘制三角形顶点坐标的数组
    private val vertexData = floatArrayOf(
        -1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f
    )
    private lateinit var vertexBuffer: FloatBuffer
    //3.三角形shader脚本的编写(res/raw)

    //
    private var program: Int = 0
    private var avPosition: Int = 0
    private var afColor: Int = 0

    init {
        //2.给三角形坐标分配内存地址
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(vertexData)
            .also {
                it.position(0)
            }
    }

    override fun onDrawFrame(gl: GL10?) {
        GLES20.glClear(GLES20.GL_DEPTH_BUFFER_BIT or GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)
        GLES20.glUseProgram(program)
        GLES20.glUniform4f(afColor, 1.0f, 1.0f, 0.0f, 1.0f)
        GLES20.glEnableVertexAttribArray(avPosition)
        GLES20.glVertexAttribPointer(avPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer)
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 3)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        val vertexSource = ZedShaderHelper.readRawText(context, R.raw.vertex_shader)
        val fragmentSource = ZedShaderHelper.readRawText(context, R.raw.fragment_shader)
        program = ZedShaderHelper.createProgram(vertexSource, fragmentSource)
            .apply {
                takeIf { it > 0 }?.let {
                    avPosition = GLES20.glGetAttribLocation(it, "av_Position")
                    afColor = GLES20.glGetUniformLocation(it, "af_Color")
                }
            }
    }
}