#include "smallchatServer.h"

/* ============================================================================
 * 简单聊天服务器 - smallchat-server.c
 * 
 * 这个程序实现了一个基本的聊天服务器，读取客户端输入并广播给所有其他连接的客户端。
 * 功能包括：
 * - 接受新客户端连接
 * - 处理客户端消息并广播
 * - 支持/nick命令设置昵称
 * - 使用select()进行多路复用I/O
 * ============================================================================ */


struct chatState *Chat; // 在启动时初始化

/* 创建一个绑定到'fd'的新客户端。在新客户端连接时调用。
 * 作为副作用，更新全局Chat状态。
 * 参数：fd - 客户端套接字描述符
 * 返回值：新创建的客户端结构体指针 */
struct client *createClient(int fd)
{
    char nick[32];//创建初始昵称
    int nicklen = snprintf(nick, sizeof(nick), "User%d", fd);
    struct client *c = chatMalloc(sizeof(struct client));
    //设置非阻塞
    socketSetNonBlockNoDelay(fd);//假设不会失败,设置为非阻塞无延迟
    c->fd = fd;
    c->nick = chatMalloc(nicklen+1);//分配昵称内存
    memcpy(c->nick, nick, nicklen);//复制昵称
    assert(Chat->clients[c->fd] == NULL);//这个槽位应该可用
    Chat->clients[c->fd]=c;
    /*如果需要,更新最大客户端编号*/
    if(c->fd > Chat->maxclient) Chat->maxclient = c->fd;
    Chat->numclients++;
    return c;
}

/* 释放客户端、相关资源，并从全局Chat状态中解绑。
 * 参数：c - 要释放的客户端结构体指针 */
void freeClient(struct client *c)
{
    free(c->nick);//释放昵称内存
    close(c->fd);//关闭套接字
    Chat->clients[c->fd] = NULL;//从数组中移除
    Chat->numclients--;//减少客户端计数
    if(Chat->maxclient == c->fd){
        /* 这是最大的客户端编号,找到最新槽位*/
        int j;
        for(j=Chat->maxclient-1; j>=0; j--){
            if(Chat->clients[j]!= NULL) {
                Chat->maxclient = j;
                break;
            }
        }
        if(j == -1)Chat -> maxclient = -1; // 没有可用的槽位
    }
    free(c);//释放客户端结构体内存
}

/* 分配并初始化全局数据。 */
void initChat(void)
{
    Chat = chatMalloc(sizeof(struct chatState));
    memset(Chat, 0, sizeof(struct chatState));
    Chat->numclients = 0;
    Chat->maxclient = -1;

    /* 创建监听套接字 */
    Chat->serversock = createTCPServer(SERVER_PORT);
    if (Chat->serversock == -1) {
        perror("createTCPServer");
        exit(1);
    }
}

/* 将指定的字符串发送给所有连接的客户端，除了套接字描述符为'excluded'的那个。
 * 如果想发送给所有客户端，只需将excluded设为不可能的套接字：-1。
 * 参数：excluded - 排除的客户端套接字，s - 要发送的字符串，len - 字符串长度 */
void sendMsgToAllClientsBut(int excluded, char *s, size_t len) {
    for (int j = 0; j <= Chat->maxclient; j++) {
        if (Chat->clients[j] == NULL ||
            Chat->clients[j]->fd == excluded) continue;  // 跳过NULL或排除的客户端

        /* 重要：我们不做任何缓冲。只使用内核套接字缓冲区。
         * 如果内容不适合，我们不管。这需要保持程序简单。 */
        write(Chat->clients[j]->fd,s,len);  // 发送消息
    }
}

/* main()函数实现主要的聊天逻辑：
 * 1. 如果有，接受新客户端连接。
 * 2. 检查是否有客户端发送了新消息。
 * 3. 将消息发送给所有其他客户端。 */
int smallchatServer(void)
{
    initChat(); // 初始化全局数据

    while(1){
        fd_set rfds;
        struct timeval tv;
        int retval;

        /* 等待客户端连接 */
        FD_ZERO(&rfds);//清空文件描述符集
        FD_SET(Chat->serversock, &rfds);//监听套接字

        //添加所有客户连接套接字
        for(int i=0; i<=Chat->maxclient; i++){
            if(Chat->clients[i]!= NULL && Chat->clients[i]->fd >= 0)
            {
                FD_SET(Chat->clients[i]->fd, &rfds);
            }
        }

        //设置超时时间
        tv.tv_sec = 1;
        tv.tv_usec = 0;


        int maxfd = Chat->serversock;
        if(Chat->maxclient > maxfd) maxfd = Chat->maxclient;

        /* 等待I/O事件 */
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            perror("select");
            exit(1);
        }else if (retval) {
            /* 有I/O事件发生 */
            if (FD_ISSET(Chat->serversock, &rfds)) {
                /* 有新客户端连接 */
                int fd = accept(Chat->serversock, NULL, NULL);
                struct client *c = createClient(fd);
                                /* 发送欢迎消息 */
                char *welcome_msg =
                    "Welcome to Simple Chat! "
                    "Use /nick <nick> to set your nick.\n";
                write(c->fd,welcome_msg,strlen(welcome_msg));  // 发送欢迎消息
                printf("Connected client fd=%d\n", fd);  // 打印连接信息
            }

            /* 这里为每个连接的客户端，检查是否有客户端发送的待处理数据。 */
            char readbuf[256];
            for(int j = 0; j<=Chat->maxclient; j++){
                if(Chat->clients[j] == NULL) continue; //跳过NULL或已关闭的客户端
                if(FD_ISSET(j, &rfds)){
                    int n = read(j, readbuf, sizeof(readbuf));
                    if(n <= 0){
                        /* 客户端关闭连接 */
                        printf("Client fd=%d disconnected\n", j);
                        freeClient(Chat->clients[j]);
                    }else{
                        /* 客户端发送消息 */
                        struct client *c = Chat->clients[j];
                        readbuf[n] = '\0'; // 确保字符串以'\0'结尾
                        /* 如果用户消息以"/"开头，我们将其作为客户端命令处理。
                         * 到目前为止，只实现了/nick <newnick>命令。 */
                        if(readbuf[0] == '/'){
                            /* 移除任何尾随换行符 */
                            char *p;
                            p = strchr(readbuf,'\r'); if (p) *p = 0;
                            p = strchr(readbuf,'\n'); if (p) *p = 0;
                            /* 检查命令后的参数,在空格之后*/
                            char *arg = strchr(readbuf,' ');
                            if(arg){
                                *arg = '\0'; // 截断命令
                                arg++; // 跳过空格
                            }
                            if(!strcmp(readbuf,"/nick")&& arg){
                                // 如果是nick命令
                                free(c->nick); // 释放旧昵称
                                int nicklen = strlen(arg);
                                c->nick = chatMalloc(nicklen+1);
                                memcpy(c->nick, arg, nicklen+1); // 复制新昵称
                            }else{
                                /* 不支持的命令,发送错误*/
                                char *err_msg = "Unknown command. Use /nick <newnick> to set your nick.\n";
                                write(c->fd,err_msg,strlen(err_msg));  // 发送错误信息
                            }
                        }else{
                            /* 创建要发送给所有人（并在服务器控制台上显示）的消息，格式为：
                             *   nick> some message. */
                            char msg[256];
                            int msglen = snprintf(msg, sizeof(msg), "%s> %s", c->nick, readbuf);//格式化消息

                            /*如果消息太长，截断*/
                            if(msglen > (int)sizeof(msg)-1) msglen = sizeof(msg)-1;
                            msg[msglen] = '\0'; // 确保字符串以'\0'结尾
                            printf("%s\n", msg);  // 打印消息到服务器控制台
                            sendMsgToAllClientsBut(j, msg, msglen);  // 发送消息给所有客户端，除了发送者
                        }
                    }
                }
            }
        }else{
            /* 超时，继续等待 */
        }
    }
    return 0;
}







