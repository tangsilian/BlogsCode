#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <elf.h>
#include "DoGotHook/DoGotHook.h"

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
 * @param 文件指针
 * @return stringtable的首地址
 */
char *GetElf32StringTabBaseFromfile(FILE *fp)
{

	Elf32_Ehdr Elf32_ElfHeader;
	memset(&Elf32_ElfHeader,0, sizeof(Elf32_ElfHeader));

	Elf32_Shdr Elf32_SectionHeader;
	memset(&Elf32_SectionHeader,0, sizeof(Elf32_SectionHeader));

	int iShstrtabOffset = 0;
	char *pStringTab = NULL;

	//获取ELF头
	fseek(fp, 0, SEEK_SET);
	fread(&Elf32_ElfHeader, sizeof(Elf32_ElfHeader), 1, fp);

	//获取Section中字符串Tab的位置
	iShstrtabOffset = Elf32_ElfHeader.e_shoff + Elf32_ElfHeader.e_shstrndx * Elf32_ElfHeader.e_shentsize;
	fseek(fp, iShstrtabOffset, SEEK_SET);
	fread(&Elf32_SectionHeader, sizeof(Elf32_SectionHeader), 1, fp);

	//获取字符串表的起始地址
	pStringTab = (char *)malloc(Elf32_SectionHeader.sh_size);
	if(NULL == pStringTab)
	{
		LOGE("[-] GetElf32ShdNameByNameOff malloc Elf32_SectionHeader fail.");
		return NULL;
	}
	fseek(fp, Elf32_SectionHeader.sh_offset, SEEK_SET);
	fread(pStringTab, Elf32_SectionHeader.sh_size, 1, fp);
	LOGI("[+] GetElf32StringTabBaseFromfile done, pStringTab is %p",pStringTab);
	return pStringTab;
}

/**
 * @description 获取Got表的首地址和size
 * @param IN 文件指针 
 * @param OUT Got表静态首地址 
 * @param OUT Got表的size
 * @return 错误返回 -1
 */
int GetGotStartAddrAndSize(FILE *fp, uint32_t *GotTableStartaddr, uint32_t *GotTableSize)
{
	const char szGotTableName[] = ".got";

	Elf32_Ehdr Elf32_ElfHeader;
	memset(&Elf32_ElfHeader, 0, sizeof(Elf32_ElfHeader));

	fseek(fp, 0, SEEK_SET);
	fread(&Elf32_ElfHeader, sizeof(Elf32_ElfHeader), 1, fp);

	Elf32_Shdr Elf32_SectionHeader;
	memset(&Elf32_SectionHeader, 0, sizeof(Elf32_SectionHeader));

	fseek(fp, Elf32_ElfHeader.e_shoff, SEEK_SET);

	char *pStringTableStartOff = NULL;
	pStringTableStartOff = GetElf32StringTabBaseFromfile(fp);

	if(NULL == pStringTableStartOff)
	{
		LOGE("[-] GetGotStartAddrAndSize get pStringTableStartOff fail.");
		return -1;
	}

	for(int i = 0; i < Elf32_ElfHeader.e_shnum; ++i)
	{
		fread(&Elf32_SectionHeader, Elf32_ElfHeader.e_shentsize, 1, fp);

		if((Elf32_SectionHeader.sh_type == SHT_PROGBITS)&&
			0 == strncmp(szGotTableName, pStringTableStartOff + Elf32_SectionHeader.sh_name, sizeof(szGotTableName)))
		{
			*GotTableStartaddr = Elf32_SectionHeader.sh_addr;
			*GotTableSize = Elf32_SectionHeader.sh_size;
		}
	}
	free(pStringTableStartOff);
	LOGI("[+] GetGotStartAddrAndSize done");
}


/**
 * @description GotHook接口
 * @param IN 目标目录
 * @param IN 模块名字
 * @param IN 要Hook的symbol地址
 * @param IN 新函数地址
 * @param OUT 旧函数存放的地址
 * @return 失败返回 -1
 */
int DoGotHook(const char *TargetDir, const char *TargetSoName,
			 void *symbol, void *new_function, void **old_function)
{
	uint32_t uiGotTableStartaddr = 0;
	uint32_t uiGotTableSize = 0;
	//打开要寻找GOT位置的so

	if(NULL == symbol)
	{
		LOGE("[-] symbol is NUll.");
		return -1;
	}

	if(NULL == new_function)
	{
		LOGE("[-] new_function is NUll.");
		return -1;
	}

	if(NULL == TargetDir)
	{
		LOGE("[-] TargetDir is NUll.");
		return -1;
	}

	if(NULL == TargetSoName)
	{
		LOGE("[-] TargetSoName is NUll.");
		return -1;
	}

	char filepath[256] = {0};
	snprintf(filepath, sizeof(filepath), "%s%s",TargetDir, TargetSoName);

	FILE *file = fopen(filepath, "rb");
	if(NULL == file)
	{
		LOGE("[-] DoGotHook open file fail.");
		return -1;
	}
	int nRet = GetGotStartAddrAndSize(file, &uiGotTableStartaddr, &uiGotTableSize);
	if(-1 == nRet)
	{
		LOGE("[-] GetGotStartAddrAndSize fail.");
		return -1;
	}
	LOGI("[+] uiGotTableStartaddr is %08x\n",uiGotTableStartaddr);
	LOGI("[+] uiGotTableSize is %08x\n",uiGotTableSize);

	uint32_t base = get_module_base(-1, TargetSoName);

	int bHaveFoundTargetAddr = 0;
	for(int i = 0; i < uiGotTableSize; i = i + 4)
	{
		if(*(uint32_t *)(base + uiGotTableStartaddr + i) == (uint32_t)symbol)
		{

			//保存旧值赋新值
			*old_function = symbol;

			LOGI("[+] the addr of old_function is: %p",symbol);
			LOGI("[+] the addr of new_function is: %p",new_function);
			//修改地址写权限，往地址写值
			write_data_to_addr(base + uiGotTableStartaddr + i, (uint32_t)new_function);
			LOGI("[+] modify done. it is point to addr : %08x",
				 *(uint32_t *)(base + uiGotTableStartaddr + i));

			bHaveFoundTargetAddr = 1;
			break;
		}
	}
	if(1 == bHaveFoundTargetAddr)
	{
		LOGI("[+] DoGotHook done");
	}
	else
	{
		LOGE("[-] DoGotHook fail");
	}	
	fclose(file);
	return 0;
}


