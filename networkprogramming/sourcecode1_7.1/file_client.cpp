#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 30
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc,char*argv[])
{
    int sd;
    FILE *fp;
    char buf[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_adr;
    if(argc!=3){
        printf("USAGE:%s<IP><port>\n",argv[0]);
        exit(1);
    }
    fp = fopen("receive.dat","wb");
    sd = socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_adr.sin_port=htons(atoi(argv[2]));

    connect(sd,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
    int i=0;
    while((read_cnt=read(sd,buf,BUF_SIZE))!=0)
    {   
        printf("index:%d\n",i++);
        fwrite((void*)buf,1,read_cnt,fp);
        printf(buf);
    }
    puts("received file data");
    write(sd,"thank you",10);
    fclose(fp);
    close(sd);
    return 0;
}