LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := lib/iconv.c \
                   libcharset/lib/localcharset.c \
                   lib/relocatable.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/libcharset \
                    $(LOCAL_PATH)/libcharset/include

#LOCAL_CFLAGS := \
#    -DBUILDING_LIBCHARSET \
#    -DBUILDING_LIBICONV \
#    -DIN_LIBRARY

LOCAL_CFLAGS := \
  -Wno-multichar \
  -D_ANDROID \
  -DLIBDIR="c" \
  -DBUILDING_LIBICONV \
  -DIN_LIBRARY

LOCAL_MODULE:= iconv

#$(info Configuring iconv...)
#COMMAND := $(shell \
#           export PATH=$(TOOLCHAIN_INSTALL_DIR)/bin:$$PATH; \
#           cd $(LOCAL_PATH); \
#           make distclean; \
#           ./configure --host="arm-linux-androideabi")
#$(info iconv configured.)

include $(BUILD_STATIC_LIBRARY)
