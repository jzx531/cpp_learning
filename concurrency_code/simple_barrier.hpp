#ifndef SIMPLE_BARRIER_HPP
#define SIMPLE_BARRIER_HPP
#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include "mingw.condition_variable.h"
#include "mingw.mutex.h"
#include "mingw.future.h"
#include <list>
#include "thread_safe_stack.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include "join_threads.hpp"

class barrier
{
    unsigned const count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
    public:
    explicit barrier(unsigned count):count(count),spaces(count),generation(0){}
    void wait()
    {
        unsigned const my_generation=generation;
        if(!--spaces)//当空座递减为0时，说明所有线程都已经到达线程卡,generation才更新
        {
            spaces=count;
            ++generation;
        }
        else{
            while(generation==my_generation)
                std::this_thread::yield();
        }
    }
};

class atomic_barrier
{
    std::atomic<unsigned> count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
    public:
    explicit atomic_barrier(unsigned count):count(count),spaces(count),generation(0){}
    void wait()
    {
        unsigned const my_generation=generation;
        if(!--spaces)//当空座递减为0时，说明所有线程都已经到达线程卡,generation才更新
        {
            spaces=count.load();
            ++generation;
        }
        else{
            while(generation==my_generation)
                std::this_thread::yield();
        }
    }
    void done_waiting()//减少线程,剔除线程卡
    {
        --count;        
        if(!--spaces)
        {
            spaces=count.load();
            ++generation;
        }
    }
};
#endif // SIMPLE_BARRIER_HPP