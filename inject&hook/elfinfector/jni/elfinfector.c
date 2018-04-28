#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <elf.h>
#include <libgen.h>

#define SO_NAME "libhello.so"
#define MAX_NAME_LENGTH 255

void LoadElfFile(char *pszFileName, uint32_t *pFileSize, char **ppbyDataBuff);
uint32_t ModifyElfFile(char *pbyELfFile, uint32_t ulFileSize);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description: 主流程函数， 控制文件备份，加载，修改，写回的流程
 * Input:       argc 参数个数， argv为参数列表
 * Output:      无
 * Return:      返回程序运行状态
 * Others:      无
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

int main(int argc, char **argv){

    if (argc != 2) {
        printf("Usage: %s <soname>\n", basename(argv[0]));
        exit(-1);
    }
    char filename[MAX_NAME_LENGTH];
    char *pszElfFileName = argv[1];
    strncpy(filename, argv[1], MAX_NAME_LENGTH - 1);
    filename[MAX_NAME_LENGTH - 1] = 0;
    char *pszBakFileName = strcat(filename, ".bak");
    int ret = rename(pszElfFileName, pszBakFileName);
    if(ret != 0) {
        printf("[Error] backup %s file failed!\n", pszElfFileName);
        exit(-1);
    }
    uint32_t ulFileSize = 0;
    char *pbyBakFile;

    LoadElfFile(pszBakFileName, &ulFileSize, &pbyBakFile);
    uint32_t ulNewFileSize = ModifyElfFile(pbyBakFile, ulFileSize);
    FILE* pElfFile = fopen(pszElfFileName, "wb");
    if(!pElfFile) {
        printf("[Error] open file %s failed!\n", pszElfFileName);
    }
    fwrite(pbyBakFile, 1, ulNewFileSize, pElfFile);
    fclose(pElfFile);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description: 加载原Elf文件的数据到内存
 * Input:       pszFileName为Elf文件名， pFileSize为文件大小
 * Output:      ppbyDataBuff为加载到内存后的buff指针
 * Return:      无
 * Others:      无
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

void LoadElfFile(char *pszFileName, uint32_t *pFileSize, char **ppbyDataBuff) {

    FILE* pFile= fopen(pszFileName, "rb");
    if(pFile == NULL){
        printf("[ERROR] %s file may couldn't be opened!\n", pszFileName);
        exit(-2);
    }

    fseek(pFile, 0, SEEK_END);
    uint32_t ulFileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    uint8_t *pbyDataBuff = malloc(ulFileSize*2);
    memset(pbyDataBuff, 0, ulFileSize*2);
    if(!pbyDataBuff) {
        printf("[ERROR] malloc failed!\n");
        exit(-2);
    }

    uint32_t ulReadLen = fread(pbyDataBuff, 1, ulFileSize, pFile);
    if(ulReadLen != ulFileSize) {
        printf("[ERROR] read file %s failed!\n", pszFileName);
        exit(-2);
    }
    fclose(pFile);

    *pFileSize = ulFileSize;
    *ppbyDataBuff = pbyDataBuff;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description: 修改内存中Elf文件的数据buff，使其在运行时加载自定义模块
 * Input:       pbyElfFile为Elf文件数据buff, ulFileSize为文件大小
 * Output:      ppbyDataBuff为加载到内存后的buff指针
 * Return:      修改之后的ELF文件大小
 * Others:      无
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

uint32_t ModifyElfFile(char *pbyElfFile, uint32_t ulFileSize) {

    Elf32_Ehdr* pEhdr = (Elf32_Ehdr*)pbyElfFile;
    if(!IS_ELF(*pEhdr)) {
        printf("[ERROR] elf magic error!\n");
        exit(-3);
    }
    
    //将程序头表移动到文件尾以便新增表项
    Elf32_Phdr* pPhdr= (Elf32_Phdr*)(pbyElfFile + pEhdr->e_phoff);
    uint32_t ulPhdrSize = pEhdr->e_phnum * pEhdr->e_phentsize;
    memcpy(pbyElfFile + ulFileSize, pPhdr, ulPhdrSize);             
    pPhdr = (Elf32_Phdr*)(pbyElfFile + ulFileSize);
    printf("[INFO] move phdr success!\n");

    ///增加一个程序头表项
    Elf32_Phdr *pPTLoad = pPhdr + pEhdr->e_phnum;
    pEhdr->e_phoff = ulFileSize;
    pEhdr->e_phnum += 1;
    ulPhdrSize += pEhdr->e_phentsize;
    printf("[INFO] add phdr ent success!\n");
    
    //将DT_STRTAB移到文件尾并且添加新so的名字
    char *pOldStrTab = NULL;
    uint32_t ulOldStrTabLen = 0;
    int i = 0;
    for(i = 0; i < pEhdr->e_phnum; i++) {
        if(pPhdr[i].p_type == PT_DYNAMIC) {
            Elf32_Dyn *pDyn = (Elf32_Dyn*)(pbyElfFile + pPhdr[i].p_offset);
            for(; (char *)pDyn - (pbyElfFile + pPhdr[i].p_offset + pPhdr[i].p_filesz) < 0; pDyn++) {
                if(pDyn->d_tag == DT_STRTAB) {
                    pOldStrTab = pbyElfFile + pDyn->d_un.d_ptr;
                }else if(pDyn->d_tag == DT_STRSZ) {
                    ulOldStrTabLen = pDyn->d_un.d_val;
                }
            }
            break;
        }
    }
    printf("[INFO] get DT_STRTAB success!\n");

    char *pNewStrTab = (char *)(pPhdr + pEhdr->e_phnum);
    uint32_t ulNewStrTabLen = ulOldStrTabLen;

    memcpy(pNewStrTab, pOldStrTab, ulOldStrTabLen);
    strncpy(pNewStrTab + ulNewStrTabLen, SO_NAME, MAX_NAME_LENGTH);
    char *pNewModuleName = pNewStrTab + ulNewStrTabLen;
    ulNewStrTabLen += strlen(SO_NAME) + 1;

    //计算文件尾映射时的虚拟地址
    uint32_t ulVAOffset = 0;
    uint32_t ulPageSize = sysconf(_SC_PAGESIZE);
    for(i = 0; i < pEhdr->e_phnum; i++) {
        if(pPhdr[i].p_type == PT_LOAD) {
            ulVAOffset = pPhdr[i].p_vaddr - pPhdr[i].p_offset;
            ulVAOffset += (pPhdr[i].p_memsz - pPhdr[i].p_filesz) & ~(ulPageSize - 1);
            ulVAOffset += ulPageSize;
        }
    }

    pPTLoad->p_type = PT_LOAD;
    pPTLoad->p_offset = ulFileSize;
    pPTLoad->p_vaddr = pPTLoad->p_paddr = pPTLoad->p_offset + ulVAOffset;
    pPTLoad->p_filesz = pPTLoad->p_memsz = ulPhdrSize + ulNewStrTabLen;
    pPTLoad->p_flags = PF_X | PF_W | PF_R;
    pPTLoad->p_align = ulPageSize;
    printf("[INFO] add PT_LOAD success!\n");

    //修改PT_PHDR中文件偏移和虚拟地址
    for(i = 0; i < pEhdr->e_phnum; i++) {
        if(pPhdr[i].p_type == PT_PHDR) {
            pPhdr[i].p_offset = ulFileSize;
            pPhdr[i].p_paddr = pPhdr[i].p_vaddr = ulFileSize + ulVAOffset;
            pPhdr[i].p_filesz = pPhdr[i].p_memsz = pEhdr->e_phnum * pEhdr->e_phentsize;
        }
    }
    

    //修改DT_STRTAB和DT_STRSZ
    for(i = 0; i < pEhdr->e_phnum; i++) {
        if(pPhdr[i].p_type == PT_DYNAMIC) {
            Elf32_Dyn *pDyn = (Elf32_Dyn*)(pbyElfFile + pPhdr[i].p_offset);
            for(; (char *)pDyn - (pbyElfFile + pPhdr[i].p_offset + pPhdr[i].p_filesz) < 0; pDyn++) {
                if(pDyn->d_tag == DT_STRTAB) {
                    pDyn->d_un.d_ptr = pNewStrTab - pbyElfFile + ulVAOffset;
                }else if(pDyn->d_tag == DT_STRSZ) {
                    pDyn->d_un.d_val = ulNewStrTabLen;
                }else if(pDyn->d_tag == DT_NULL) {
                    pDyn->d_tag = DT_NEEDED;
                    pDyn->d_un.d_ptr = pNewModuleName - pNewStrTab;
                    break;
                }
            }
            break;
        }
    }
    printf("[INFO] modify DT_STRTAB success!\n");
    return ulFileSize + ulPhdrSize + ulNewStrTabLen;
} 
