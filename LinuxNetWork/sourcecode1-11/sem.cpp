#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

void *producter_f(void * arg);
void *consumer_f(void * arg);
sem_t sem;
int running =1;
int main(void)
{
    pthread_t consumer_t;
    pthread_t producter_t;

    sem_init(&sem,0,16);
    pthread_create(&producter_t, NULL, producter_f, NULL);
    pthread_create(&consumer_t, NULL, consumer_f, NULL);

    sleep(1);
    running = 0;
    pthread_join(consumer_t, NULL);
    pthread_join(producter_t, NULL);
    sem_destroy(&sem);
    return 0;
}
    
void *producter_f(void * arg)
{
    int semval =0;
    while(running)
    {
        sleep(1);
        sem_post(&sem);
        sem_getvalue(&sem,&semval);
        printf("producter: semval = %d\n",semval);
    }
}

void *consumer_f(void * arg)
{
    int semval =0;
    while(running)
    {
        sleep(1);
        sem_wait(&sem);
        sem_getvalue(&sem,&semval);
        printf("consumer: semval = %d\n",semval);
    }
}
