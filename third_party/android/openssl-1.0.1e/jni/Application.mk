NDK_TOOLCHAIN_VERSION=4.4.3
APP_PROJECT_PATH := $(shell pwd)
APP_MODULES      :=  crypto_static ssl_static
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/Android.mk
APP_ABI := armeabi armeabi-v7a
