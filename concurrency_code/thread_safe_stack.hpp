#ifndef THREAD_SAFE_STACK_HPP
#define THREAD_SAFE_STACK_HPP

#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include <stack>
#include <atomic>
#include <exception>

struct empty_stack:std::exception{
    const char* what() const throw();
};

template<typename T>
class threadsafe_stack
{
    private:
        std::stack<T> stack;
        mutable std::mutex m;
    public:
        threadsafe_stack(){}
        threadsafe_stack(const threadsafe_stack& other)
        {
            std::lock_guard<std::mutex> lock(other.m);
            data = other.data;
        }
        threadsafe_stack& operator=(const threadsafe_stack& )=delete;
        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock(m);
            stack.push(new_value);
        }

        std::shared_ptr<T> pop()
        {
            std::lock_guard<std::mutex> lock(m);
            if(data.empty()) throw empty_stack();
            std::shared_ptr<T> const res(std::make_shared<T>(std::move(data.top())));
            data.pop();
            return res;
        }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> lock(m);
            if(data.empty())throw empty_stack();
            value = std::move(data.top());
            data.pop();
        }

        bool empty()const{
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
};

#endif // THREAD_SAFE_STACK_HPP