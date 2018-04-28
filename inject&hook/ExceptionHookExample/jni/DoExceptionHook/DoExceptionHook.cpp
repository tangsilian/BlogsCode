#include <elf.h>
#include <stddef.h>
#include <ucontext.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <assert.h>
#include "DoExceptionHook/DoExceptionHook.h"

static uint32_t g_uiTargetaddr = 0xFFFFFFFF;
static uint32_t g_uiTargetOriginOpcode = 0xFFFFFFFF;
static uint32_t g_uiTargetNextOriginOpcode = 0xFFFFFFFF;
static int g_bIsThumb = 0;
char g_TargetSoName[80] = {0};

/**
 * @description 改变地址的权限为可读可写可执行
 * @param IN 地址 
 * @return 返回0即成功，-1即失败
 */

int change_addr_to_rwx(uint32_t addr)
{
	uint32_t pagesize = sysconf(_SC_PAGESIZE);
	uint32_t pagestart = addr & (~(pagesize-1));
	int nRet = mprotect((void *) pagestart, (pagesize), PROT_READ | PROT_WRITE | PROT_EXEC);
	if(nRet == -1)
    {
        LOGE("[-] mprotect error");
        return -1;
    }
    return 0;
}


/**
 * @description 往给定32位地址写数值
 * @param IN 地址 
 * @param IN 数据
 * @return 返回0即成功，-1即失败
 */

int write_data_to_addr(uint32_t addr, uint32_t value)
{
	LOGI("[+] modify start,addr is %08x,value is %08x", addr, value);
	int nRet = change_addr_to_rwx(addr);
	if(-1 == nRet)
	{
		LOGE("[-] write_data_to_addr error");
		return -1;
	}
    *(uint32_t *)(addr) = value ;
    LOGI("[+] modify completed,addr is %08x,value is %08x", addr, value);
    return 0;
}

/**
 * @description 获取模块基址
 * @param IN pid值，如果为-1，即获取自己的 
 * @param IN 模块名
 * @return 返回模块基址
 */

uint32_t get_module_base(pid_t pid, const char* module_name) 
{
	FILE *fp = NULL;
	uint32_t addr = 0;
	char *pAddrRange = NULL;
	char filename[32] = {0};
	char line[1024] = {0};

	if (pid < 0) 
	{
		snprintf(filename, sizeof(filename), "/proc/self/maps");
	} 
	else 
	{
		snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	}
	fp = fopen(filename, "r");
	if (fp != NULL) 
	{
		while (fgets(line, sizeof(line), fp)) 
		{
			if (strstr(line, module_name))
			{
				pAddrRange = strtok(line, "-");
				addr = strtoul(pAddrRange, NULL, 16);
				if (addr == 0x8000)
				{
					addr = 0;
				}
				break;
			}
		}
		fclose(fp);
	}
	return addr;
}


/**
 * @description 获取当前地址指令长度
 * @param IN 地址 
 * @return 返回长度
 */

int getCurAddrInstructionLength(uint32_t addr)
{
	//断言这个是thumb才这样去算
	assert(1 == (addr & 0x00000001));
	int iInslength = -1;
	uint32_t opcode = *(uint32_t *)(addr & (~0x00000001));
	LOGI("[+] opcode is %08x", opcode);
	//只有在[15:13]为 111 以及 [12:11] 不是 00 的时候是 32bit
	if(((opcode & 0x0000E000) == 0x0000E000) && ((opcode & 0x00001800) != 0))
	{
		// Thumb-2 32bit instruction
		iInslength = 4;
	}
	else
	{
		iInslength = 2;
	}

	LOGI("[+] get the length of the addr's instruction,length is %08x", iInslength);
	return iInslength;
}


/**
 * @description 往地址写非法指令并保存原来的opcode
 * @param IN 地址 
 * @param OUT 原opcode
 */

void WriteillegalInstructionAndSaveOpcode(uint32_t addr, uint32_t *OriginOpcode)
{

	if(0x00000001 == (addr & 0x00000001))
	{
		g_bIsThumb = 1;
		*OriginOpcode = *(uint32_t *)(addr & (~0x00000001));
		//Thumb illegal instruction : 0xdeXX
		uint32_t uiThumbillegalValue = 0x0000de00 | (0xFFFF0000 & *OriginOpcode);
		write_data_to_addr(addr & (~0x00000001), uiThumbillegalValue);
	}
	else
	{
		g_bIsThumb = 0;
		//Arm illegal instruction: 0xf7fXaXXX
		*OriginOpcode = *(uint32_t *)addr;
		uint32_t uiArmillegalValue = 0x7f000f0;
		write_data_to_addr(addr, uiArmillegalValue);
	}

	LOGI("[+] g_bIsThumb is %08x \n",g_bIsThumb);
	LOGI("[+] WriteillegalInstruction addr: %08x, OriginalOpcode is %08x",addr & (~0x00000001), *OriginOpcode);

}


/**
 * @description 对地址恢复原opcode
 * @param IN 地址 
 * @param IN 原opcode
 */

void ResumetheOrgInstruction(uint32_t addr, uint32_t opcode)
{
	write_data_to_addr(addr & (~0x00000001), opcode);
	LOGI("[+] ResumetheOrgInstruction addr: %08x, ResumeOpcode is %08x",addr & (~0x00000001), opcode);
}

/**
 * @description ExceptionHook的Handler
 * @param IN signum
 * @param IN siginfo_t
 * @param IN context
 */

void ExceptionHookHandler(int signum, siginfo_t *Ssiginfo, void *context)
{

	LOGI("[+] signum is %d", signum);
	ucontext_t *ucontext = (ucontext_t *)context;
	LOGI("[+] base is %08x",get_module_base(-1, g_TargetSoName));
	LOGI("[+] g_uiTargetaddr is %08x", g_uiTargetaddr);

	//其实下面这个不用转，只不过是因为我的clang不够智能,不转下会提示出错。
	struct sigcontext sigcontext = *(struct sigcontext *)&(ucontext->uc_mcontext);

	if (sigcontext.arm_pc == (g_uiTargetaddr & (~0x00000001)))
	{
		LOGI("[+] r0 = %08lx \n"\
		"[+] r1 = %08lx \n"\
		"[+] r2 = %08lx \n"\
		"[+] r3 = %08lx \n"\
		"[+] r4 = %08lx \n"\
		"[+] r5 = %08lx \n"\
		"[+] r6 = %08lx \n"\
		"[+] r7 = %08lx \n"\
		"[+] r8 = %08lx \n"\
		"[+] r9 = %08lx \n"\
		"[+] r10 = %08lx \n"\
		"[+] fp = %08lx \n"\
		"[+] ip = %08lx \n"\
		"[+] sp = %08lx \n"\
		"[+] lr = %08lx \n"\
		"[+] pc = %08lx \n"\
		"[+] cpsr = %08lx \n",
		sigcontext.arm_r0,
		sigcontext.arm_r1,
		sigcontext.arm_r2,
		sigcontext.arm_r3,
		sigcontext.arm_r4,
		sigcontext.arm_r5,
		sigcontext.arm_r6,
		sigcontext.arm_r7,
		sigcontext.arm_r8,
		sigcontext.arm_r9,
		sigcontext.arm_r10,
		sigcontext.arm_fp,
		sigcontext.arm_ip,
		sigcontext.arm_sp,
		sigcontext.arm_lr,
		sigcontext.arm_pc,	
		sigcontext.arm_cpsr);

		ResumetheOrgInstruction(g_uiTargetaddr, g_uiTargetOriginOpcode);
		LOGI("[+] Resume the g_uiTargetOriginOpcode");

		//给下一个地址写非法指令
		if(1 == g_bIsThumb)
		{
			int lengthOfThisInstruction = getCurAddrInstructionLength(g_uiTargetaddr);
			WriteillegalInstructionAndSaveOpcode(g_uiTargetaddr + lengthOfThisInstruction, &g_uiTargetNextOriginOpcode);
			LOGI("[+] Write illegal instruction to (g_uiTargetaddr x+ %d)", lengthOfThisInstruction);	
		}
		else
		{
			WriteillegalInstructionAndSaveOpcode(g_uiTargetaddr + 4, &g_uiTargetNextOriginOpcode);
			LOGI("[+] Write illegal instruction to (g_uiTargetaddr + 4)");	
		}

	}
	else
	{
		if(sigcontext.arm_pc == ((g_uiTargetaddr & (~0x00000001)) + 2)||
		   sigcontext.arm_pc == ((g_uiTargetaddr & (~0x00000001)) + 4) )
		{

			ResumetheOrgInstruction(sigcontext.arm_pc, g_uiTargetNextOriginOpcode);
			LOGI("[+] Resume the g_uiTargetNextOriginOpcode");
			//这里所有指令都恢复正常了

			//重新写非法指令，为下一次目的地址被执行而Hook
			WriteillegalInstructionAndSaveOpcode(g_uiTargetaddr, &g_uiTargetOriginOpcode);
			LOGI("[+] Write illegal instruction to g_uiTargetaddr");
		}
	}

}

/**
 * @description ExceptionHook接口
 * @param IN 模块名字
 * @param IN 地址
 */

int DoExceptionHook(const char *TargetsoName, uint32_t addr)
{

	if(NULL == TargetsoName)
	{
		LOGE("[-] TargetsoName is NUll.");
		return -1;
	}

	struct sigaction Ssigaction;
	sigemptyset(&Ssigaction.sa_mask);

	snprintf(g_TargetSoName, sizeof(g_TargetSoName), "%s", TargetsoName);

	//设置 SA_SIGINFO ，指定处理函数为sigaction的sa_sigaction函数
	Ssigaction.sa_flags = SA_SIGINFO;
	Ssigaction.sa_sigaction = ExceptionHookHandler;
	int nRet = sigaction(SIGILL, &Ssigaction, NULL);
	if(-1 == nRet)
	{
		LOGE("[-] sigaction error");
		return -1;
	}

	g_uiTargetaddr = get_module_base(-1, TargetsoName) + addr;	
	LOGI("[+] g_uiTargetaddr is %08x\n",g_uiTargetaddr);

	LOGI("[+] DoExceptionHook Writes illegal instruction to g_uiTargetaddr");
	WriteillegalInstructionAndSaveOpcode(g_uiTargetaddr, &g_uiTargetOriginOpcode);
	return 0;
}