#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

static int run = 1;
static int retvalue;

void *start_routine(void *arg) {
    int *running = (int *)arg;
    printf("线程正在运行；传入参数：%d\n", *running);
    while (*running) {
        printf("线程正在执行中\n");
        sleep(1);
    }

    printf("线程结束\n");
    retvalue = 0;
    pthread_exit((void *)(intptr_t)retvalue); // 将返回值转换为指针类型
}

int main(void) {
    pthread_t pt;
    int ret = -1;
    int times = 3;
    int i = 0;
    void *ret_join = NULL; // 用于接收线程返回值

    ret = pthread_create(&pt, NULL, start_routine, &run); // 传递 run 的地址
    if (ret != 0) {
        printf("线程创建失败\n");
        return 1;
    }

    sleep(1);
    for (i = 0; i < times; i++) {
        printf("主线程正在打印\n");
    }

    run = 0; // 任务结束
    pthread_join(pt, &ret_join); // 等待线程结束，并接收返回值
    printf("线程返回值：%ld\n", (intptr_t)ret_join); // 将指针类型转换回整型

    return 0;
}