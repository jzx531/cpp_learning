#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_main(void* arg)
{
    int i;
    int cnt = *((int*)arg);
    for (i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("Thread 1 running");
    }
    return NULL;  // 明确返回 NULL
}

int main()
{
    pthread_t tid;
    int cnt = 5;

    if (pthread_create(&tid, NULL, thread_main, (void*)cnt) != 0)
    {
        printf("Error creating thread\n");
        return 1;
    }
    sleep(10);
    puts("Main thread running end");
    return 0;
}