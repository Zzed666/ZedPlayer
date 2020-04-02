package com.github.zedmediaplayerlib.commons


/**
 * Created by ywl on 2017-12-22.
 */

object ZedTimeUtil {

    /**
     * format times
     * @param secds
     * @param totalsecds
     * @return
     */
    fun secdsToDateFormat(secds: Int, totalsecds: Int): String {
        val hours = (secds / (60 * 60)).toLong()
        val minutes = (secds % (60 * 60) / 60).toLong()
        val seconds = (secds % 60).toLong()

        var sh = "00"
        if (hours > 0) {
            sh = if (hours < 10) {
                "0$hours"
            } else {
                hours.toString() + ""
            }
        }
        var sm = "00"
        if (minutes > 0) {
            sm = if (minutes < 10) {
                "0$minutes"
            } else {
                minutes.toString() + ""
            }
        }

        var ss = "00"
        if (seconds > 0) {
            ss = if (seconds < 10) {
                "0$seconds"
            } else {
                seconds.toString() + ""
            }
        }
        return if (totalsecds >= 3600) {
            "$sh:$sm:$ss"
        } else "$sm:$ss"
    }
}
