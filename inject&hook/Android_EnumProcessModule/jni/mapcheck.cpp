#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool GetModuleBase(unsigned long &ulModBase, pid_t pid, const char *pszModName) {
	bool bRet = false;
	FILE *fp = NULL;
	char szMapFilePath[32] = {0};
	char szMapFileLine[1024] = {0};
	
	if (pszModName == NULL)
	{
		return bRet;
	}

	if (pid < 0)
	{
		sprintf(szMapFilePath, "/proc/self/maps");
	}
	else
	{
		sprintf(szMapFilePath, "/proc/%d/maps", pid);
	}

	fp = fopen(szMapFilePath, "r");
	if (fp != NULL)
	{
		while (fgets(szMapFileLine, 1023, fp) != NULL)
		{
			if (strstr(szMapFileLine, pszModName))
			{
				char *pszModAddrStart = strtok(szMapFileLine, "-");
				if (pszModAddrStart)
				{
					ulModBase = strtoul(pszModAddrStart, NULL, 16);

					if (ulModBase == 0x8000)
						ulModBase = 0;
						
					bRet = true;

					break;
				}
			}
		}

		fclose(fp);
	}

	return bRet;
}

bool GetModuleFullName(pid_t pid, const char *pszModName, char *pszFullModName, int nBuffSize)
{
	bool bRet = false;
	FILE *fp = NULL;
	char szMapFilePath[32] = {0};
	char szMapFileLine[1024] = {0};
	char *pszFullName = NULL;

	if (pszModName == NULL || pszFullModName == NULL || nBuffSize <= 0 )
	{
		return bRet;
	}
	
	if (pid < 0)
	{
		sprintf(szMapFilePath, "/proc/self/maps");
	}
	else
	{
		sprintf(szMapFilePath, "/proc/%d/maps", pid);
	}
	
	fp = fopen(szMapFilePath, "r");
	if (fp != NULL)
	{
		while (fgets(szMapFileLine, 1023, fp) != NULL)
		{
			if (strstr(szMapFileLine, pszModName))
			{
				if (szMapFileLine[strlen(szMapFileLine) - 1] == '\n')
				{
					szMapFileLine[strlen(szMapFileLine) - 1] = 0;
				}
				
				pszFullName = strchr(szMapFileLine, '/');
				if (pszFullName == NULL)
				{
					continue;
				}
				
				strncpy(pszFullModName, pszFullName, nBuffSize - 1);
				
				bRet = true;
			}
		}

		fclose(fp);
	}

	return bRet;
}

int main(int argc, char **argv)
{
	unsigned long ulCModBase = 0;
	char szCModPath[256] = {0};
	
	if (GetModuleBase(ulCModBase, getpid(), "libc.so"))
	{
		printf("c mod base:0x%lx\n", ulCModBase);
	}
	
	if (GetModuleFullName(getpid(), "libc.so", szCModPath, 256))
	{
		printf("c mod full path:%s\n", szCModPath);
	}
	
	printf("finish \n");
	
	return 0;
}