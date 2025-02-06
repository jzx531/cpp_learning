#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * productor(void * arg);
void * consumer(void * arg);
int buffer_has_item = 0;
pthread_mutex_t mutex;
int running = 1;

int main(void)
{
    pthread_t consumer_t;
    pthread_t producer_t;

    pthread_mutex_init(&mutex, NULL);
    pthread_create(&consumer_t, NULL, consumer, NULL);
    pthread_create(&producer_t, NULL, productor, NULL);

    sleep(1);
    running = 0;
    pthread_join(consumer_t, NULL);
    pthread_join(producer_t, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void * productor(void * arg)
{
    while(running)
    {
        pthread_mutex_lock(&mutex);
        buffer_has_item ++;
        printf("productor: produce an item%d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
    }
}

void * consumer(void * arg)
{
    while(running){
        pthread_mutex_lock(&mutex);
        buffer_has_item--;
        printf("consumer: consume an item%d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
    }
}
