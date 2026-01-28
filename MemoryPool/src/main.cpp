#include "memory_pool.h"
#include <stdio.h>

int main(){
    memory_pool * pool = memory_pool::createPool(1024);
    //-分配小内存
    char*p1 = (char*)memory_pool::poolMalloc(pool,2);
    fprintf(stdout,"little malloc1:%p\n",p1);
    char*p2 = (char*)memory_pool::poolMalloc(pool,4);
    fprintf(stdout,"little malloc2:%p\n",p2);
    char*p3 = (char*)memory_pool::poolMalloc(pool,8);
    fprintf(stdout,"little malloc3:%p\n",p3);
    char*p4 = (char*)memory_pool::poolMalloc(pool,256);
    fprintf(stdout,"little malloc4:%p\n",p4);
    char*p5 = (char*)memory_pool::poolMalloc(pool,512);
    fprintf(stdout,"little malloc5:%p\n",p5);

    //-测试分配不足开辟新的small block
    char*p6 = (char*)memory_pool::poolMalloc(pool,512);
    fprintf(stdout,"little malloc6:%p\n",p6);

    //-测试分配大内存
    char*p7 = (char*)memory_pool::poolMalloc(pool,2048);
    fprintf(stdout,"big malloc1:%p\n",p7);

    char*p8 = (char*)memory_pool::poolMalloc(pool,4096);
    fprintf(stdout,"big malloc2:%p\n",p8);

    //-测试free大内存
    memory_pool::freeBigBlock(pool, p8);

    //-测试再次分配大内存（我这里测试结果和p8一样）
    char*p9 = (char*)memory_pool::poolMalloc(pool,2048);
    fprintf(stdout,"big malloc3:%p\n",p9);

    //-销毁内存池
    memory_pool::destroyPool(pool);

    exit(EXIT_SUCCESS);
}