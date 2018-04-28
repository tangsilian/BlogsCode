#ifndef _TARGETLIBRARY_H_
#define _TARGETLIBRARY_H_

#include "android/log.h"

#define MYLOG "GSLab"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MYLOG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MYLOG, __VA_ARGS__)

void foo(void);

#endif