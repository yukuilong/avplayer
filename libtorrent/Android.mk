LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := torrent

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/../third_party/android/openssl-1.0.1e/include \
		$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/include/boost-1_53 \
		$(LOCAL_PATH)/../third_party/android/iconv-master/include \
		$(LOCAL_PATH)/include/libtorrent \
		$(LOCAL_PATH)/include

LOCAL_LDLIBS += \
		-L$(LOCAL_PATH)/../third_party/android/openssl-1.0.1e/obj/local/$(TARGET_ARCH_ABI)  -lssl_static -lcrypto_static \
		-L$(LOCAL_PATH)/../third_party/android/Boost-for-Android-master/build/lib -lboost_system-gcc-mt-1_53 \
		-L$(LOCAL_PATH)/../third_party/android/iconv-master/obj/local/$(TARGET_ARCH_ABI) -liconv 

LOCAL_CFLAGS +=  -D_FILE_OFFSET_BITS=64 -DBOOST_ASIO_DYN_LINK -DBOOST_ASIO_ENABLE_CANCELIO \
		 -DTORRENT_USE_OPENSSL -DUNICODE -D_UNICODE -DBOOST_DISABLE_EXCEPTION \
		 -D__STDC_LIMIT_MACROS 

FILE_LIST = $(wildcard $(LOCAL_PATH)/src/*.cpp $(LOCAL_PATH)/src/*.c $(LOCAL_PATH)/src/kademlia/*.cpp)
	    

LOCAL_SRC_FILES = $(FILE_LIST:$(LOCAL_PATH)/src/%=src/%)

#$(warning $(LOCAL_SRC_FILES))

LOCAL_ARM_MODE := arm

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti
	
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
