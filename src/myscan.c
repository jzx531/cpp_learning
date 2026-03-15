#include"myscan.h"

unsigned int portlist[65536];           // 端口池，portlist[0]存储端口数量，portlist[1..n]为端口号
unsigned int Threads = DEFAULT_THREADS; // 实际使用的线程数
int debug = 0;                          // 调试标志，1为输出调试信息，0为不输出

const struct ScanData NILDATA; // 初始化用的空数据，所有字段为0
pthread_attr_t t_c;            // 线程属性，用于设置线程分离态

// 打印程序用法说明，包含参数格式和示例
void usage() {
    printf("Usage:\n"
           "program -p Port1[,Port2,Port3...] [-t Thread](default 10) [-d](DEBUG) StartIp EndIp\n"
           "Example:myscan -p 80 192.168.1.1 192.168.1.254\n"
           "        myscan -p 21,22,23,80,443,8080 -t 256 192.168.1.1 192.168.1.254\n");
    // -p 指定端口，多个端口用逗号分隔
    // -t 指定线程数，默认10
    // -d 开启调试输出
    // StartIp/EndIp 指定扫描的起止IP
}

// 初始化网络和线程属性，程序启动时必须调用
void Init() {
    WSADATA wd;
    int ret = 0;
    // 初始化Winsock2，必须在使用socket前调用
    ret = WSAStartup(MAKEWORD(2, 2), &wd); // 版本2.2
    if (ret != 0) {
        printf("Init Fail");
        exit(1);
    }
    // 检查返回的版本号是否为2.2
    if (HIBYTE(wd.wVersion) != 2 || LOBYTE(wd.wVersion) != 2) {
        printf("Init Fail");
        WSACleanup();
        exit(1);
    }

    // 初始化线程属性，设置为分离态（线程结束后自动回收资源）
    pthread_attr_init(&t_c);
    pthread_attr_setdetachstate(&t_c, PTHREAD_CREATE_DETACHED);
}

// 把10进制无符号整形ip转换为点分十进制IP字符串
// 例如：3232235777(11000000 10101000 00000001 00000001) -> "192.168.1.1"
// 注意：返回的字符串需手动free
char *ipback(unsigned int ip) {
    char *ipstr = (char *)malloc(17*sizeof(char)); // 16字节足够存储点分十进制IP字符串
    sprintf(ipstr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    return ipstr;
}

// 把IP地址字符串（如"192.168.1.1"）转换为10进制无符号整形
// 检查每段是否合法（0~255），非法则退出
unsigned int getip(char *ip)
{
    char myip[20] = "";
    strcpy(myip, ip); // 拷贝一份，避免修改原始字符串

    char * temp = strtok(myip, "."); // 按"."分割IP地址
    unsigned int ip_add = 0;    // 最终结果
    for(int i=0; i<4; i++)
    {
        if(temp == NULL || strlen(temp) == 0|| strlen(temp) > 3) // 字符串中不包含"."，不合法
        {
            printf("IP address format error!\n");
            exit(1);
        }
        int num = atoi(temp); // 转换为整数
        if(num < 0 || num > 255) // 超出范围，不合法
        {
            printf("IP address format error!\n");
            exit(1);
        }
        ip_add = ip_add << 8 | num; // 左移8位或或，合并为无符号整形
        temp = strtok(NULL, "."); // 继续按"."分割
    }
    if(temp!= NULL){
        printf("IP address format error!\n");
        exit(1);
    }
    return ip_add;
}

// 端口扫描主控函数，负责分配线程进行端口扫描
// StartIp: 起始IP（整数），EndIp: 结束IP，Thread: 每批次线程数
void scan(unsigned int StartIp, unsigned int EndIp, unsigned int Thread)
{
    if(StartIp > EndIp)
    {
        usage();
        printf("StartIp must less than EndIp!\n");
        exit(1);
    }
    int port_p = 1;//当前扫描的端口在portlist中的下标
    for(int i = StartIp; i <= EndIp;)
    {
        int last = EndIp - i; // 计算本批次要处理的IP数量
        if (last >= Thread) {
            last = Thread; // 最多不超过线程数
        } else if (last == 0)
            last = 1; // 只剩一个IP
        
        struct ScanData pData[last];//本批次线程数据
        for (int k = 0; k < last; k++)
            pData[k] = NILDATA; // 初始化扫描数据为空
        
        pthread_t t[last]; // 线程句柄数组
        for(int j = 0;j<last;j++){
            pData[j].ip =i;//当前ip
            pData[j].port = portlist[port_p++];//当前端口
            // 如果端口池用完，重置端口下标并切换到下一个IP
            if (port_p > portlist[0]) {
                port_p = 1;
                i++;
            }
            // 创建线程进行端口扫描，传递pData[j]地址
            if (pthread_create(&t[j], &t_c, threadscan, (void *)&pData[j]) != 0)
                printf("\nCREATE THREAD ERROR\n");
            else
                pthread_join(t[j], NULL); // 等待线程结束（此处为串行，实际可优化为并发）
            Sleep(10); // 稍作延迟，防止线程过快创建，减轻系统压力
        }
        Sleep(1000);// 每轮结束后延迟，避免过快扫描
    }
    Sleep(3000);// 扫描全部结束后延迟，确保输出完整
}

// 线程扫描函数，尝试连接指定IP和端口
// sd: 指向ScanData结构体的指针
void *threadscan(void *sd) {
    struct ScanData *pa = (struct ScanData *)sd;//取出参数
    char ip[20] = "";
    sprintf(ip,"%u",pa->ip);//将整数IP转为字符串(inet_addr为字符串)
    if (debug)
        printf("Testing %-16s %d ...\n", ipback(pa->ip), pa->port); // 输出调试信息

    SOCKET c;
    SOCKADDR_IN saddr;

    // 创建客户端socket，AF_INET表示IPv4，SOCK_STREAM表示TCP
    c = socket(AF_INET, SOCK_STREAM, 0);
    // 填充目标服务器信息结构体
    saddr.sin_addr.S_un.S_addr = inet_addr(ip); // 目标IP
    saddr.sin_family = AF_INET;                 // 地址族
    saddr.sin_port = htons(pa->port);           // 端口号，主机字节序转网络字节序
    // 尝试连接目标主机端口
    if (connect(c, (SOCKADDR *)&saddr, sizeof(SOCKADDR)) != -1) {
        // 连接成功，说明端口开放
        printf("%-16s %d Open\n", ipback(pa->ip), pa->port);
    }
    closesocket(c); // 关闭socket
    pthread_exit(NULL); // 线程退出
    return NULL;
}

int myscan(int argc, char *argv[])
{
    Init(); // 初始化网络和线程属性，必须最先调用
    char opt;
    if (argc < 4) { // 参数数量不足，至少要有程序名、-p端口、起始IP、结束IP
        usage();
        WSACleanup();
        return 1;
    }
    // 解析命令行参数，支持-p、-t、-d
    while ((opt = getopt(argc, argv, "p:t:d")) != -1) {
        switch (opt) {
        case 'p': // 端口参数，支持单端口或逗号分隔的多个端口
        {
            portlist[0] = 1; // 先假定只有一个端口
            char *p = NULL;
            char *optarg_copy = strdup(optarg); // 复制端口参数，避免strtok破坏原始字符串
            // 统计端口数量并检查合法性
            for (p = optarg_copy; *p != '\0'; p++) {
                if (*p == ',' && *(p + 1) != ',' && *(p + 1) != '\0') // 统计逗号分隔的端口数
                    portlist[0]++;
                else if (*p < '0' || *p > '9') { // 检查端口字符是否合法
                    printf("!!!PORT ERROR!!!  Check your port set.\n");
                    free(optarg_copy);
                    exit(1);
                }
            }

            int i = 1; // 端口池下标
            if (portlist[0] != 1) { // 多端口
                if ((p = strtok(optarg_copy, ",")) != NULL) { // 分割字符串，分离端口
                    int temp = atoi(p);
                    if (temp <= 0 || temp > 65535) { // 检查端口范围
                        printf("!!!PORT ERROR!!!  Check your port set.\n");
                        free(optarg_copy);
                        exit(1);
                    }
                    portlist[i++] = temp;
                    while ((p = strtok(NULL, ",")) != NULL) { // 继续分割
                        int temp = atoi(p);
                        if (temp <= 0 || temp > 65535) {
                            printf("!!!PORT ERROR!!!  Check your port set.\n");
                            free(optarg_copy);
                            exit(1);
                        }
                        portlist[i++] = temp;
                    }
                }
            } else { // 单端口
                int temp = atoi(optarg_copy);
                if (temp <= 0 || temp > 65535) {
                    printf("!!!PORT ERROR!!!  Check your port set.\n");
                    free(optarg_copy);
                    exit(1);
                }
                portlist[i++] = temp;
            }
            free(optarg_copy); // 释放内存
            break;
        }
        case 't': // 线程数参数
        {
            int temp = atoi(optarg);
            if (temp <= 0) { // 线程数必须大于0
                printf("!!!THREAD ERROR!!!  Check your thread set.");
                exit(1);
            } else
                Threads = temp; // 设置线程数
            break;
        }
        case 'd': // debug参数，开启调试输出
            debug = 1;
            break;
        default:
            usage();
            exit(1);
            break;
        }
    }
    // 扫描指定IP段和端口，调用主控函数
    scan(getip(argv[argc - 2]), getip(argv[argc - 1]), Threads); 
    WSACleanup(); // 清理网络资源，释放Winsock
    return 0;
}