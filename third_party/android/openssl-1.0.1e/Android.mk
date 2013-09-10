#LOCAL_PATH := $(call my-dir)

#subdirs := $(addprefix $(LOCAL_PATH)/,$(addsuffix /Android.mk, \
#		crypto \
#		ssl \
#		apps \
#	))

#include $(subdirs)

LOCAL_PATH := $(call my-dir)

#subdirs := $(addprefix $(LOCAL_PATH)/,$(addsuffix /Android.mk, \
#		crypto \
#		ssl \
#		apps \
#	))

#subdirs := $(addprefix $(LOCAL_PATH)/,$(addsuffix /Android.mk, \
#		crypto \
#		ssl \
#	))

include $(LOCAL_PATH)/crypto/Android.mk
include $(LOCAL_PATH)/../ssl/Android.mk
#include $(subdirs)
