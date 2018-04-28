//
// Created by betabin on 16/3/14.
//

#include "com_example_gslab_ihook_MainActivity.h"

static unsigned int uiTimeCounter = 0x1;

//该函数，根据uiTimeCounter全局变量，返回两种结果字符串
//大于30则返回成功提示，否则返回等待提示
JNIEXPORT jstring JNICALL Java_com_example_gslab_ihook_MainActivity_UpdateResult
        (JNIEnv *pJniEnv, jclass Jclass)
{
        uiTimeCounter ++;

        if(uiTimeCounter > 30)
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