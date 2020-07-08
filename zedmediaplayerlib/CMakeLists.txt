cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/libs/include

        src/main/cpp/soundtouch/include)

add_library(avcodec

        SHARED

        IMPORTED
        )

add_library(avdevice

        SHARED

        IMPORTED
        )

add_library(avfilter

        SHARED

        IMPORTED
        )

add_library(avformat

        SHARED

        IMPORTED
        )
add_library(avutil

        SHARED

        IMPORTED
        )

add_library(swresample

        SHARED

        IMPORTED
        )
add_library(swscale

        SHARED

        IMPORTED
        )

add_library(jni-lib

        SHARED

        src/main/cpp/native-lib.cpp

        src/main/cpp/zed-player-lib.cpp

#        src/main/cpp/zedaudio/zed-audio-lib.cpp

        src/main/cpp/ffmpeg/ZedFfmpeg.cpp

        src/main/cpp/cjava/CCallJava.cpp

        src/main/cpp/zedaudio/ZedAudio.cpp

        src/main/cpp/zedvideo/ZedVideo.cpp

        src/main/cpp/queues/ZedQueue.cpp

        src/main/cpp/status/ZedStatus.cpp

        src/main/cpp/pcms/beans/ZedPcmBean.cpp

        src/main/cpp/pcms/bufferqueues/ZedBufferQueue.cpp


        src/main/cpp/soundtouch/source/AAFilter.cpp

        src/main/cpp/soundtouch/source/BPMDetect.cpp

        src/main/cpp/soundtouch/source/cpu_detect_x86.cpp

        src/main/cpp/soundtouch/source/FIFOSampleBuffer.cpp

        src/main/cpp/soundtouch/source/FIRFilter.cpp

        src/main/cpp/soundtouch/source/InterpolateCubic.cpp

        src/main/cpp/soundtouch/source/InterpolateLinear.cpp

        src/main/cpp/soundtouch/source/InterpolateShannon.cpp

        src/main/cpp/soundtouch/source/mmx_optimized.cpp

        src/main/cpp/soundtouch/source/PeakFinder.cpp

        src/main/cpp/soundtouch/source/RateTransposer.cpp

        src/main/cpp/soundtouch/source/SoundTouch.cpp

        src/main/cpp/soundtouch/source/sse_optimized.cpp

        src/main/cpp/soundtouch/source/TDStretch.cpp)

set_target_properties(
        avcodec
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavcodec.so
)

set_target_properties(
        avdevice
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavdevice.so
)

set_target_properties(
        avfilter
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavfilter.so
)

set_target_properties(
        avformat
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavformat.so
)

set_target_properties(
        avutil
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libavutil.so
)

set_target_properties(
        swresample
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libswresample.so
)

set_target_properties(
        swscale
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libswscale.so
)

find_library(log-lib

        log)

target_link_libraries(jni-lib
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        OpenSLES
        ${log-lib})