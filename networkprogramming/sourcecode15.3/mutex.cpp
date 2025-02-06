#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_THREAD 100
long long num = 0;

pthread_mutex_t mutex;

void *thread_inc(void *arg)
{
    int i;
    for (i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);
        num++;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *thread_dec(void *arg)
{
    int i;
    for (i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);
        num--;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[NUM_THREAD];
    int i;

    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < NUM_THREAD; i++) {
        if (i % 2 == 0) {
            pthread_create(&tid[i], NULL, thread_inc, NULL);
        } else {
            pthread_create(&tid[i], NULL, thread_dec, NULL);
        }
    }

    for (i = 0; i < NUM_THREAD; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("num = %lld\n", num);

    pthread_mutex_destroy(&mutex);

    return 0;
}