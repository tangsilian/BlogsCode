#include<string.h>
#include<stdio.h>
#include "elf.h"
#include <cstdlib>
/**
	非常重要的一个宏，功能很简单：
	P:需要对其的段地址
	ALIGNBYTES:对其的字节数
	功能：将P值补充到时ALIGNBYTES的整数倍
	这个函数也叫：页面对其函数
	eg: 0x3e45/0x1000 == >0x4000
	
*/
#define ALIGN(P, ALIGNBYTES)  ( ((unsigned long)P + ALIGNBYTES -1)&~(ALIGNBYTES-1) )

int addSectionFun(char*, char*, unsigned int);

int main()
{
	addSectionFun("test.so", ".jiangwei", 0x1000);
	return 0;
}

int addSectionFun(char *lpPath, char *szSecname, unsigned int nNewSecSize)
{
	char name[50];
	FILE *fdr, *fdw;
	char *base = NULL;
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *t_phdr, *load1, *load2, *dynamic;
	Elf32_Shdr *s_hdr;
	int flag = 0;
	int i = 0;
	unsigned mapSZ = 0;
	unsigned nLoop = 0;
	unsigned int nAddInitFun = 0;
	unsigned int nNewSecAddr = 0;
	unsigned int nModuleBase = 0;
	memset(name, 0, sizeof(name));
	if(nNewSecSize == 0)
	{
		return 0;
	}
	fdr = fopen(lpPath, "rb");
	strcpy(name, lpPath);
	if(strchr(name, '.'))
	{
		strcpy(strchr(name, '.'), "_new.so");
	}
	else
	{
		strcat(name, "_new");
	}
	fdw = fopen(name, "wb");
	if(fdr == NULL || fdw == NULL)
	{
		printf("Open file failed");
		return 1;
	}
	fseek(fdr, 0, SEEK_END);
	mapSZ = ftell(fdr);
	printf("mapSZ:0x%x\n", mapSZ);//映射的文件的大小

	base = (char*)malloc(mapSZ * 2 + nNewSecSize);//文件大小乘2+添加节的大小
	printf("base 0x%x \n", base);

	memset(base, 0, mapSZ * 2 + nNewSecSize);
	fseek(fdr, 0, SEEK_SET);//定位到文件开始的地方
	fread(base, 1, mapSZ, fdr);////拷贝源文件内容到base
	if(base == (void*) -1)
	{
		printf("fread fd failed");
		return 2;
	}

	//判断Program Header
	ehdr = (Elf32_Ehdr*) base;
	t_phdr = (Elf32_Phdr*)(base + sizeof(Elf32_Ehdr));
	for(i=0;i<ehdr->e_phnum;i++)//遍历program herader
	{
		if(t_phdr->p_type == PT_LOAD)//修改Program Header中的第一个type=LOAD段（可加载段）的文件大小和内存大小为修改之后整个文件的长度(这个后面会详细说明)
		{
			//
			if(flag == 0)
			{
				load1 = t_phdr;
				flag = 1;
				nModuleBase = load1->p_vaddr;//段的第一个字节在内存中的虚拟地址。去除第一个LOAD的Segment的值
				printf("load1 = %p, offset = 0x%x \n", load1, load1->p_offset);

			}
			else
			{
				load2 = t_phdr;//打印段的偏移
				printf("load2 = %p, offset = 0x%x \n", load2, load2->p_offset);
			}
		}
		if(t_phdr->p_type == PT_DYNAMIC)//动态段
		{
			dynamic = t_phdr;//打印段的偏移
			printf("dynamic = %p, offset = 0x%x \n", dynamic, dynamic->p_offset);
		}
		t_phdr ++;
	}

	//section header
	s_hdr = (Elf32_Shdr*)(base + ehdr->e_shoff);
	//获取到新加section的位置，这个是重点,需要进行页面对其操作
	printf("addr:0x%x\n",load2->p_paddr);
	nNewSecAddr = ALIGN(load2->p_paddr + load2->p_memsz - nModuleBase, load2->p_align);
	printf("new section add:%x \n", nNewSecAddr);

	if(load1->p_filesz < ALIGN(load2->p_paddr + load2->p_memsz, load2->p_align) )
	{
		printf("offset:%x\n",(ehdr->e_shoff + sizeof(Elf32_Shdr) * ehdr->e_shnum));
		//注意这里的代码的执行条件，这里其实就是判断section header是不是在文件的末尾
		if( (ehdr->e_shoff + sizeof(Elf32_Shdr) * ehdr->e_shnum) != mapSZ)
		{
			if(mapSZ + sizeof(Elf32_Shdr) * (ehdr->e_shnum + 1) > nNewSecAddr)
			{
				printf("无法添加节\n");
				return 3;
			}
			else
			{
				memcpy(base + mapSZ, base + ehdr->e_shoff, sizeof(Elf32_Shdr) * ehdr->e_shnum);//将Section Header拷贝到原来文件的末尾
				ehdr->e_shoff = mapSZ;
				mapSZ += sizeof(Elf32_Shdr) * ehdr->e_shnum;//加上Section Header的长度
				s_hdr = (Elf32_Shdr*)(base + ehdr->e_shoff);
				printf("ehdr_offset:%x",ehdr->e_shoff);
			}
		}
	}
	else
	{
		nNewSecAddr = load1->p_filesz;
	}
	printf("还可添加 %d 个节\n", (nNewSecAddr - ehdr->e_shoff) / sizeof(Elf32_Shdr) - ehdr->e_shnum - 1);
	//需要写入的大小
	int nWriteLen = nNewSecAddr + ALIGN(strlen(szSecname) + 1, 0x10) + nNewSecSize;//添加section之后的文件总长度：原来的长度 + section name + section size
	printf("write len %x\n",nWriteLen);


	char *lpWriteBuf = (char *)malloc(nWriteLen);//nWriteLen :最后文件的总大小
	memset(lpWriteBuf, 0, nWriteLen);

	//因为我们新加的Section name在String Section中，所以我们还得修改一下String Section的大小了
	//ehdr->e_shstrndx 首先知道String Section Header，然后替换sh_size位置的字节数即可。新的String Section的大小变成了  文件末尾+0x10 – 0x3075
	s_hdr[ehdr->e_shstrndx].sh_size = nNewSecAddr - s_hdr[ehdr->e_shstrndx].sh_offset + strlen(szSecname) + 1;
	
	strcpy(lpWriteBuf + nNewSecAddr, szSecname);//section name
	
	/**
		####section header
	 * typedef struct elf32_shdr {
		  Elf32_Word	sh_name;
		  Elf32_Word	sh_type;
		  Elf32_Word	sh_flags;
		  Elf32_Addr	sh_addr;
		  Elf32_Off	sh_offset;
		  Elf32_Word	sh_size;
		  Elf32_Word	sh_link;
		  Elf32_Word	sh_info;
		  Elf32_Word	sh_addralign;
		  Elf32_Word	sh_entsize;
		} Elf32_Shdr;
	 */
	Elf32_Shdr newSecShdr = {0};
	newSecShdr.sh_name = nNewSecAddr - s_hdr[ehdr->e_shstrndx].sh_offset;
	newSecShdr.sh_type = SHT_PROGBITS;//设置成和rodata相同的属性
	newSecShdr.sh_flags = SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR;
	nNewSecAddr += ALIGN(strlen(szSecname) + 1, 0x10);
	newSecShdr.sh_size = nNewSecSize;
	newSecShdr.sh_offset = nNewSecAddr;
	newSecShdr.sh_addr = nNewSecAddr + nModuleBase;
	newSecShdr.sh_addralign = 4;

	//修改Program Header中第一个类型为LOAD的段的文件大小和内存映像的大小
	load1->p_filesz = nWriteLen;//LOAD的段的文件大小
	load1->p_memsz = nNewSecAddr + nNewSecSize;//内存映像的大小
	load1->p_flags = 7;		


	ehdr->e_shnum++;//修改elf头部中section的count数
	memcpy(lpWriteBuf, base, mapSZ);
	memcpy(lpWriteBuf + mapSZ, &newSecShdr, sizeof(Elf32_Shdr));
	
	fseek(fdw, 0, SEEK_SET);
	fwrite(lpWriteBuf, 1, nWriteLen, fdw);
	fclose(fdw);
	fclose(fdr);
	free(base);
	free(lpWriteBuf);
	return 0;
}
