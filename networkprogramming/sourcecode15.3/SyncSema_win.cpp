#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>

unsigned WINAPI ThreadFunc(void* arg);
unsigned WINAPI Acu(void* arg);

static HANDLE semOne, semTwo;
static int num;

int main(int argc,char* argv[])
{
    HANDLE hThread1, hThread2;
    semOne = CreateSemaphore(NULL, 0, 1, NULL);
    semTwo = CreateSemaphore(NULL, 1, 1, NULL);

    hThread1 = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, Acu, NULL, 0, NULL);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(semOne);
    CloseHandle(semTwo);

    return 0;
}


unsigned WINAPI ThreadFunc(void* arg)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        fputs("Input number:",stdout);
        WaitForSingleObject(semOne, INFINITE);
        scanf("%d", &num);
        ReleaseSemaphore(semTwo, 1, NULL);
        // fputs("Number received:",stdout);
        // printf("%d\n", num);
    }
    return 0;
}

unsigned WINAPI Acu(void* arg)
{
    int sum = 0,i;
    for(i=0;i<5;i++)
    {
        WaitForSingleObject(semTwo, INFINITE);
        sum += num;
        ReleaseSemaphore(semOne, 1, NULL);
    }
    printf("Sum of numbers is %d\n", sum);
    return 0;
}
