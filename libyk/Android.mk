 LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := cyk

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../third_party/android/openssl-1.0.1e/include \
	$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/include/boost-1_53 \
	$(LOCAL_PATH)/../third_party/avhttp/include \
	$(LOCAL_PATH)/utf8 \
	$(LOCAL_PATH)

LOCAL_CFLAGS += -DBOOST_ASIO_DYN_LINK #-DAVHTTP_ENABLE_OPENSSL #-D__STDC_CONSTANT_MACROS -DANSI_TERMINAL_COLORS 

LOCAL_LDLIBS += \
		-L$(LOCAL_PATH)/../third_party/android/openssl-1.0.1e/libs/$(TARGET_ARCH_ABI) -lcrypto -lssl \
		-L$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/lib \
		-lboost_thread-gcc-mt-1_53 -lboost_filesystem-gcc-mt-1_53 -lboost_system-gcc-mt-1_53 \
		-lboost_signals-gcc-mt-1_53 -lboost_date_time-gcc-mt-1_53 -lboost_program_options-gcc-mt-1_53 \
		-lboost_program_options-gcc-mt-1_53 -lboost_chrono-gcc-mt-1_53 -lboost_regex-gcc-mt-1_53 -lgnustl_static
		

LOCAL_SRC_FILES = libyk.cpp \
	          youku_impl.cpp

LOCAL_ARM_MODE := arm

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
