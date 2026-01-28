#include "memory_block.h"
#include "memory_pool.h"
#include <stdlib.h>
#include <stdio.h>

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
char* memory_pool::mallocBigBlock(memory_pool * poll,size_t size)
{
    void * temp = malloc(size);
    memeset(temp,0,size);
    
}




