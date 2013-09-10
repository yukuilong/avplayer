LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := librtmp

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/ \
	$(LOCAL_PATH)/../../openssl-1.0.1e/include 

LOCAL_LDLIBS += \
		-L$(LOCAL_PATH)/../../openssl-1.0.1e/obj/local/$(TARGET_ARCH_ABI) -lssl_static -lcrypto_static 
	

LOCAL_SRC_FILES += \
	amf.c \
	hashswf.c \
	log.c \
	parseurl.c \
	rtmp.c 
	
LOCAL_ARM_MODE := arm

LOCAL_LDLIBS += -lz

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
