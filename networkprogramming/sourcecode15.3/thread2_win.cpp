#include <stdio.h>
#include <process.h>
#include <windows.h>
unsigned WINAPI ThreadFunc(void * arg)
{
    int i;
    int cnt=*(int*)arg;
    for(i=0;i<cnt;i++)
    {
        Sleep(1000);
        puts("running thread");
    }
    return 0;
}
int main(int argc, char* argv[])
{
    HANDLE hThread;
    DWORD wr;
    unsigned threadID;
    int param=5;

    hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadFunc, &param, 0, &threadID);
    if(hThread == 0)
    {
        puts("Create thread failed");
        return -1;
    }
    if(wr=WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
    {
        puts("Wait for thread failed");
        return -1;
    }
    printf("wait result:%s\n",(wr==WAIT_OBJECT_0)?"signaled":"timeout");
    puts("end of main");
    return 0;
}