#ifndef MYCLASS_H
#define MYCLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/ipc.h>



void msg_show_attr(int msg_id, struct msqid_ds *msg_info);

#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
typedef int sem_t;
// union semun{
//     int val;
//     struct semid_ds *buf;
//     unsigned short *array;
// } arg;

sem_t CreateSem(key_t key, int value);
int Sem_P(sem_t semid);

int Sem_V(sem_t semid);

void SetvalueSem(sem_t semid, int value);

int GetvalueSem(sem_t semid);

void DestroySem(sem_t semid);

#endif // MYCLASS_H