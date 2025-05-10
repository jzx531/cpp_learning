#ifndef THREADPOOL_WITH_LOCALQUEUE_HPP
#define THREADPOOL_WITH_LOCALQUEUE_HPP
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
#include <queue>
#include "join_threads.hpp"

#include "threadsafe_queue.hpp" 

class function_wrapper{
    struct impl_base{
        virtual void call()=0;
        virtual ~impl_base(){}
    };
    std::unique_ptr<impl_base> impl;
    template<typename F>
    struct impl_type:impl_base
    {
        F f;
        impl_type(F f_):f(std::move(f_)){}
        void call(){F();}
    };
    public:
    template<typename F>
    function_wrapper(F &&f):impl(new impl_type<F>(std::move<F>(f))){}
    void operator()()
    {
        impl->call();
    }
    function_wrapper()=default;
    function_wrapper(function_wrapper && other)
    {
        impl = std::move(other.impl);
    }
    function_wrapper & operator=(function_wrapper && other)
    {
        impl = std::move(other.impl);
        return *this;
    }
    function_wrapper(const function_wrapper & other) = delete;
    function_wrapper(function_wrapper && other) = delete;
    function_wrapper& operator=(const function_wrapper & other) = delete;
};
class thread_pool
{
    threadsafe_queue<function_wrapper> pool_work_queue;
    typedef std::queue<function_wrapper> local_queue_type;
    static thread_local std::unique_ptr<local_queue_type> local_work_queue;
    
    void worker_thread()
    {
        local_work_queue.reset(new local_queue_type);
        while(true)
        {
            run_pending_task();
        }
    }
    public:
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(f);
        std::future<result_type> res(task.get_future());
        if(local_work_queue)
        {
            local_work_queue->emplace(std::move(task));
        }
        else
        {
            pool_work_queue.push(std::move(task));
        }
        return res;
    }
    void run_pending_task()
    {
        function_wrapper task;
        if(local_work_queue && !local_work_queue->empty())
        {
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        }
        else if(pool_work_queue.try_pop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
    
};


#endif // THREADPOOL_WITH_LOCALQUEUE_HPP