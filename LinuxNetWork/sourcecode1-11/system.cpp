#include <stdio.h>
#include <stdlib.h>

int main()
{
    int ret;
    ret = system("ls -l");
    printf("system() returned %d\n", ret);
    return 0;
}
