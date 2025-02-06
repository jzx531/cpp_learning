// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <netdb.h>

// void ErrorHandling(char *message)
// {
//     fputs(message, stderr);
//     fputc('\n', stderr);
//     exit(1);
// }

// int main(int argc, char *argv[])
// {
//     int i;
//     struct hostent *host;
//     struct sockaddr_in addr;
//     if(argc!= 2) {
//         printf("Usage : %s <IP address>\n", argv[0]);
//         exit(1);    
//     }
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_addr.s_addr = inet_addr(argv[1]);
//     host = gethostbyaddr((char *)&addr.sin_addr,4, AF_INET);
//     if(host == NULL) {
//         ErrorHandling("gethostbyaddr() error");
//     }
//     printf("Official name: %s\n", host->h_name);
//     for(i = 0; host->h_aliases[i] != NULL; i++) {
//         printf("Alias name: %s\n", host->h_aliases[i]);
//     }
//     for(i = 0; host->h_addr_list[i] != NULL; i++) {
//         printf("IP address: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
//     }
//     return 0;
// }

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    return -1;
}

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    struct sockaddr_in addr;
    if (argc != 2) {
        printf("Usage : %s <IP address>\n", argv[0]);
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if (addr.sin_addr.s_addr == INADDR_NONE) {
        return ErrorHandling("Invalid IP address");
    }

    host = gethostbyaddr((char *)&addr.sin_addr.s_addr, 4, AF_INET);
    if (host == NULL) {
        fprintf(stderr, "gethostbyaddr() error: %s\n", hstrerror(h_errno));
        exit(1);
    }

    printf("Official name: %s\n", host->h_name);
    for (i = 0; host->h_aliases[i] != NULL; i++) {
        printf("Alias name: %s\n", host->h_aliases[i]);
    }

    for (i = 0; host->h_addr_list[i] != NULL; i++) {
        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[i], sizeof(addr));
        printf("IP address: %s\n", inet_ntoa(addr));
    }

    return 0;
}