LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_CXXFLAGS +=  -g -O0
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := IBored
LOCAL_SRC_FILES := IBored.cpp

include $(BUILD_SHARED_LIBRARY)