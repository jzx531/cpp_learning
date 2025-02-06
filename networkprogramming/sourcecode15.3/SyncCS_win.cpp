#include <stdio.h>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
long long num = 0;
CRITICAL_SECTION cs;
unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDec(void * arg);

int main(int argc, char* argv[])
{
    HANDLE tHandles[NUM_THREAD];
    int i;
    InitializeCriticalSection(&cs);
    for (i = 0; i < NUM_THREAD; i++) {
        if (i % 2 == 0) {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
        }
        else {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDec, NULL, 0, NULL);
        }
    }
    WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
    DeleteCriticalSection(&cs);
    printf("Final value of num is %lld\n", num);
    return 0;
}

unsigned WINAPI threadInc(void * arg)
{
    long long i;
    for (i = 0; i < 1000000; i++) {
        EnterCriticalSection(&cs);
        num++;
        LeaveCriticalSection(&cs);
    }
    return 0;
}

unsigned WINAPI threadDec(void * arg)
{
    long long i;
    for (i = 0; i < 1000000; i++) {
        EnterCriticalSection(&cs);
        num--;
        LeaveCriticalSection(&cs);
    }
    return 0;
}