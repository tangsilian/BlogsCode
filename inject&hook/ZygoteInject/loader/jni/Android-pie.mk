#==============================
# compile inject exe
#==============================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := loader
LOCAL_SRC_FILES := loader.c shellcode.s ../../shared_src/ctools.c ../../shared_src/log.c
LOCAL_LDLIBS := -llog

LOCAL_ARM_MODE := arm

include $(BUILD_EXECUTABLE)