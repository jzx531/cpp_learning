#include <myclass.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/ipc.h>
int main()
{
    int ret =-1;
    int msg_flags,msg_id;
    key_t key;
    struct msgmbuf{
        int mtype;
        char mtext[10];
    };
    struct msqid_ds msg_info;
    struct msgmbuf msg_buf;

    int msg_sflags,msg_rflags;
    char *msgpath ="./Makefile";
    key = ftok(msgpath, 'b');
    if(key == -1)
    {
        printf("ftok failed\n");
    }
    else
    {
        printf("key = %d\n",key);
    }

    msg_flags = IPC_CREAT | IPC_EXCL;
    msg_id = msgget(key, msg_flags|0x0666);
    if(msg_id == -1)
    {
        printf("msgget failed\n");
    }
    else
    {
        printf("msg_id = %d\n",msg_id);
    }
    
    msg_show_attr(msg_id,&msg_info);
    msg_sflags = IPC_NOWAIT;
    msg_buf.mtype = 10;
    memcpy(msg_buf.mtext,"test message",sizeof("test message"));
    ret = msgsnd(msg_id, &msg_buf, sizeof(msg_buf.mtext), msg_sflags);
    if(ret == -1)
    {
        printf("msgsnd failed\n");
    }
    else
    {
        printf("send message success\n");
    }
    msg_show_attr(msg_id,&msg_info);
    msg_info.msg_qnum = 0;

    msg_rflags = IPC_NOWAIT|MSG_NOERROR;
    ret = msgrcv(msg_id, &msg_buf, sizeof(msg_buf.mtext), 10, msg_rflags);
    if(ret == -1)
    {
        printf("msgrcv failed\n");
    }
    else
    {
        printf("receive message success\n");
    }
    msg_show_attr(msg_id,&msg_info);

    msg_info.msg_perm.uid = 8;
    msg_info.msg_perm.gid = 8;
    msg_info.msg_qbytes = 666;
    ret = msgctl(msg_id, IPC_SET, &msg_info);
    if(ret == -1)
    {
        printf("msgctl failed\n");
    }
    else
    {
        printf("set message attribute success\n");
    }
    msg_show_attr(msg_id,&msg_info);

    ret = msgctl(msg_id, IPC_RMID, NULL);
    if(ret == -1)
    {
        printf("msgctl failed\n");
    }
    else
    {
        printf("remove message queue success\n");
    }
    return 0;
}
