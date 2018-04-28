#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include "DoExceptionHook/DoExceptionHook.h"

typedef void (*pfnfoo)(void);

const char g_MainDir[] = "/data/local/tmp/main/";
const char g_TargetSo[] = "libTargetLibrary.so";


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

	//下面演示ExceptionHook，R3寄存器
	DoExceptionHook(g_TargetSo, 0x00000CD0);

	foo();
	LOGI("[+] The end of the function main\n");
	return 0;
}
