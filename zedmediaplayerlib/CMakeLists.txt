cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/libs/include)

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

        #[[src/main/cpp/CCallJava.cpp

        src/main/cpp/WlFFmpeg.cpp

        src/main/cpp/WlPlayStatus.cpp

        src/main/cpp/WlAudio.cpp

        src/main/cpp/WlQueue.cpp]])

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