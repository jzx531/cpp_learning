#include <stdio.h>
#include <fcntl.h>

int main(void)
{
    FILE *fp;
    int fd = open("test.txt", O_WRONLY|O_CREAT|O_TRUNC);
    if (fd == -1) {
        fputs("Error opening file\n", stdout);
        return -1;
    }
    fp = fdopen(fd, "w");
    fputs("Hello, world!\n", fp);
    fclose(fp);
    return 0;   
    
}