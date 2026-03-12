#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// 确保目录存在
void ensure_mem_dir() {
    mkdir("./mem", 0755);
}

// 真正的分配逻辑
void *my_malloc_trace(size_t size, const char *file, const char *func, int line) {
    // 【关键】在这里暂时取消宏定义，防止递归调用自己
    #undef malloc
    void *p = malloc(size); 
    // 恢复宏定义（如果在其他文件还需要用）
    #define malloc(size) my_malloc_trace(size, __FILE__, __FUNCTION__, __LINE__)

    if (!p) return NULL;

    ensure_mem_dir();

    char str[256];
    // 格式化地址，去掉 0x 前缀可能更干净，但保留也可以
    snprintf(str, sizeof(str), "./mem/%p.mem", p);

    FILE *fp = fopen(str, "w");
    if (fp) {
        fprintf(fp, "[info:] file:%s, func:%s, line:%d, addr:%p, size:%zu\n", 
                file, func, line, p, size);
        fclose(fp);
    } else {
        perror("Failed to create trace file");
    }

    return p;
}

void my_free_trace(void *p, const char *file, const char *func, int line) {
    if (!p) return;

    ensure_mem_dir();

    char str[256];
    snprintf(str, sizeof(str), "./mem/%p.mem", p);

    // 【关键】暂时取消宏定义
    #undef free
    int ret = unlink(str);
    if (ret != 0) {
        // 注意：unlink 失败不一定是 double free，可能是文件本来就没创建成功
        // 这里仅做提示，不要随意判定为 double free
        fprintf(stderr, "[Warning] Trace file not found for %p (at %s:%d)\n", p, file, line);
    }
    
    free(p); // 调用真正的 free
    
    // 恢复宏
    #define free(p) my_free_trace(p, __FILE__, __FUNCTION__, __LINE__)
}

// 定义宏，替换标准函数
#define malloc(size) my_malloc_trace(size, __FILE__, __FUNCTION__, __LINE__)
#define free(p) my_free_trace(p, __FILE__, __FUNCTION__, __LINE__)

int main() {
    void *p1 = malloc(10);
    void *p2 = malloc(20);
    
    if (p1) free(p1);
    if (p2) free(p2); // 修复原代码中 p2 未 free 的问题
    
    return 0;
}