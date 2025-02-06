// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>
// #include <sys/wait.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <fcntl.h>
// #include <netinet/in.h>

// #define BUF_SIZ 30
// void ErrorHandling(char *message){
//     fputs(message,stderr);
//     fputc('\n',stderr);
//     exit(1);
// }

// int acpt_sock;
// int recv_sock;
// void urg_handler(int signo)
// {
//     int str_len;
//     char buf[BUF_SIZ];
//     str_len = recv(recv_sock,buf,sizeof(buf)-1,MSG_OOB);
//     str_len = strlen(buf);
//     buf[str_len] = '\0';
//     printf("signoaction");
//     printf("Received urgent data: %s\n",buf);
// }

// int main(int argc,char *argv[])
// {
//     struct sockaddr_in serv_addr,recv_addr;
//     int str_len,state;
//     socklen_t serv_adr_sz;
//     struct sigaction act;
//     char buf[BUF_SIZ];
//     if(argc!=2)
//     {
//         printf("Usage: %s <port>\n",argv[0]);
//         exit(1);
//     }
//     act.sa_handler = urg_handler;
//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;

//     acpt_sock = socket(PF_INET,SOCK_STREAM,0);
//     memset(&recv_addr,0,sizeof(recv_addr));
//     recv_addr.sin_family = AF_INET;
//     recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     recv_addr.sin_port = htons(atoi(argv[1]));
//     if(bind(acpt_sock,(struct sockaddr*)&recv_addr,sizeof(recv_addr))<0)
//         ErrorHandling("bind() error");
//     if(listen(acpt_sock,5)<0)
//         ErrorHandling("listen() error");
//     serv_adr_sz = sizeof(serv_addr);
//     recv_sock = accept(acpt_sock,(struct sockaddr*)&serv_addr,&serv_adr_sz);

//     fcntl(recv_sock,F_SETOWN,getpid());
//     state = sigaction(SIGURG,&act,0);
//     if(state==-1)
//         ErrorHandling("sigaction() error");
//     while((str_len= recv(recv_sock,buf,sizeof(buf)-1,0)!=0))
//     {
//         if(str_len==-1) continue;
//         str_len = strlen(buf);
//         buf[str_len+1] = 0;
//         puts(buf);
//     }
//     close(acpt_sock);
//     close(recv_sock);
//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>

#define BUF_SIZ 30

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int acpt_sock;
int recv_sock;

void urg_handler(int signo) {
    int str_len;
    char buf[BUF_SIZ];
    str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB);
    if (str_len > 0) {
        buf[str_len] = '\0';
        printf("Received urgent data: %s\n", buf);
    } else if (str_len == -1) {
        ErrorHandling("recv() error in urg_handler");
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr, recv_addr;
    int str_len, state;
    socklen_t serv_adr_sz;
    struct sigaction act;
    char buf[BUF_SIZ];

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (acpt_sock == -1) {
        ErrorHandling("socket() error");
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(acpt_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) == -1) {
        ErrorHandling("bind() error");
    }

    if (listen(acpt_sock, 5) == -1) {
        ErrorHandling("listen() error");
    }

    serv_adr_sz = sizeof(serv_addr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_addr, &serv_adr_sz);
    if (recv_sock == -1) {
        ErrorHandling("accept() error");
    }

    if (fcntl(recv_sock, F_SETOWN, getpid()) == -1) {
        ErrorHandling("fcntl() error");
    }

    if (sigaction(SIGURG, &act, NULL) == -1) {
        ErrorHandling("sigaction() error");
    }

    while ((str_len = recv(recv_sock, buf, sizeof(buf) - 1, 0)) != 0) {
        if (str_len == -1) continue;
        buf[str_len] = '\0';
        puts(buf);
    }

    close(acpt_sock);
    close(recv_sock);
    return 0;
}