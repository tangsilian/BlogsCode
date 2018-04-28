LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE := poll_anti_debug
LOCAL_SRC_FILES := main.cpp

include $(BUILD_EXECUTABLE)