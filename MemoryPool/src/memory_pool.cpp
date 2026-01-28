#include "memory_block.h"
#include "memory_pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

memory_pool* memory_pool::createPool(size_t capacity)
{
    size_t total_size = sizeof(memory_pool)+sizeof(small_block) + capacity;
    void * temp = malloc(total_size);
    memset(temp,0,total_size);

    memory_pool* pool = (memory_pool*)temp;
    fprintf(stdout,"Memory pool created at %p\n",pool);

    pool->small_buffer_capacity = capacity;
    pool->big_block_start = nullptr;
    pool->cur_usable_small_block=(small_block*)(pool->small_block_start);

    //pool+1是指针偏移了一个pool的地址空间，所以根据上面的malloc，pool+1地址将是small_block的地址
    small_block *sbp =(small_block*)(pool+1);
    fprintf(stdout,"Small block created at %p\n",sbp);

    //与上面对应buffer的偏移对应到capacity大小部分的内存
    sbp->cur_usable_buffer = (char*)(sbp+1);
    fprintf(stdout,"Small block buffer created at %p\n",sbp->cur_usable_buffer);
    sbp->buffer_end = sbp->cur_usable_buffer + capacity;
    sbp->next_block = nullptr;
    sbp->no_enough_times = 0;
}

void* memory_pool::poolMalloc(memory_pool* pool,size_t size)
{
    if(size < pool->small_buffer_capacity)//使用小内存
    {
        small_block *temp = pool->cur_usable_small_block;
        do{
            if(temp->buffer_end - temp->cur_usable_buffer > size)
            {
                char* res = temp->cur_usable_buffer+size;
                temp->cur_usable_buffer = res;
                return res;
            }
            temp = temp->next_block;
        }while(temp);
        return createNewSmallBlock(pool,size);
    }
    return mallocBigBlock(pool,size);
}

char* memory_pool::createNewSmallBlock(memory_pool* pool,size_t size)
{
    size_t malloc_size = sizeof(small_block) + pool->small_buffer_capacity;
    void * temp = malloc(malloc_size);
    memset(temp,0,malloc_size);

    small_block * sbp = (small_block*)temp;
    fprintf(stdout,"New small block created at %p\n",sbp);
    sbp->cur_usable_buffer = (char*)(sbp+1);
    fprintf(stdout,"New small block buffer created at %p\n",sbp->cur_usable_buffer);
    sbp->buffer_end = (char*)temp+malloc_size;
    sbp->next_block = nullptr;
    sbp->no_enough_times = 0;

    char* res = sbp->cur_usable_buffer;
    sbp->cur_usable_buffer =res+ size;

    small_block * p = pool -> cur_usable_small_block;
    while(p->next_block){
        if(p->no_enough_times>4){
            pool -> cur_usable_small_block = p->next_block;
        }
        p->no_enough_times++;
        p = p->next_block;
    }
    p->next_block = sbp;

    if(pool->cur_usable_small_block == nullptr){
        pool->cur_usable_small_block = sbp;
    }
    return res;
}

//分配大块内存
char* memory_pool::mallocBigBlock(memory_pool * pool,size_t size)
{
    void * temp = malloc(size);
    memset(temp,0,size);

    big_block * bbp = pool->big_block_start;
    int i = 0;
    while(bbp){
        if(bbp->big_buffer == nullptr){
            bbp->big_buffer =(char*)temp;
            return bbp->big_buffer;
        }
        if (i>3)
        {
            //三次找不到能用的bigblock
            break;
        }
        bbp = bbp->next_block;
        ++i;
        
    }
    //-创建新的big_block，这里比较难懂的点，就是Nginx觉得big_block的buffer虽然是一个随机地址的大内存
    //-但是big_block本身算一个小内存，那就不应该还是用随机地址，应该保存在内存池内部的空间。
    //-所以这里有个套娃的内存池malloc操作
    big_block* new_bbp = (big_block*)memory_pool::poolMalloc(pool,sizeof(big_block));
    //-初始化
    new_bbp -> big_buffer = (char*)temp;
    new_bbp ->next_block = pool->big_block_start;
    pool -> big_block_start = new_bbp;

    //-返回分配内存的首地址
    return new_bbp->big_buffer;
    
}

void memory_pool::freeBigBlock(memory_pool*pool,char *buffer_ptr)
{
    big_block* bbp = pool->big_block_start;
    while(bbp){
        if(bbp->big_buffer == buffer_ptr){
            free(bbp->big_buffer);
            bbp->big_buffer =nullptr;
            return;
        }
        bbp = bbp->next_block;
    }
}

void memory_pool::destroyPool(memory_pool * pool)
{
    //销毁大内存
    big_block* bbp = pool->big_block_start;
    while(bbp){
        if(bbp->big_buffer){
            free(bbp->big_buffer);
            delete bbp->big_buffer;
        }
        bbp = bbp->next_block;
    }

    //释放小内存
    small_block * temp = pool->small_block_start->next_block;
    while(temp){
        small_block *next = temp->next_block;
        free(temp);
        temp = next;
    }
    delete temp;
    free(pool);
}




