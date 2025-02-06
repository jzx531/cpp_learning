#include "myclass.h"
#include <iostream>

#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>
#include <string.h>
#include <errno.h>

void msg_show_attr(int msg_id, struct msqid_ds *msg_info) {
    int ret = -1;
    sleep(1);

    // 获取消息队列状态
    ret = msgctl(msg_id, IPC_STAT, msg_info);
    if (ret == -1) {
        fprintf(stderr, "msgctl error: %s\n", strerror(errno));
        return;
    }

    // 打印消息队列属性
    printf("msg_id: %d\n", msg_id);
    printf("msg_info.msg_perm.uid: %d\n", msg_info->msg_perm.uid);
    printf("msg_info.msg_perm.gid: %d\n", msg_info->msg_perm.gid);
    printf("msg_info.msg_perm.cuid: %d\n", msg_info->msg_perm.cuid);
    printf("msg_info.msg_perm.cgid: %d\n", msg_info->msg_perm.cgid);

    // 格式化权限模式
    printf("msg_info.msg_perm.mode: %o\n", msg_info->msg_perm.mode & 0777);

    // 格式化时间戳
    printf("msg_info.msg_stime: %s", ctime(&msg_info->msg_stime));
    printf("msg_info.msg_rtime: %s", ctime(&msg_info->msg_rtime));
    printf("msg_info.msg_ctime: %s", ctime(&msg_info->msg_ctime));

    printf("msg_info.msg_qnum: %ld\n", msg_info->msg_qnum);
    printf("msg_info.msg_qbytes: %ld\n", msg_info->msg_qbytes);
}

#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
typedef int sem_t;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} arg;

sem_t CreateSem(key_t key, int value)
{
    union semun sem;  //信号量参数
    sem_t semid;      //信号量ID     
    sem.val = value;  //初始化信号量的值
    semid = semget(key, 1, IPC_CREAT | 0666);  //创建信号量
    if(semid == -1)
    {
        printf("create semaphore error\n");
        return -1;
    }
    semctl(semid,0,SETVAL,sem);
    return semid;
}
int Sem_P(sem_t semid)
{
    struct sembuf sops={0,+1,IPC_NOWAIT};
    return semop(semid,&sops,1);
}

int Sem_V(sem_t semid)
{
    struct sembuf sops={0,-1,IPC_NOWAIT};
    return semop(semid,&sops,1);
}
void SetvalueSem(sem_t semid, int value)
{
    union semun sem;
    sem.val = value;
    semctl(semid, 0, SETVAL, sem);
}

int GetvalueSem(sem_t semid)
{
    union semun sem;
    sem.val =0;
    semctl(semid,0,IPC_RMID,sem);
}
void DestroySem(sem_t semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid, 0, IPC_RMID, sem);
}

