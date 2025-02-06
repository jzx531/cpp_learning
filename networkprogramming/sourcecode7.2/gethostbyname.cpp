#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    if(argc!=2){
        printf("Usage : %s hostname\n", argv[0]);
        exit(1);
    }
    host = gethostbyname(argv[1]);
    if(host == NULL){
        ErrorHandling("gethostbyname() error");
    }
    printf("Official name: %s\n", host->h_name);
    for(i=0; host->h_aliases[i]!=NULL; i++){
        printf("Alias name: %s\n", host->h_aliases[i]);
    }
    printf("Address type: %s\n", host->h_addrtype==AF_INET? "AF_INET" : "AF_INET6");
    for(i=0; host->h_addr_list[i]!=NULL; i++){
        printf("IP Address: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }
    return 0;
}