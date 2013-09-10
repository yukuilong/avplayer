LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := video

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/../../third_party/android/ffmpeg_2.0/jni/ffmpeg \
		$(LOCAL_PATH)/../../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) \
		$(LOCAL_PATH)/../../libav \
		$(LOCAL_PATH)/.. \
		$(LOCAL_PATH) 

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS

LOCAL_LDLIBS += -lGLESv2 -llog -ldl

LOCAL_SRC_FILES += \
		../video_out.cpp \
		android_opengles_render.cpp \
		shaders.c

LOCAL_STATIC_LIBRARIES := libav

LOCAL_ARM_MODE := arm

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
