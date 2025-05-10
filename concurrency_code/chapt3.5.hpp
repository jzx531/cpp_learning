#pragma once
#include <iostream>
#include "mingw.mutex.h"
#include <memory>
#include <stack>

struct empty_stack:std::exception{
    const char* what() const throw();
};

template<typename T>
class threadsafe_stack{
    private:
        std::stack<T> data;
        mutable std::mutex m;
    public:
        threadsafe_stack(){}
        threadsafe_stack(const threadsafe_stack& other)
        {
            std::lock_guard<std::mutex> lock(other.m);
            data=other.data;
        }
        threadsafe_stack& operator=(const threadsafe_stack& other)=delete;
        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock(m);
            data.push(std::move(new_value));
        }
        std::shared_ptr<T> pop()
        {
            std::lock_guard<std::mutex> lock(m);
            if(data.empty())
                throw empty_stack();
            auto res=std::make_shared<T>(std::move(data.top()));
            data.pop();
        }
        bool empty() const{
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
};