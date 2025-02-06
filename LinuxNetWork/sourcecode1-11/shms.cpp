#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <myclass.h>

static char msg[]="Hello World!";

int main()
{
    key_t key;
    int semid,shmid;
    char i,*shms,*shmc;
    struct semid_ds buf;
    int value = 0;
    char buffer[80];
    pid_t p;

    key = ftok("./Makefile", 'a');
    shmid = shmget(key, 1024, IPC_CREAT | 0666);

    semid =CreateSem(key,0);
    p = fork();
    if(p>0){
        shms = (char*)shmat(shmid, 0, 0);
        memcpy(shms,msg,strlen(msg)+1);
        sleep(10);
        Sem_P(semid);
        shmdt(shms);
        DestroySem(semid);
    }
    else if(p==0){
        shmc = (char*)shmat(shmid, 0, 0);
        Sem_V(semid);
        printf("Message from shared memory: %s\n",shmc);
        shmdt(shmc);
    }
    return 0;
}