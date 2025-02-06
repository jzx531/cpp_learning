#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
int main(void){
    fd_set rd;
    struct timeval timeout;
    int err;

    FD_ZERO(&rd);
    FD_SET(0, &rd);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    err = select(1, &rd, NULL, NULL, &timeout);
    if(err == -1){
        perror("select error");
        return 1;
    }
    else if(err){
        printf("data is available\n");
    }
    else{
        printf("timeout\n");
    }
    return 0;
}