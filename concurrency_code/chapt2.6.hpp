#pragma once
#include <iostream>
#include "mingw.thread.h"

class scoped_thread{
    std::thread t;
    public:
    explicit scoped_thread(std::thread&& t_) : t(std::move(t_)){
        if(!t.joinable()){
            throw std::logic_error("No thread");
        }
    }
    ~scoped_thread(){
        if(t.joinable()){
            t.join();
        }
    }
    scoped_thread(const scoped_thread&) = delete;
    scoped_thread& operator=(const scoped_thread&) = delete;
};

class joining_thread{
    std::thread t;
    public:
    joining_thread() noexcept= default;
    template<typename Callable,typename...Args>
    explicit joining_thread(Callable&& func,Args&&...args):t(std::forward<Callable>(func),std::forward<Args>(args)...){};
    explicit joining_thread(std::thread&& t_):t(std::move(t_)){};
    explicit joining_thread(joining_thread&& other)noexcept:t(std::move(other.t)){};
    joining_thread& operator=(joining_thread&& other)noexcept{
        if(joinable())
            join();
        t=std::move(other.t);
        return *this;
    }
    ~joining_thread(){
        if(joinable())
            join();
    }
    void swap(joining_thread& other)noexcept{
        t.swap(other.t);
    }
    std::thread::id get_id()const noexcept{
        return t.get_id();
    }
    bool joinable()const noexcept{
        return t.joinable();
    }
    void join(){
        if(!t.joinable()){
            throw std::logic_error("No thread");
        }
        t.join();
    }
    void detach(){
        t.detach();
    }
    std::thread& as_thread() noexcept
    {
        return t;
    }
    const std::thread& as_thread()const noexcept{
        return t;
    }
};