#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI ThreadFunc(void* arg)
{
    int i;
    int cnt = *(int*)arg;
    for(i=0;i<cnt;i++)
    {
        printf("ThreadFunc: %d\n", i);
        Sleep(1000);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    HANDLE hThread;
    unsigned threadID;
    int param = 5;
    hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, &param, 0, &threadID);
    if(hThread == 0)
    {
        puts("_beginthreadex failed");
        return -1;
    }
    hThread =(HANDLE) _beginthreadex(NULL, 0, ThreadFunc, &param, 0, &threadID);
    if(hThread == 0)
    {
        puts("_beginthreadex failed");
        return -1;
    }
    Sleep(10000);
    puts("Main thread exit");
    return 0;
}