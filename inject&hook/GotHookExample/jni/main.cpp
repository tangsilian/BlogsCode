#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include "DoGotHook/DoGotHook.h"

typedef void (*pfnfoo)(void);

const char g_MainDir[] = "/data/local/tmp/main/";
const char g_TargetSo[] = "libTargetLibrary.so";

int (*old_gettimeofday)(struct timeval *tv, struct timezone *tz);
int replaced_gettimeofday(struct timeval *tv, struct timezone *tz)
{
	LOGI("[+] GSLab gettimeofday GOT Hack OK\n");
	return old_gettimeofday(tv, tz);
} 

int main()
{
	LOGI("[+] GSLab start main\n");

	char absolutepath[256] = {0};
	snprintf(absolutepath, sizeof(absolutepath), "%s%s",g_MainDir, g_TargetSo);

	void *handler = dlopen(absolutepath, RTLD_NOW);
	if(NULL == handler)
	{
		LOGE("[-] dlopen fail");
		return -1;
	}
	pfnfoo foo = (pfnfoo)dlsym(handler, "foo");

	//下面演示GotHook
	DoGotHook(g_MainDir, g_TargetSo, (void *)gettimeofday, (void *)replaced_gettimeofday, (void **)&old_gettimeofday);

	foo();
	LOGI("[+] The end of the function main\n");
	return 0;
}
