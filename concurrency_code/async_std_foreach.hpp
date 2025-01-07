#ifndef ASYNC_STD_FOREACH_HPP
#define ASYNC_STD_FOREACH_HPP

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


template<typename Iterator,typename Func>
void parallel_for_each(Iterator first,Iterator last,Func f)
{
    unsigned long const length=std::distance(first,last);
    if(length==0) return;
    unsigned long const min_per_thread=25;
    if(length<2*min_per_thread)
    {
        std::for_each(first,last,f);
    }
    else
    {
        Iterator const mid_point=first+length/2;
        std::future<void> first_half_result=std::async(parallel_for_each<Iterator,Func>,first,mid_point,f);
        parallel_for_each(mid_point,last,f);
        first_half_result.get();
    }
    unsigned long const length=std::distance(first,last);
    if(length==0) return;
    unsigned long const min_per_thread=25;
    unsigned long const max_threads=(length+min_per_thread-1)/min_per_thread;
    unsigned long const hardware_threads=std::thread::hardware_concurrency();
    unsigned long const num_threads=std::min(hardware_threads!=0?hardware_threads:2,max_threads);
    unsigned long const block_size=length/num_threads;
    std::promise<Iterator> result;
    std::atomic<bool> done{false};
    std::vector<std::thread>threads(num_threads-1);
    {
        join_threads joiner(threads);
        Iterator block_start = first;
        for (size_t i = 0; i < count; i++)
        {
           Iterator block_end = block_start;
           std::advance(block_end,block_size);
           threads[i]=std::thread(find_element(),block_start,block_end,match,&result,&done);
        }
        find_element(first,last,match,&result,&done);
    }
    if(!done_result.load())
    {
        return last;
    }
    return result.get_future().get();
};

#endif