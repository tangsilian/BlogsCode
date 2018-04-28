#ifndef _DOEXCEPTIONHOOK_H_
#define _DOEXCEPTIONHOOK_H_

#include "android/log.h"
#include <stdio.h>

#define MYLOG "GSLab"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MYLOG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MYLOG, __VA_ARGS__)


int DoExceptionHook(const char *TargetsoName, uint32_t addr);

#endif