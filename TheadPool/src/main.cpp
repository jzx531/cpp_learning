#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <random>

// 包含你的 ThreadPool 和 BlockingQueue 实现
#include "ThreadPool.hpp"  // 假设你把上面的 ThreadPool 放在 ThreadPool.hpp 中

int main() {
    const int numTasks = 20;
    const int poolSize = 4;

    std::cout << "Creating thread pool with " << poolSize << " threads...\n";

    {
        ThreadPool pool(poolSize);

        std::vector<std::future<int>> futures;

        // 提交任务
        for (int i = 0; i < numTasks; ++i) {
            int task_id = i;
            auto promise = std::make_shared<std::promise<int>>();

            futures.push_back(promise->get_future());

            pool.execute([task_id, promise]() {
                // 模拟工作：随机休眠 10～100ms
                std::this_thread::sleep_for(std::chrono::milliseconds(10 + task_id % 90));
                std::cout << "Task " << task_id << " executed by thread "
                          << std::this_thread::get_id() << '\n';
                promise->set_value(task_id * 2);
            });
        }

        // 等待所有任务完成并验证结果
        bool all_ok = true;
        for (int i = 0; i < numTasks; ++i) {
            int result = futures[i].get();  // 阻塞直到完成
            if (result != i * 2) {
                all_ok = false;
            }
        }

        std::cout << "All tasks completed. Result correct: " << (all_ok ? "YES" : "NO") << "\n";
    } // ThreadPool 析构，自动 join 所有线程

    std::cout << "Thread pool destroyed safely.\n";
    return 0;
}