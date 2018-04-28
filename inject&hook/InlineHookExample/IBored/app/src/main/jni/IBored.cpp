//
//

#include "com_example_gslab_ibored_MainActivity.h"

static unsigned int uiTimeCounter = 0x1;

//该函数，根据uiTimeCounter全局变量，返回两种结果字符串
//大于30则返回成功提示，否则返回等待提示
JNIEXPORT jstring JNICALL Java_com_example_gslab_ibored_MainActivity_UpdateResult
        (JNIEnv *pJniEnv, jclass Jclass)
{
    unsigned  int uiLocalVar = 1;

    uiTimeCounter += uiLocalVar;

    if(uiTimeCounter > 300)
    {
        //win
        return pJniEnv->NewStringUTF("Enough. You Win!");
    }
    else
    {
        //wait
        return pJniEnv->NewStringUTF("Just Wait.");
    }
}