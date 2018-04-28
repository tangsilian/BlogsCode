LOCAL_PATH := $(call my-dir)  


include $(CLEAR_VARS)

LOCAL_CXXFLAGS +=  -g -O0
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := IRead
LOCAL_SRC_FILES := IRead.cpp
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE)