#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *thread_main(void *arg)
{
    int i;
    int cnt = *(int *)arg;
    char *msg = (char *)malloc(sizeof(char) * 50);
    strcpy(msg, "Hello World");
    for(i=0; i<cnt; i++)
    {
        sleep(1);
        puts(msg);
    }
    return (void *)msg;
}

int main(int argc,char *argv[])
{
    pthread_t tid;
    int thread_param=5;
    void *thread_result;

    if(pthread_create(&tid, NULL, thread_main, &thread_param)!= 0)
    {
        printf("Error creating thread\n");
        exit(1);
    }

    if(pthread_join(tid, &thread_result)!= 0)
    {
        printf("Error joining thread\n");
        exit(1);
    }

    printf("Thread returned message: %s\n", (char *)thread_result);

    return 0;
}