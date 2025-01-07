#ifndef SIMPLIST_THREAD_POOL_HPP
#define SIMPLIST_THREAD_POOL_HPP
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
#include <atomic>
#include "join_threads.hpp"

#include "threadsafe_queue.hpp" 

class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<std::function<void() >> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;//帮助threads自动析构
    void worker_thread()
    {
        while(!done)
        {
            std::function<void()>task;
            if(work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
    public:
    thread_pool():done(false),joiner(threads)
    {
        unsigned const thread_count= std::thread::hardware_concurrency();
        try{
            for(unsigned i=0;i<thread_count;++i)
            {
                threads.push_back(std::thread(&thread_pool::worker_thread,this));
            }
        }
        catch(...)
        {
            done=true;
            throw;
        
        }
    }
    ~thread_pool()
    {
        done=true;
    }
    template<typename FunctionType>
    void submit(FunctionType f)
    {
        work_queue.push(std::function<void() > (f));
    }

};

#endif // SIMPLIST_THREAD_POOL_HPP