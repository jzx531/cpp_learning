#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

class small_block{
    public:
        char * cur_usable_buffer;
        char * buffer_end;
        small_block * next_block;
        int no_enough_times;
};

class big_block{
    public:
        char * buffer;
        big_block * next_block;
};


#endif // MEMORY_BLOCK_H


