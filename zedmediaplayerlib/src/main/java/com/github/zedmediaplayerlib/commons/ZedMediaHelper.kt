package com.github.zedmediaplayerlib.commons

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import com.github.zedmediaplayerlib.listener.OnRecordListener
import java.io.File
import java.io.FileOutputStream

class ZedMediaHelper(private var onRecordListener: OnRecordListener?) {
    var isInitMediaCodec: Boolean = false
    private var audioFormat: MediaFormat? = null
    private var audioEncoder: MediaCodec? = null
    private var audioOutputStream: FileOutputStream? = null
    private var bufferInfo: MediaCodec.BufferInfo? = null
    private var pcmBufferSize: Int = 0
    private var audioSampleRate: Int = 0
    private var audioRecordTime: Double = 0.0
    private var outTempBuffer: ByteArray? = null
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun initMediaCodec(sampleRate: Int, outFile: File) {
        Log.d("wszed", "initMediaCodec")
        isInitMediaCodec = true
        audioRecordTime = 0.0
        audioSampleRate = sampleRate
        //创建audio_media_format
        audioFormat = MediaFormat.createAudioFormat(
            MediaFormat.MIMETYPE_AUDIO_AAC,
            sampleRate,
            2
        )
        audioFormat?.apply {
            setInteger(MediaFormat.KEY_BIT_RATE, 96000)
        }?.apply {
            setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC)
        }?.apply {
            setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096)
        }?.let { mediaFormat ->
            //创建audio_media_codec
            audioEncoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC)
            audioEncoder?.apply {
                configure(
                    mediaFormat,
                    null, null,
                    MediaCodec.CONFIGURE_FLAG_ENCODE
                )
            }?.apply {
                audioOutputStream = FileOutputStream(outFile)
                bufferInfo = MediaCodec.BufferInfo()
            }?.run {
                start()
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun encodePcmToAAC(srcBuffer: ByteArray, srcBufferSize: Int) {
        audioEncoder?.let {
            val inputBufferIndex = it.dequeueInputBuffer(0)
            it.takeIf { inputBufferIndex >= 0 }?.apply {
                getInputBuffer(inputBufferIndex)?.run {
                    clear()
                    put(srcBuffer)
                    queueInputBuffer(inputBufferIndex, 0, srcBufferSize, 0, 0)
                }
            }?.let { encoder ->
                encodeOutBuffer(encoder, srcBufferSize)
            }
        }
    }

    fun releaseMediaCodec() {
        takeIf { it.isInitMediaCodec }?.run {
            try {
                audioOutputStream?.close()
                audioOutputStream = null
                audioEncoder?.stop()
                audioEncoder?.release()
                audioEncoder = null
                audioFormat = null
                bufferInfo = null
                audioRecordTime = 0.0
                isInitMediaCodec = false
                Log.d("wszed", "releaseMediaCodec")
            } catch (e: Exception) {

            } finally {
                audioOutputStream?.close()
                audioOutputStream = null
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    private fun encodeOutBuffer(encoder: MediaCodec, bufferSize: Int) {
        encoder.dequeueOutputBuffer(bufferInfo!!, 0)
            .takeIf { outputBufferIndex -> outputBufferIndex >= 0 }?.let {
                Log.d("wszed", "encodeOutBuffer")
                encoder.getOutputBuffer(it)?.run {
                    pcmBufferSize = bufferInfo!!.size + 7
                    outTempBuffer = ByteArray(pcmBufferSize)
                    position(bufferInfo!!.offset)
                    limit(bufferInfo!!.size + bufferInfo!!.offset)
                    addAdtsHeader(outTempBuffer!!, pcmBufferSize)
                    get(outTempBuffer!!, 7, bufferInfo!!.size)
                    position(bufferInfo!!.offset)
                    audioOutputStream?.write(outTempBuffer!!, 0, pcmBufferSize)
                    encoder.releaseOutputBuffer(it, 0)
                    audioRecordTime += getRecordTime(bufferSize)
                    onRecordListener?.onRecord(audioRecordTime.toInt())
                    outTempBuffer = null
                    encodeOutBuffer(encoder, bufferSize)
                }
            }
    }

    private fun getRecordTime(bufferSize: Int) = bufferSize * 1.0 / (audioSampleRate * 2 * 2)

    private fun addAdtsHeader(packet: ByteArray, packetLen: Int) {
        //AAC LC
        val profile = 2
        val freqIdx = getAdtsSmapleRate(audioSampleRate)
        val chanCfg = 2

        //0xFFF(12bit),这里只取了8位,所以还差4位放到下一个里面
        packet[0] = 0xFF.toByte()
        //第一个4位放F
        packet[1] = 0xF9.toByte()
        packet[2] = (((profile - 1).shl(6)) + (freqIdx.shl(2)) + (chanCfg.shr(2))).toByte()
        packet[3] = (((chanCfg and 3).shl(6)) + (packetLen.shr(11))).toByte()
        packet[4] = (((packetLen and 0x7FF).shr(3))).toByte()
        packet[5] = (((packetLen and 7).shl(5)) + 0x1F).toByte()
        packet[6] = 0xFC.toByte()
    }

    private fun getAdtsSmapleRate(sampleRate: Int): Int = when (sampleRate) {
        96000 -> 0
        88200 -> 1
        64000 -> 2
        48000 -> 3
        44100 -> 4
        32000 -> 5
        24000 -> 6
        22050 -> 7
        16000 -> 8
        12000 -> 9
        11025 -> 10
        8000 -> 11
        7350 -> 12
        else -> 4
    }
}