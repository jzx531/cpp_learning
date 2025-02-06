#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd = open("/dev/cdrom", O_RDONLY);
    if(fd<0){
        printf("open error\n");
        return -1;
    }
    if(!ioctl(fd,CDROMEJECT,NULL)){
        printf("eject success\n");
    }
    else{
        printf("eject error\n");
    }
    close(fd);
    return 0;
}