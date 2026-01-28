#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stdlib.h>
#include <stddef.h>
#include "memory_block.h"


class memory_pool{
    public:
        // members
        size_t small_buffer_capacity;
        small_block * cur_usable_small_block;
        big_block * big_block_start;
        small_block small_block_start[0];

        //api
        static memory_pool * createPool(size_t capacity);
        static void destroyPool(memory_pool * pool);
        static char * createNewSmallBlock(memory_pool * pool,size_t size);
        static char* mallocBigBlock(memory_pool * pool,size_t size);
        static void * poolMalloc(memory_pool *pool,size_t size);
        static void freeBigBlock(memory_pool * pool, char *buffer_ptr);

};


#endif // MEMORY_POOL_H

