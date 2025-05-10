#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"

class hierachical_mutex
{
    std::mutex internal_mutex;
    unsigned long const hierachy_value;
    unsigned long previous_hierachy_value;
    static thread_local unsigned long this_thread_hierachy_value;
    void check_for_hierarchy_violation() 
    {
        if (this_thread_hierachy_value <=hierachy_value)
        {
            throw std::logic_error("Hierachy violation");
        }
    }
    void update_hierachy_value()
    {
        previous_hierachy_value = this_thread_hierachy_value;
        this_thread_hierachy_value = hierachy_value;
    }
    public:
    explicit hierachical_mutex(unsigned long value):hierachy_value(value),previous_hierachy_value(0){}
    void lock(){
       check_for_hierarchy_violation();
       internal_mutex.lock();
       update_hierachy_value();
    }
    void unlock(){
        if(this_thread_hierachy_value != hierachy_value)
        {
            throw std::logic_error("Hierachy violation");
        }
        this_thread_hierachy_value = previous_hierachy_value;
        internal_mutex.unlock();
    }
    bool try_lock(){
        check_for_hierarchy_violation();
        if(!internal_mutex.try_lock())
        {
            return false;
        }
        update_hierachy_value();
        return true;
    }
};

thread_local unsigned long hierachical_mutex::this_thread_hierachy_value(ULLONG_MAX);

