#include<sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static void display_error(const char *on_what)
{
    perror(on_what);
    exit(1);
}

int main(int argc, char *argv[])
{
    int error;
    int sock_UNIX;
    struct sockaddr_un addr_UNIX;
    int len_UNIX;
    const char path[]= "/tmp/my_socket";

    sock_UNIX = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_UNIX == -1) {
        display_error("socket");
    }
    unlink(path);
    memset(&addr_UNIX, 0, sizeof(addr_UNIX));
    addr_UNIX.sun_family = AF_LOCAL;
    strcpy(addr_UNIX.sun_path, path);
    len_UNIX = sizeof(addr_UNIX.sun_family) + strlen(addr_UNIX.sun_path);
    error = bind(sock_UNIX, (struct sockaddr *) &addr_UNIX, len_UNIX);
    if (error == -1) {
        display_error("bind");
    }
    close(sock_UNIX);
    unlink(path);
    return 0;
}