#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define BUF_SIZ 30

int main(int argc,char *argv[])
{
    fd_set reads,temps;
    int result ,str_len;
    char buf[BUF_SIZ];
    struct timeval timeout;
    FD_ZERO(&reads);
    FD_SET(0,&reads);
    while(1)
    {
        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(1,&temps,NULL,NULL,&timeout);
        if (result == -1)
        {
            puts("select error");
            break;
        }
        else if(result == 0)
        {
            printf("timeout\n");
        }
        else{
            if(FD_ISSET(0,&temps))
            {
                str_len = read (0,buf,BUF_SIZ);
                buf[str_len] = '\0';
                printf("recv from client: %s\n",buf);
            }
        }
    }
    return 0;
}