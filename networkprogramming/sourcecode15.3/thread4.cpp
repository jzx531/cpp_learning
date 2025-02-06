#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREAD 100
long long num = 0;

void *thread_inc(void *arg)
{
    int i;
    for(i=0; i<1000000; i++)
    {
        num++;
    }
    return NULL;
}

void *thread_dec(void *arg)
{
    int i;
    for(i=0; i<1000000; i++)
    {
        num--;
    }
    return NULL;
}

int main()
{
    pthread_t tid[NUM_THREAD];
    int i;
    for(i=0; i<NUM_THREAD; i++)
    {
        if(i%2 == 0)
        {
            pthread_create(&tid[i], NULL, thread_inc, NULL);
        }
        else
        {
            pthread_create(&tid[i], NULL, thread_dec, NULL);
        }
    }
    for(i=0; i<NUM_THREAD; i++)
    {
        pthread_join(tid[i], NULL);
    }
    printf("num = %lld\n", num);
    return 0;
}
