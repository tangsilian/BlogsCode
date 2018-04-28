#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#define TRACERPID "TracerPid:"
#define TRACERPID_LEN (sizeof(TRACERPID) - 1)

void loop()
{
    while(true) 
    {
        sleep(60);
    }
}

bool check_debugger(pid_t pid)
{
    const int pathSize = 256;
    const int bufSize = 1024;

    char path[pathSize];
    char line[bufSize];

    snprintf(path, sizeof(path) - 1, "/proc/%d/status", pid);

    bool result = true;
    FILE *fp = fopen(path, "rt");
    if (fp != NULL) 
    {   
        while (fgets(line, sizeof(line), fp)) 
        {
            if (strncmp(line, TRACERPID, TRACERPID_LEN) == 0) 
            {
                pid_t tracerPid = 0; 

                sscanf(line, "%*s%d", &tracerPid);
                if (!tracerPid) result = false;
                
                printf("%s", line);
                break;
            }
        }

        fclose(fp);
    }

    return result;
}

void *anti_debugger_thread(void *data)
{
    pid_t pid = getpid();

    while (true)
    {
        check_debugger(pid);
        sleep(1);
    }
}

void anti_debugger()
{    
    pthread_t tid;
    pthread_create(&tid, NULL, &anti_debugger_thread, NULL);
}

int main()
{
    printf("pid: %d\n", getpid());

    anti_debugger();
    loop();
    
    return 0;
}