#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syslog.h>
#include <unistd.h>

typedef void *(*malloc_hook_t)(size_t size, const void *caller); //系统的__malloc_hook实际的函数类型
malloc_hook_t malloc_f;                                          //用于保存系统默认的__malloc_hook函数地址
typedef void (*free_hook_t)(void *p, const void *caller);        //系统的__free_hook的实际函数类型
free_hook_t free_f;                                              //用于保存系统默认的__free_hook函数指针地址
int replaced = 0;                                                //如果为1，malloc/free指向我们自定义的函数
void mem_trace(void);                                            //让其malloc指向我们自己定义的函数
void mem_untrace(void);                                          //让其free指向我们自己定义的函数
//自定义的malloc函数，与系统的__malloc_hook保持一致
//caller参数代表调用该函数的地址(__builtin_return_address(0)返回的地址就是这个地址)
void *malloc_hook_f(size_t size, const void *caller)
{
    //防止递归-如果不加这句，会让下面的malloc继续执行malloc_hook_f，从而造成递归
    //我们只要得到caller指针这个值就可以了。
    mem_untrace();
    void *ptr = malloc(size);
    //printf("+%p: addr[%p]\n", caller, ptr);
    char buff[128] = {0};
    sprintf(buff, "./mem/%p.mem", ptr);
    FILE *fp = fopen(buff, "w");
    fprintf(fp, "[+%p] --> addr:%p, size:%ld\n", caller, ptr, size);
    fflush(fp);
    fclose(fp);  //free
    mem_trace(); //保证下次malloc还是用我们自定义的
    return ptr;
}
void free_hook_f(void *p, const void *caller)
{
    mem_untrace(); //防止free函数递归
    //printf("-%p: addr[%p]\n", caller, p);
    char buff[128] = {0};
    sprintf(buff, "./mem/%p.mem", p);
    if (unlink(buff) < 0)
    { // no exist
        printf("double free: %p\n", p);
        return;
    }
    free(p);
    mem_trace();
}
void mem_trace(void)
{ //mtrace
    replaced = 1;
    malloc_f = __malloc_hook;      //__malloc_hook是系统本身提供的函数指针(会在malloc调用时初始化)
    free_f = __free_hook;          //__free_hook是系统本身提供的(free调用时会初始化)
    __malloc_hook = malloc_hook_f; //指向我们自定义函数，malloc会调用我们定义的函数
    __free_hook = free_hook_f;     //指向我们自定义函数，free会调用我们自定义的函数
}
//
void mem_untrace(void)
{
    __malloc_hook = malloc_f;
    __free_hook = free_f;
    replaced = 0;
}

int main()
{
    mem_trace();
    void *p1 = malloc(10);
    void *p2 = malloc(20);
    free(p1);
    mem_untrace();
    return 0;
}