package com.github.zedmediaplayerlib.opengles

import android.content.Context
import android.opengl.GLES30
import android.util.Log
import java.io.BufferedReader
import java.io.InputStreamReader
import java.lang.StringBuilder

object ZedMediaShaderHelper {
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

    private fun createShader(shaderType: Int, source: String): Int = GLES30.glCreateShader(shaderType)
        .let {
            GLES30.glShaderSource(it, source)
            GLES30.glCompileShader(it)
            intArrayOf(1).let { array ->
                GLES30.glGetShaderiv(it, GLES30.GL_COMPILE_STATUS, array, 0)
                if (array[0] != GLES30.GL_TRUE) {
                    Log.e("wszed","shader compile error")
                    GLES30.glDeleteShader(it)
                    0
                } else it
            }
        }

    fun createProgram(vertexSource: String, fragmentSource: String): Int {
        GLES30.glCreateProgram().let { program ->
            if (program == 0) return 0
            createShader(GLES30.GL_VERTEX_SHADER, vertexSource).let {
                when (it) {
                    0 -> return 0
                    else -> GLES30.glAttachShader(program, it)
                }
            }
            createShader(GLES30.GL_FRAGMENT_SHADER, fragmentSource).let {
                when (it) {
                    0 -> return 0
                    else -> GLES30.glAttachShader(program, it)
                }
            }
            GLES30.glLinkProgram(program)
            return intArrayOf(1).let { array ->
                GLES30.glGetProgramiv(program, GLES30.GL_LINK_STATUS, array, 0)
                if (array[0] != GLES30.GL_TRUE) {
                    Log.e("wszed","link program error")
                    GLES30.glDeleteProgram(program)
                    0
                } else program
            }
        }
    }
}