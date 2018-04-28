LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := elfinfector
LOCAL_SRC_FILES := elfinfector.c

include $(BUILD_EXECUTABLE)
