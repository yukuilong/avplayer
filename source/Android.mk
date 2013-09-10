LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := source

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg \
	$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/jni/ffmpeg/android/$(TARGET_ARCH_ABI) \
	$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/include/boost-1_53 \
	$(LOCAL_PATH)/../libyk \
	$(LOCAL_PATH)/../libtorrent/include \
	$(LOCAL_PATH)/../libav \
	$(LOCAL_PATH)/../third_party/android/openssl-1.0.1e/include \
	$(LOCAL_PATH)

LOCAL_CFLAGS += -DANSI_TERMINAL_COLORS -DBOOST_ASIO_DYN_LINK -D__STDC_CONSTANT_MACROS -DUSE_TORRENT #-DUSE_YK

LOCAL_LDLIBS += -L$(LOCAL_PATH)/../third_party/android/ffmpeg_2.0/$(TARGET_ARCH_ABI) \
		-L$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/lib -lboost_thread-gcc-mt-1_53 -lboost_filesystem-gcc-mt-1_53 -lboost_system-gcc-mt-1_53 \
		-L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(NDK_TOOLCHAIN_VERSION)/libs/$(TARGET_ARCH_ABI) -lgnustl_static \	

LOCAL_SRC_FILES = asio.cpp \
		file_source.cpp \
		source.cpp \
		torrent_source.cpp 
		#yk_source.cpp 

LOCAL_ARM_MODE := arm

LOCAL_STATIC_LIBRARIES := libtorrent

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
