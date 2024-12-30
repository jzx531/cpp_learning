#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include<algorithm>
#include<list>

class some_data{
    int a;
    std::string b;
    public:
    void do_something();
};

class data_wrapper{
    private:
    some_data data;
    std::mutex mtx;
    public:
    template<typename Function>
    void process_data(Function func)
    {
        std::lock_guard<std::mutex> lock(mtx);
        func(data);
    }
};
some_data* unprotected = nullptr;
void  malicious_function(some_data& protected_data)
{
    unprotected = &protected_data;
}
data_wrapper wrapper;

void foo()
{
    wrapper.process_data(malicious_function);
    unprotected->do_something();
}
