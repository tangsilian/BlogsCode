#ifndef _DOGOTHOOK_H_
#define _DOGOTHOOK_H_

#include "android/log.h"
#include <stdio.h>

#define MYLOG "GSLab"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MYLOG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MYLOG, __VA_ARGS__)

int DoGotHook(const char *TargetDir, const char *TargetSoName,
			 void *symbol, void *new_function, void **old_function);

#endif