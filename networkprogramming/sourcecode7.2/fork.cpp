#include <stdio.h>
#include <unistd.h>

int gval=10;
int main(int argc,char *argv[])
{
    pid_t pid;
    int lval=20;
    gval++,lval+=5;
    pid =fork();
    if(pid==0){
        gval+=2,lval+=2;
        printf("child Proc:[%d,%d]\n",gval,lval);
    }
    else {
        gval-=2,lval-=2;
        printf("parent Proc:[%d,%d]\n",gval,lval);
    }
    return 0;
}