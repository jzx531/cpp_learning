#ifndef SIMPLE_INTERRUPTIBLE_THREAD_HPP
#define SIMPLE_INTERRUPTIBLE_THREAD_HPP

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

class interrupt_flag
{
    public:
        void set();
        bool is_set() const;
};
thread_local interrupt_flag this_thread_interrupt_flag;
class interruptible_thread
{
    std::thread internal_thread;
    interrupt_flag* flag;
    public:
        template<typename FunctionType>
        interruptible_thread(FunctionType f)
        {
            std::promise<interrupt_flag*> p;
            internal_thread = std::thread([f,&p]{
                p.set_value(&this_thread_interrupt_flag);
                f();
            });
            flag = p.get_future().get();
        }
        void interrupt()
        {
            if(flag)
            {
                flag->set();
            }
        };
        // void interruption_point()
        // {
        //     if(this_thread_interrupt_flag.is_set())
        //     {
        //         throw thread_interrputed();
        //     }
        // }
        // void foo()
        // {
        //     while(!done)
        //     {
        //         interruption_point();
        //         process_next_item();
        //     }
        // }
};

#endif // SIMPLE_INTERRUPTIBLE_THREAD_HPP