LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := audio

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/../../third_party/android/ffmpeg_2.0/jni/ffmpeg \
		$(LOCAL_PATH)/../../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) \
		$(LOCAL_PATH)/../../libav \
		$(LOCAL_PATH)/.. \
		$(LOCAL_PATH) 

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS

LOCAL_LDLIBS += -llog -ldl
		

LOCAL_SRC_FILES += \
		../audio_out.cpp \
		av_buffer.cpp \
		loader.cpp \
		String8.cpp \
		AudioRecord.cpp \
		AudioSystem.cpp \
		AudioTrack.cpp \
		android_sys_render.cpp
		


LOCAL_ARM_MODE := arm

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
