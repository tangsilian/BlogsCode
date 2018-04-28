LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := mapcheck
LOCAL_SRC_FILES := mapcheck.cpp

include $(BUILD_EXECUTABLE)