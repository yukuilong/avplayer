NDK_TOOLCHAIN_VERSION=4.7
APP_PROJECT_PATH := $(call my-dir)/../
APP_BUILD_SCRIPT:= $(APP_PROJECT_PATH)../../Android.mk 

APP_MODULES      :=  avcodec-2.0 avformat-2.0 swscale-2.0 swresample-2.0 avutil-2.0 torrent av  source demux video audio avplayer

APP_STL := gnustl_static
APP_PLATFORM := android-8
APP_ABI := armeabi armeabi-v7a
APP_CFLAGS:=-DDISABLE_NEON
