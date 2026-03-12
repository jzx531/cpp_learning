#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syslog.h>
#include <unistd.h>

extern void *__libc_malloc(size_t size); //malloc.h里面定义的
int enable_malloc_hook = 1;              //终止递归的变量（具体参考malloc函数说明）

extern void __libc_free(void *p);
int enable_free_hook = 1;

void *malloc(size_t size)
{
    if (enable_malloc_hook)
    { //调用系统
        enable_malloc_hook = 0;
        void *p = __libc_malloc(size); //分配内存，系统的malloc实际上也是调用的这个api进行内存分配。

        //返回malloc调用完成时的地址，可以结合addr2line命令定位到哪一行内存泄露。
        void *caller = __builtin_return_address(0);
        char buff[128] = {0};
        sprintf(buff, "./mem/%p.mem", p); //使用malloc返回的地址作为文件名(p.mem)
        FILE *fp = fopen(buff, "w");
        fprintf(fp, "[+%p] --> addr:%p, size:%ld\n", caller, p, size);
        fflush(fp);
        //fclose(fp);//注意不能close文件
        //printf函数内部会调用malloc，如果不用enable_malloc_hook变量会导致递归malloc的使用
        printf("malloc :%p\n", p);
        //保证下次调用malloc进入到if，注意，多线程不是线程安全的
        enable_malloc_hook = 1;

        return p;
    }
    else
    { //如果是其他API(比如printf)调用了malloc，会直接调用__lib_malloc(size_t size)进行分配，从而使得递归得以退出

        return __libc_malloc(size);
    }
}

void free(void *p)
{
    if (enable_free_hook)
    { //调用free
        enable_free_hook = 0;
        __libc_free(p);
        char buff[128] = {0};
                sprintf(buff, "./mem/%p.mem", p);
        if (unlink(buff) < 0) //删除文件，如果返回小于0，说明释放了2次
        {
            printf("double free:%p\n", p);
        }
        printf("free:%p\n", p);
        enable_free_hook = 1; //保证下次free,还走if
    }
    else
    { //其他系统API调用会直接调用__libc_free.
        __libc_free(p);
    }
}
int main()
{
    void *p1 = malloc(10);
    void *p2 = malloc(20);
    free(p1);
    return 0;
}