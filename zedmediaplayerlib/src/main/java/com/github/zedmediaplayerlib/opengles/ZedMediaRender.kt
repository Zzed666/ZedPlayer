package com.github.zedmediaplayerlib.opengles

import android.content.Context
import android.opengl.GLES30
import android.opengl.GLSurfaceView
import com.github.zedmediaplayerlib.R
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class ZedMediaRender(private val mContext: Context) : GLSurfaceView.Renderer {

    private var mYUVProgram: Int = 0

    private var mYUVAVPosition: Int = 0
    private var mYUVAFPosition: Int = 0
    private var mYUVSampler_Y: Int = 0
    private var mYUVSampler_U: Int = 0
    private var mYUVSampler_V: Int = 0

    private var mYUVWidth: Int = 0
    private var mYUVHeight: Int = 0

    private var mYUVVertexBuffer: FloatBuffer
    private var mYUVTextureBuffer: FloatBuffer

    private var mYUV_ByteBuffer_Y: ByteBuffer? = null
    private var mYUV_ByteBuffer_U: ByteBuffer? = null
    private var mYUV_ByteBuffer_V: ByteBuffer? = null

    private val mYUVVertexData = floatArrayOf(
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    )

    private val mYUVTextureData = floatArrayOf(
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    )

    private val mYUVTextureIds = intArrayOf(1, 1, 1)

    init {
        mYUVTextureBuffer = ByteBuffer.allocateDirect(mYUVTextureData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(mYUVTextureData)
            .also {
                it.position(0)
            }
        mYUVVertexBuffer = ByteBuffer.allocateDirect(mYUVVertexData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(mYUVVertexData)
            .also {
                it.position(0)
            }
    }

    private fun initYUVData() {
        val vertexSource = ZedMediaShaderHelper.readRawText(mContext, R.raw.zed_yuv_vertex_shader)
        val fragmentSource =
            ZedMediaShaderHelper.readRawText(mContext, R.raw.zed_yuv_fragment_shader)
        mYUVProgram = ZedMediaShaderHelper.createProgram(vertexSource, fragmentSource)
            .apply {
                takeIf { it > 0 }?.let {
                    mYUVAVPosition = GLES30.glGetAttribLocation(it, "av_Position")
                    mYUVAFPosition = GLES30.glGetAttribLocation(it, "af_Position")

                    mYUVSampler_Y = GLES30.glGetUniformLocation(it, "sampler_y")
                    mYUVSampler_U = GLES30.glGetUniformLocation(it, "sampler_u")
                    mYUVSampler_V = GLES30.glGetUniformLocation(it, "sampler_v")


                    GLES30.glGenTextures(3, mYUVTextureIds, 0)
                    for (index in 0 until 3) {
                        mYUVTextureIds[index].takeIf { it != 0 }?.let {
                            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, it)
                            GLES30.glTexParameteri(
                                GLES30.GL_TEXTURE_2D,
                                GLES30.GL_TEXTURE_WRAP_S,
                                GLES30.GL_REPEAT
                            )
                            GLES30.glTexParameteri(
                                GLES30.GL_TEXTURE_2D,
                                GLES30.GL_TEXTURE_WRAP_T,
                                GLES30.GL_REPEAT
                            )
                            GLES30.glTexParameteri(
                                GLES30.GL_TEXTURE_2D,
                                GLES30.GL_TEXTURE_MIN_FILTER,
                                GLES30.GL_LINEAR
                            )
                            GLES30.glTexParameteri(
                                GLES30.GL_TEXTURE_2D,
                                GLES30.GL_TEXTURE_MAG_FILTER,
                                GLES30.GL_LINEAR
                            )
                        }
                    }

                }
            }
    }

    fun setYUVData(
        width: Int,
        height: Int,
        yArray: ByteArray,
        uArray: ByteArray,
        vArray: ByteArray
    ) {
        mYUVWidth = width
        mYUVHeight = height
        mYUV_ByteBuffer_Y = ByteBuffer.wrap(yArray)
        mYUV_ByteBuffer_U = ByteBuffer.wrap(uArray)
        mYUV_ByteBuffer_V = ByteBuffer.wrap(vArray)
    }

    private fun renderYUV() {
        if (mYUVWidth > 0
            && mYUVHeight > 0
            && mYUV_ByteBuffer_Y != null
            && mYUV_ByteBuffer_U != null
            && mYUV_ByteBuffer_V != null
        ) {
            GLES30.glUseProgram(mYUVProgram)

            GLES30.glEnableVertexAttribArray(mYUVAVPosition)
            GLES30.glVertexAttribPointer(
                mYUVAVPosition,
                2,
                GLES30.GL_FLOAT,
                false,
                8,
                mYUVVertexBuffer
            )

            GLES30.glEnableVertexAttribArray(mYUVAFPosition)
            GLES30.glVertexAttribPointer(
                mYUVAFPosition,
                2,
                GLES30.GL_FLOAT,
                false,
                8,
                mYUVTextureBuffer
            )

            GLES30.glActiveTexture(GLES30.GL_TEXTURE0)
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mYUVTextureIds[0])
            GLES30.glTexImage2D(
                GLES30.GL_TEXTURE_2D,
                0,
                GLES30.GL_LUMINANCE,
                mYUVWidth,
                mYUVHeight,
                0,
                GLES30.GL_LUMINANCE,
                GLES30.GL_UNSIGNED_BYTE,
                mYUV_ByteBuffer_Y
            )

            GLES30.glActiveTexture(GLES30.GL_TEXTURE1)
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mYUVTextureIds[1])
            GLES30.glTexImage2D(
                GLES30.GL_TEXTURE_2D,
                0,
                GLES30.GL_LUMINANCE,
                mYUVWidth / 2,
                mYUVHeight / 2,
                0,
                GLES30.GL_LUMINANCE,
                GLES30.GL_UNSIGNED_BYTE,
                mYUV_ByteBuffer_U
            )

            GLES30.glActiveTexture(GLES30.GL_TEXTURE2)
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mYUVTextureIds[2])
            GLES30.glTexImage2D(
                GLES30.GL_TEXTURE_2D,
                0,
                GLES30.GL_LUMINANCE,
                mYUVWidth / 2,
                mYUVHeight / 2,
                0,
                GLES30.GL_LUMINANCE,
                GLES30.GL_UNSIGNED_BYTE,
                mYUV_ByteBuffer_V
            )

            GLES30.glUniform1i(mYUVSampler_Y, 0)
            GLES30.glUniform1i(mYUVSampler_U, 1)
            GLES30.glUniform1i(mYUVSampler_V, 2)

            mYUV_ByteBuffer_Y?.clear()
            mYUV_ByteBuffer_U?.clear()
            mYUV_ByteBuffer_V?.clear()
        }
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        initYUVData()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        GLES30.glViewport(0, 0, width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        GLES30.glClear(GLES30.GL_DEPTH_BUFFER_BIT or GLES30.GL_COLOR_BUFFER_BIT)
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)

        renderYUV()
        //if mYUVWidth > 0&& mYUVHeight > 0&& mYUV_ByteBuffer_Y != null&& mYUV_ByteBuffer_U != null&& mYUV_ByteBuffer_V != null
        //glDrawArrays放在判断里边，如果不满足条件就不会执行glDrawArrays方法，而出现黑屏,所以将glDrawArrays方法放到renderYUV后边执行
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_STRIP, 0, 4)
    }

}