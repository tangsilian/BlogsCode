#include <stdio.h>
#include <Android/log.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define LOG_TAG "GSLab"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);

/**
 * 写文件，指定偏移及大小
 * @param   pFile       需要写入的文件指针
 * @param   ulOffsetBegin   要写入位置，文件偏移
 * @param   size        要写入的长度，byte为单位
 * @param   pszBuffer   写入的内容，需要外部保证访问大小不小于size
 * @return  bool        是否写成功
 */
bool IWrite(FILE *pFile, unsigned long ulOffsetBegin, size_t size, char* pszBuffer)
{
    bool bRet = false;
    
    while(1)
    {
        if(NULL == pFile || NULL == pszBuffer)
        {
            break;
        }
        
        int iRet = fseek(pFile, ulOffsetBegin, SEEK_SET);
        if(-1 == iRet)
        {
            break;
        }
        
        for(int i = 0; i < size; i++)
        {
            fputc(pszBuffer[i], pFile);
        }
        LOGI("write success");
        bRet = true;
        break;
    }
    
    return bRet;
}

/**
 * 读文件，指定偏移及大小
 * @param   pFile       需要读取的文件指针
 * @param   ulOffsetBegin   要读取位置，文件偏移
 * @param   size        要读取的长度，byte为单位
 * @param   pszOutBuffer    存放读取结果，需要外部保证其大小不小于size
 * @return  bool        读取是否成功
 */
bool IRead(FILE *pFile, unsigned long ulOffsetBegin, size_t size, char* pszOutBuffer)
{
    bool bRet = false;
    
    while(1)
    {
        if(pFile == NULL && pszOutBuffer == NULL)
        {
            break;
        }
        
        int iRet = fseek(pFile, ulOffsetBegin, SEEK_SET);
        if(-1 == iRet)
        {
            break;
        }
        
        fread(pszOutBuffer, 1, size, pFile);
        bRet = true;
        break;
    }
    
    return bRet;
}

/**
 * 测试函数，用于修改IBored ndk层的关键数据uiTimeCounter，实现逻辑修改
 * 经过分析，可以发现uiTimeCounter存放在段首，可读可写段+0x4的位置
 * @param   pszPid      demo应用的pid
 * @param   pszRWModuleBegin    其可读可写段的内存偏移，用于搜索数据修改逻辑
 */
void ModifyIBored(const char* pszPid, const char* pszRWModuleBegin)
{
    char szMemPath[32] = {0};   //存放mem文件路径
    unsigned long ulRWOffsetBegin = atol(pszRWModuleBegin);
    
    sprintf(szMemPath, "/proc/%s/mem", pszPid);
    
    FILE *pmemm = fopen(szMemPath, "r+");    
    LOGI("mem path:%s;\tOffset:0x%08X", szMemPath, (unsigned int)ulRWOffsetBegin);

    if(pmemm == NULL)
    {
        LOGI("fopen error:%d", errno);
    }
    
    //读取原来的uiTimeCounter，在可读可写段的+4位置
    char szBackupTime[4] = {0};
    if(IRead(pmemm, ulRWOffsetBegin + 4, 4, szBackupTime) == false)
    {
        LOGI("read error.");
        fclose(pmemm);
        return;
    }
    else
    {
        LOGI("Time:[0x%02X] [0x%02X] [0x%02X] [0x%02X]", szBackupTime[0], szBackupTime[1], szBackupTime[2], szBackupTime[3])
    }
    
    //改写IBored的uiTimeCounter，改成0（让其回归等待状态）
    char szEvilData[12] = {0x0, 0x0, 0x0, 0x0};
    if(IWrite(pmemm, ulRWOffsetBegin + 4, 4, szEvilData) == false)
    {
        LOGI("write error.");
        fclose(pmemm);
        return;
    }
    
    fclose(pmemm);
}


int main(int argc, char const *argv[])
{
    LOGI("In main");
    ModifyIBored(argv[1], argv[2]);
    
    return 0;
}