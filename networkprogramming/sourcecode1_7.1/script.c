#include <stdio.h>
#include <sys/socket.h>

int main() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed");
        return 0;
    }
    printf("Socket created successfully\n");
    return 0;
}