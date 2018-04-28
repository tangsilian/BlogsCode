LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES:=  TargetLibrary/TargetLibrary.c
LOCAL_MODULE := TargetLibrary
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_SRC_FILES:= DoExceptionHook/DoExceptionHook.cpp main.cpp
LOCAL_MODULE := main
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
include $(BUILD_EXECUTABLE)




