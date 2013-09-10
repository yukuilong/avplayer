APP_PROJECT_PATH := $(call my-dir)/../
APP_BUILD_SCRIPT:= $(APP_PROJECT_PATH)Android.mk 

APP_MODULES      :=libiconv


#APP_STL := stlport_static
#APP_STL := gnustl_static
#APP_PLATFORM := android-8
APP_ABI := armeabi armeabi-v7a
#APP_CFLAGS:=-DDISABLE_NEON
