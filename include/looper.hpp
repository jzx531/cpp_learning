#ifndef LOOPER_HPP
#define LOOPER_HPP

#include "buffer.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>

namespace LOG
{
    using Functor = std::function<void(Buffer &)>;
    enum class AsyncType
    {
        //考虑安全不安全两种模式
        ASYNC_SAFE,//考虑资源耗尽，当缓冲区满了就阻塞
        ASYNC_UNSAFE //不考虑资源耗尽的问题
    };

    class AsyncLooper
    {
    private:
        AsyncType _looper_type; // 选择异步工作器工作模式（安全与非安全模式）
        std::atomic<bool> _stop; // 工作器退出标志
        Buffer _pro_buf; 
        Buffer _con_buf; 
        std::mutex _mutex;
        std::condition_variable _pro_cond;//生产锁
        std::condition_variable _con_cond;//消费锁
        std::thread _thread; // 异步工作器对应的工作线程
        
    private:
        Functor _call_back;

    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor& cb, AsyncType looper_type = AsyncType::ASYNC_SAFE)
        :_call_back(cb),_looper_type(looper_type),_stop(false),_thread(std::thread(&AsyncLooper::threadEntry, this)){};

        ~AsyncLooper() { stop(); }

        void stop()
        {
            _stop = true;
            _con_cond.notify_all();
            _thread.join();
        }

        void push(const char *data,size_t len)
        {
            std::unique_lock lock(_mutex);
            //如果是安全模式就需要阻塞生产者cond直到buffer足够push
            if(_looper_type ==  AsyncType:: ASYNC_SAFE)
            {
                _pro_cond.wait(lock,[&](){return _pro_buf.writeAbleSize()>=len;});
            }
            _pro_buf.push(data,len);
            _con_cond.notify_one();
        }

    private:
        //线程入口函数
        void threadEntry()
        {
            while(1)
            {
                std::unique_lock lock(_mutex);
                if(_stop && _pro_buf.empty()) break;
                _con_cond.wait(lock,[&](){return !_pro_buf.empty()||_stop;});
                _con_buf.swap(_pro_buf);
                if(_looper_type ==  AsyncType:: ASYNC_SAFE) _pro_cond.notify_all();

                lock.unlock();//结束锁

                // 3.被唤醒后，对消费者缓冲区进行数据处理
                _call_back(_con_buf);
                // 4.初始化消费者缓冲区
                _con_buf.reset();
            }
        }


    
    };
}
















#endif

