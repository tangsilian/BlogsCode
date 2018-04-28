#include <stdio.h>
#include <Android/log.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

//constructor定义可以将函数定义到init节，so被加载则自动运行
void ModifyIBored() __attribute__((constructor));

#define LOG_TAG "GSLab"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);

/**
 * 修改页属性，改成可读可写可执行
 * @param   ulAddress   需要修改属性起始地址
 * @param   size     需要修改页属性的长度，byte为单位
 * @return  bool    修改是否成功
 */
bool ChangePageProperty(unsigned long ulAddress, size_t size)
{
    bool bRet = false;
    
    //计算包含的页数、对齐起始地址
    unsigned long ulPageSize = sysconf(_SC_PAGESIZE);
    int iProtect = PROT_READ | PROT_WRITE | PROT_EXEC;
    unsigned long ulNewPageStartAddress = (long)(ulAddress) & ~(ulPageSize - 1);
    long lPageCount = (size / ulPageSize) + 1;
    
    for(long l = 0; l < lPageCount; l++)
    {
        //利用mprotect改页属性
        int iRet = mprotect((const void *)(ulNewPageStartAddress), ulPageSize, iProtect);
        if(-1 == iRet)
        {
            bRet = false;
            LOGI("mprotect error:%s", strerror(errno));
            return bRet;
        }
    }
    bRet = true;
    return bRet;
}

/**
 * 安全读内存数据，即是改页属性
 * @param   ulAddress   要读的内存起始地址
 * @param   size        要读的内存大小，byte为单位
 * @param   pszOutBuffer    存放结果，需要外部保证其大小不小于size
 * @return  bool    访问是否成功
 */
bool SafeMemRead(unsigned long ulAddress, size_t size, char *pszOutBuffer)
{
    bool bRet = false;
    while(1)
    {
        if(NULL == pszOutBuffer)
        {
            LOGI("pszOutBuffer is null");
            break;
        }
        
        if(false == ChangePageProperty(ulAddress, size))
        {
            LOGI("ChangePageProperty error");
            break;
        }
        
        //直接拷贝内存
        memcpy((void*)pszOutBuffer, (void*)ulAddress, size);
        LOGI("SafeMemRead success");
        bRet = true;
        break;
    }
    return bRet;
}

/**
 * 安全写内存数据，即是改页属性
 * @param   ulAddress   修改的内存起始地址
 * @param   size        修改的内存大小,byte为单位
 * @param   pszBuffer   要写入的数据，需要外部保证其可大小不小于size
 * @return  bool    修改是否成功
 */
bool SafeMemWrite(unsigned long ulAddress, size_t size, char *pszBuffer)
{
    bool bRet = false;
    while(1)
    {
        if(NULL == pszBuffer)
        {
            LOGI("pszBuffer is null");
            break;
        }
        
        if(false == ChangePageProperty(ulAddress, size))
        {
            LOGI("ChangePageProperty error");
            break;
        }
        
        //直接拷贝内存
        memcpy((void*)ulAddress, (void*)pszBuffer, size);
        LOGI("SafeMemWrite success");
        bRet = true;
        break;
    }
    return bRet;
}

/**
 * 通过/proc/$pid/maps，获取模块基址
 * @param   pid     模块所在进程pid，如果访问自身进程，可填小余0的值，如-1
 * @param   pszModuleName       模块名字
 * @return  void*   模块基址，错误则返回0
 */
void * GetModuleBaseAddr(pid_t pid, char* pszModuleName)
{
    FILE *pFileMaps = NULL;
    unsigned long ulBaseValue = 0;
    char szMapFilePath[256] = {0};
    char szFileLineBuffer[1024] = {0};

    //pid判断，确定maps文件
    if (pid < 0)
    {
        snprintf(szMapFilePath, sizeof(szMapFilePath), "/proc/self/maps");
    }
    else
    {
        snprintf(szMapFilePath, sizeof(szMapFilePath),  "/proc/%d/maps", pid);
    }

    pFileMaps = fopen(szMapFilePath, "r");
    if (NULL == pFileMaps)
    {
        return (void *)ulBaseValue;
    }

    //循环遍历maps文件，找到相应模块，截取地址信息
    while (fgets(szFileLineBuffer, sizeof(szFileLineBuffer), pFileMaps) != NULL)
    {
        if (strstr(szFileLineBuffer, pszModuleName))
        {
            char *pszModuleAddress = strtok(szFileLineBuffer, "-");
            if (pszModuleAddress)
            {
                ulBaseValue = strtoul(pszModuleAddress, NULL, 16);

                if (ulBaseValue == 0x8000)
                    ulBaseValue = 0;

                break;
            }
        }
    }
    fclose(pFileMaps);
    LOGI("module %s baseAddr:%x", pszModuleName, (uint32_t)ulBaseValue);
    return (void *)ulBaseValue;
}


/**
 * 在so被加载就启动，用于修改DEMO APP，直接patch代码改写跳转实现功能
 */
void ModifyIBored()
{
    LOGI("In ModifyIBored.");
    char szBackupOpcodes[8] = {0};
    
    char szTargetModule[10] = "IHook.so";   //对应模块名
    unsigned long ulJudgeOpcodeOffset = 0x00000CBC;     //该地址，为修改的跳转的模块偏移地址
    unsigned long ulIBoredSoBaseAddr = (unsigned long)GetModuleBaseAddr(-1, szTargetModule);
    
    if(ulIBoredSoBaseAddr == 0)
    {
        LOGI("get module base address error.")
        return;
    }
    
    //读取原来的opcodes内存保存
    if(SafeMemRead(ulIBoredSoBaseAddr + ulJudgeOpcodeOffset, 8, szBackupOpcodes))
    {
        for(int i = 0; i < 8; i++)
        {
            LOGI("Opcode[%02d]:0x%2X", i, szBackupOpcodes[i]);
        }
    }
    
     //MOV R3, R3指令，NOP作用，将该指令写入指定位置，修改跳转判断
    char szNullOpcodes[12] = {0x03, 0x30, 0xA0, 0xE1, 0x03, 0x30, 0xA0, 0xE1, 0x03, 0x30, 0xA0, 0xE1};
    if(SafeMemWrite(ulIBoredSoBaseAddr + ulJudgeOpcodeOffset, 8, szNullOpcodes))
    {
        LOGI("patch success.")
    }    
}
