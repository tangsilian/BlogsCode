#include <vector>

extern "C"
{
    #include "Ihook.h"
}

void ModifyIBored() __attribute__((constructor));

typedef std::vector<INLINE_HOOK_INFO*> InlineHookInfoPVec;
static InlineHookInfoPVec gs_vecInlineHookInfo;     //管理HOOK点

/**
 * 对外inline hook接口，负责管理inline hook信息
 * @param  pHookAddr     要hook的地址
 * @param  onCallBack    要插入的回调函数
 * @return               inlinehook是否设置成功（已经设置过，重复设置返回false）
 */
bool InlineHook(void *pHookAddr, void (*onCallBack)(struct pt_regs *))
{
    bool bRet = false;

    if(pHookAddr == NULL || onCallBack == NULL)
    {
        return bRet;
    }

    INLINE_HOOK_INFO* pstInlineHook = new INLINE_HOOK_INFO();
    pstInlineHook->pHookAddr = pHookAddr;
    pstInlineHook->onCallBack = onCallBack;
    
    //DEMO只很对ARM指令进行演示，更通用这里需要判断区分THUMB等指令
    if(HookArm(pstInlineHook) == false)
    {
        LOGI("HookArm fail.");
        delete pstInlineHook;
        return bRet;
    }
    
    gs_vecInlineHookInfo.push_back(pstInlineHook);
    return true;
}

/**
 * 对外接口，用于取消inline hook
 * @param  pHookAddr 要取消inline hook的位置
 * @return           是否取消成功（不存在返回取消失败）
 */
bool UnInlineHook(void *pHookAddr)
{
    bool bRet = false;

    if(pHookAddr == NULL)
    {
        return bRet;
    }

    InlineHookInfoPVec::iterator itr = gs_vecInlineHookInfo.begin();
    InlineHookInfoPVec::iterator itrend = gs_vecInlineHookInfo.end();

    for (; itr != itrend; ++itr)
    {
        if (pHookAddr == (*itr)->pHookAddr)
        {
            INLINE_HOOK_INFO* pTargetInlineHookInfo = (*itr);

            gs_vecInlineHookInfo.erase(itr);
            if(pTargetInlineHookInfo->pStubShellCodeAddr != NULL)
            {
                delete pTargetInlineHookInfo->pStubShellCodeAddr;
            }
            if(pTargetInlineHookInfo->ppOldFuncAddr)
            {
                delete *(pTargetInlineHookInfo->ppOldFuncAddr);
            }
            delete pTargetInlineHookInfo;
            bRet = true;
        }
    }

    return bRet;
}

/**
 * 用户自定义的stub函数，嵌入在hook点中，可直接操作寄存器等改变游戏逻辑操作
 * 这里将R2寄存器锁定为0x333，一个远大于30的值
 * @param regs 寄存器结构，保存寄存器当前hook点的寄存器信息
 */
void EvilHookStubFunctionForIBored(pt_regs *regs)
{
    LOGI("In Evil Hook Stub.");
    regs->uregs[2] = 0x333;
}

/**
 * 针对IBored应用，通过inline hook改变游戏逻辑的测试函数
 */
void ModifyIBored()
{
    LOGI("In IHook's ModifyIBored.");
    
    void* pModuleBaseAddr = GetModuleBaseAddr(-1, "IBored.so");
    if(pModuleBaseAddr == 0)
    {
        LOGI("get module base error.");
        return;
    }
    
    uint32_t uiHookAddr = (uint32_t)pModuleBaseAddr + 0x00000CA8;
    
    LOGI("uiHookAddr is %X", uiHookAddr);
    
    InlineHook((void*)(uiHookAddr), EvilHookStubFunctionForIBored);
}