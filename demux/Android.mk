LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := demux

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) \
	$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/include/boost-1_53 \
	$(LOCAL_PATH)/../libav \
	$(LOCAL_PATH)/../source

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS

LOCAL_LDLIBS += -L$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/lib -lboost_filesystem-gcc-mt-1_53 -lboost_system-gcc-mt-1_53 

#-L$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/libs/$(TARGET_ARCH_ABI) -lavutil-2.0 -lavformat-2.0 \
		

LOCAL_SRC_FILES = demux.cpp \
		generic_demux.cpp 
		#youku_demux.cpp 

LOCAL_ARM_MODE := arm

LOCAL_STATIC_LIBRARIES :=libav libsource
LOCAL_SHARED_LIBRARIES := libavutil-2.0 libavformat-2.0

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
