#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <condition_variable>
#include <functional>
#include <memory>
#include "BlockingQueue.hpp"  // 假设它支持 take() 返回 T

class ThreadPool {
private:
    BlockingQueue<std::function<void()>> tasks;
    std::vector<std::thread> workers;
    // std::unordered_set<std::thread> workers;
    std::atomic<bool> stop{false};
    int corePoolSize;

public:
    explicit ThreadPool(int size) : tasks(size*2),corePoolSize(size), stop(false) {
        for (int i = 0; i < corePoolSize; ++i) {
            workers.emplace_back(std::thread([this]() {
            // workers.insert(std::thread([this]() {
                while (!stop) {
                    auto task = tasks.take();  // 假设 take() 阻塞直到有任务
                    if (!stop && task) {
                        try {
                            task();
                        } catch (...) {
                            // Log or ignore
                        }
                    }
                }
            }));
        }
    }

    void execute(std::function<void()> task) {
        if (!stop) {
            tasks.put(std::move(task));  // 假设 BlockingQueue 有 put()
        }
    }

    ~ThreadPool() {
        stop = true;
        // tasks.wakeAll();  // 假设 BlockingQueue 支持唤醒所有等待线程
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    // 禁止拷贝
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};