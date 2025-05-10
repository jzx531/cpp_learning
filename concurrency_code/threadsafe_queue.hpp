#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include "mingw.condition_variable.h"
#include "mingw.mutex.h"
#include <atomic>
#include <queue>

template<typename T>
class threadsafe_queue {
    public:
        mutable std::mutex mut;
        std::queue<T> data_queue;
        std::condition_variable data_cond;
    public:
        threadsafe_queue(){};
        void push(T new_value)
        {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(std::move(new value));
            data_cond.notify_one();
        }
        void wait_and_pop(T& value)
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk,[this]{return !data_queue.empty()});
            value = std::move(data_queue.front());
            data_queue.pop();
        }
        std::shared_ptr<T> wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk,[this]{return !data_queue.empty();});
            std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
            data_queue.pop();
            return res;
        }
        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lk(mut);
            if(data_queue.empty()) return false;
            value = std::move(data_queue.front());
            data_queue.pop();
            return true;
        }
        std::shared_ptr<T> try_pop()
        {
            std::lock_guard<std::mutex> lk(mut);
            if(data_queue.empty()) return std::shared_ptr<T>();
            std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
            data_queue.pop();
            return res;
        }
        bool empty() const
        {
            std::lock_guard<std::mutex> lk(mut);
            return data_queue.empty();
        }
};


#endif // THREADSAFE_QUEUE_HPP