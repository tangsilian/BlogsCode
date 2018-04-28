LOCAL_PATH := $(call my-dir)
include $( CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp
LOCAL_LDLIBS += -llog
LOCAL_MODULE := testSo
LOCAL_MODULE_FILENAME := testSo
LOCAL_SRC_FILES := testSo.c
include $(BUILD_SHARED_LIBRARY)