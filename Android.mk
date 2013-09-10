LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ffmpeg-build-dir:=$(LOCAL_PATH)/third_party/android/ffmpeg_2.0/jni
libtorrent-build-dir:= $(LOCAL_PATH)/libtorrent
libav-build-dir:= $(LOCAL_PATH)/libav
libdemux-build-dir:= $(LOCAL_PATH)/demux
libsource-build-dir:= $(LOCAL_PATH)/source
libvideo-build-dir:= $(LOCAL_PATH)/video/android_render
libaudio-build-dir:= $(LOCAL_PATH)/audio/android_render
libyk-build-dir:= $(LOCAL_PATH)/libyk
libavplayer-build-dir:=$(LOCAL_PATH)/android/Avplayer/jni


include $(ffmpeg-build-dir)/Android.mk
include $(libtorrent-build-dir)/Android.mk
include $(libav-build-dir)/Android.mk
include $(libsource-build-dir)/Android.mk
#include $(libyk-build-dir)/Android.mk
include $(libdemux-build-dir)/Android.mk
include $(libvideo-build-dir)/Android.mk
include $(libaudio-build-dir)/Android.mk
include $(libavplayer-build-dir)/Android.mk

