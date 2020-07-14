package com.github.zedplayer.gles

import android.content.Context
import android.opengl.GLES20
import android.util.Log
import java.io.BufferedReader
import java.io.InputStreamReader
import java.lang.StringBuilder

object ZedShaderHelper {
    fun readRawText(context: Context, rawId: Int): String = StringBuilder()
        .let {
            BufferedReader(InputStreamReader(context.resources.openRawResource(rawId)))
                .let { txt ->
                    var line: String?
                    while (!(txt.readLine().also { line = it }).isNullOrEmpty()) {
                        it.append(line).append("\n")
                    }
                }
            it.toString()
        }

    private fun createShader(shaderType: Int, source: String): Int = GLES20.glCreateShader(shaderType)
        .let {
            GLES20.glShaderSource(it, source)
            GLES20.glCompileShader(it)
            intArrayOf(1).let { array ->
                GLES20.glGetShaderiv(it, GLES20.GL_COMPILE_STATUS, array, 0)
                if (array[0] != GLES20.GL_TRUE) {
                    Log.e("wszed","shader compile error")
                    GLES20.glDeleteShader(it)
                    0
                } else it
            }
        }

    fun createProgram(vertexSource: String, fragmentSource: String): Int {
        GLES20.glCreateProgram().let { program ->
            if (program == 0) return 0
            createShader(GLES20.GL_VERTEX_SHADER, vertexSource).let {
                when (it) {
                    0 -> return 0
                    else -> GLES20.glAttachShader(program, it)
                }
            }
            createShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource).let {
                when (it) {
                    0 -> return 0
                    else -> GLES20.glAttachShader(program, it)
                }
            }
            GLES20.glLinkProgram(program)
            return intArrayOf(1).let { array ->
                GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, array, 0)
                if (array[0] != GLES20.GL_TRUE) {
                    Log.e("wszed","link program error")
                    GLES20.glDeleteProgram(program)
                    0
                } else program
            }
        }
    }
}