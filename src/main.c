#include "smallchatServer.h"
#include "smallchatClient.h"

int main(int argc, char* argv[]) {
    if(argc < 2){
        printf("choose server or client mode\n");
        return 0;
    }
    if(strcmp(argv[1], "server") == 0){
        smallchatServer();
    }
    else if(strcmp(argv[1], "client") == 0){
        smallchatClient(argc, argv);
    }
    else{
        printf("choose server or client mode\n");
        return 0;
    }
    return 0;
}