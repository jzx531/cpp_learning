#ifndef BARRIER_PARTIAL_SUM_HPP
#define BARRIER_PARTIAL_SUM_HPP

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

struct barrier
{
    std::atomic<unsigned> count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
    public:
    explicit barrier(unsigned count):count(count),spaces(count),generation(0){}
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

template<typename Iterator>
void parallel_partial_sum(Iterator first,Iterator last)
{
    typedef typename Iterator::value_type value_type;
    struct process_element
    {
        void operator()(Iterator first,Iterator last,std::vector<value_type>&buffer,unsigned i,barrier&b)
        {
            value_type& ith_element=*(first+i);
            bool update_source = false;
            for(unsigned step=0,stride=1;stride<=i;++step,stride*=2)
            {
                value_type const& source = (step%2)? buffer[i]:ith_element;
                value_type & dest = (step%2)? ith_element:buffer[i];
                value_type const& addend = (step%2)? buffer[i-stride]:*(first+i-stride);
                dest=source+addend;
                update_source = !(step%2);
                b.wait();
            }
            if(update_source)
            {
                ith_element = buffer[i];
            }
            b.done_waiting();
        }
    };
    unsigned long const length=std::distance(first,last);
    if(length<=1) return;
    std::vector<value_type> buffer(length);
    barrier b(length);
    std::vector<std::thread> threads(length-1);
    join_threads joiner(threads);
    Iterator block_start=first;
    for(unsigned long i=0;i<length-1;++i)
    {
        threads[i]=std::thread(process_element(),first,last,std::ref(buffer),i,std::ref(b));
    }
    process_element()(first,last,std::ref(buffer),length-1,std::ref(b));
}

#endif // BARRIER_PARTIAL_SUM_HPP