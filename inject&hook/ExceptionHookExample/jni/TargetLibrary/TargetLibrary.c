#include <stdio.h>
#include <unistd.h>
#include "TargetLibrary/TargetLibrary.h"

void foo(void) 
{
    int a = 3;
    int b = 2;
    while(a--)
    {
        sleep(2);
        b = a * b;
        //会在这里做一个ExceptionHook操作
        LOGI("[+] b is %d\n", b);
    }
    b = b + 2;
    b = b - 1;
    struct timeval tv;
    struct timezone tz;
    memset(&tv, 0, sizeof(tv));
    memset(&tz, 0, sizeof(tz));
    gettimeofday(&tv, &tz);
    LOGI("[+] GSLab, this is %d\n", b);
}



