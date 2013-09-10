LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := av

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) 

#LOCAL_LDLIBS += -L$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/libs/$(TARGET_ARCH_ABI)

LOCAL_LDLIBS += -llog

LOCAL_SHARED_LIBRARIES :=libavcodec-2.0 libavformat-2.0 libswscale-2.0 libswresample-2.0 libavutil-2.0

LOCAL_SRC_FILES = avplay.c jni_common.c  

LOCAL_ARM_MODE := arm

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
