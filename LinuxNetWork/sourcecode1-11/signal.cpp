#include <signal.h>
#include <stdio.h>

static void sig_handle(int signo)
{
    if(signo == SIGSTOP)
        printf("SIGSTOP received\n");
    else if(signo == SIGKILL)
        printf("SIGKILL received\n");
    else{
        printf("Signal %d received\n", signo);
    }
}

int main()
{
    sighandler_t ret;
    ret = signal(SIGINT, sig_handle);
    if(ret == SIG_ERR){
        printf("signal() failed\n");
        return -1;
    }
    printf("Press Ctrl-C to send SIGINT\n");
    ret = signal(SIGKILL, sig_handle);
    if(ret == SIG_ERR){
        printf("signal() failed\n");
        return -1;
    }
    for(;;);
    return 0;
}
    
