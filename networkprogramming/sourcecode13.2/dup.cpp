#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){
    int cfd1, cfd2;
    char str1[] = "Hi ~\n";
    char str2[] = "Hello ~\n";

    cfd1 = dup(1); // duplicate stdout to cfd1
    cfd2 = dup2(cfd1, 7); // duplicate cfd1 to cfd2 (file descriptor 7)

    printf("cfd1 = %d, cfd2 = %d\n", cfd1, cfd2);
    write(cfd1, str1, strlen(str1));
    write(cfd2, str2, strlen(str2));

    close(cfd1);
    close(cfd2);
    write(1, str1, strlen(str1));
    close(1);
    write(1, str2, strlen(str2)); // 使用 stderr (文件描述符 2) 进行输出
    return 0;
}