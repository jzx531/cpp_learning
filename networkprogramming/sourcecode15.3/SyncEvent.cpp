#include <stdio.h>
#include <windows.h>
#include <process.h>
#define STR_LEN 100

unsigned WINAPI NumberofA(void* arg);
unsigned WINAPI NumberofOther(void* arg);

static char str[STR_LEN];
static HANDLE hEvent;

int main(int argc,char *argv[])
{
    HANDLE hThread1,hThread2;
    hEvent = CreateEvent(NULL,TRUE,FALSE,NULL); // 手动重置事件
    hThread1 = (HANDLE)_beginthreadex(NULL,0,NumberofA,NULL,0,NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL,0,NumberofOther,NULL,0,NULL);
    fputs("Enter a string: ",stdout);
    fgets(str,STR_LEN,stdin);
    SetEvent(hEvent); // 触发事件
    
    WaitForSingleObject(hThread1,INFINITE);
    WaitForSingleObject(hThread2,INFINITE);
    ResetEvent(hEvent); // 重置事件
    CloseHandle(hEvent);
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    return 0;
}

unsigned WINAPI NumberofA(void* arg)
{
    int i,cnt =0;
    WaitForSingleObject(hEvent,INFINITE);
    for(i=0;str[i]!=0;i++)
    {
        if (str[i] == 'a')
        {
            cnt++;
        }
    }
    printf("Number of 'a' in the string is %d\n",cnt);
    return 0;
}

unsigned WINAPI NumberofOther(void* arg)
{
    int i,cnt =0;
    WaitForSingleObject(hEvent,INFINITE);
    for(i=0;str[i]!=0;i++)
    {
        if (str[i] !='a')
        {
            cnt++;
        }
    }
    printf("Number of non-'a' characters in the string is %d\n",cnt-1);
    return 0;
}