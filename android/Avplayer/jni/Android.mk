LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := avplayer

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../../../third_party/android/ffmpeg_2.0/jni/ffmpeg \
	$(LOCAL_PATH)/../../../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) \
	$(LOCAL_PATH)/../../../libav \
	$(LOCAL_PATH)/../../../source \
	$(LOCAL_PATH)/../../../demux \
	$(LOCAL_PATH)/../../../audio \
	$(LOCAL_PATH)/../../../video 

LOCAL_LDLIBS += \
	-L$(LOCAL_PATH)/../obj/local/$(TARGET_ARCH_ABI) \
	-L$(LOCAL_PATH)/../../../third_party/android/openssl-1.0.1e/obj/local/$(TARGET_ARCH_ABI) -lssl_static -lcrypto_static \
	-L$(LOCAL_PATH)/../../../third_party/android/Boost-for-Android-master/build/lib -lboost_thread-gcc-mt-1_53 -lboost_filesystem-gcc-mt-1_53 -lboost_system-gcc-mt-1_53 \
	-L$(LOCAL_PATH)/../../../third_party/android/iconv-master/obj/local/$(TARGET_ARCH_ABI) -liconv \
	-L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(NDK_TOOLCHAIN_VERSION)/libs/$(TARGET_ARCH_ABI) -lgnustl_static

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS
		
LOCAL_LDLIBS += -llog -ldl -lGLESv2 -lz -ltorrent

LOCAL_SRC_FILES = player_impl.cpp avplayer_jni.cpp
	

LOCAL_ARM_MODE := arm

LOCAL_SHARED_LIBRARIES :=libavcodec-2.0 libavformat-2.0 libswscale-2.0 libswresample-2.0 libavutil-2.0
LOCAL_STATIC_LIBRARIES := libav libsource libdemux libvideo libaudio

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
