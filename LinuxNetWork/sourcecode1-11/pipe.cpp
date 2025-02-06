#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    int result = -1;
    int fd[2],nbytes;
    pid_t pid;
    int *write_fd = &fd[1];
    int *read_fd = &fd[0];
    char message[] = "Hello, world!";

    result = pipe(fd);
    if(-1 == result){
        printf("pipe failed\n");
        exit(1);
    }
    pid = fork();
    if(-1 == pid){
        printf("fork failed\n");
        exit(1);
    }
    if(0 == pid){
        close(*read_fd);
        nbytes = write(*write_fd, message, strlen(message));
    }
    else{
        close(*write_fd);
        nbytes = read(*read_fd, message, strlen(message));
        printf("nbytes = %d, message = %s\n", nbytes, message);
    }
    
    return 0;
}