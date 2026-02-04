#include <iostream>
#include <coroutine>
#include <vector>

struct Task {
    // 1. 定义 promise_type
    struct promise_type {
        // 2. get_return_object 必须返回一个持有当前协程句柄的 Task
        Task get_return_object() noexcept {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
    };

    // 3. Task 必须持有协程句柄
    std::coroutine_handle<promise_type> handle;

    // 4. 构造函数：接收句柄
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}

    // 5. 提供 resume 接口
    void resume() {
        if (handle && !handle.done()) {
            handle.resume();
        }
    }

    // 6. （可选）析构时销毁协程
    ~Task() {
        if (handle) {
            handle.destroy();
        }
    }

    // 禁用拷贝（协程句柄不可拷贝）
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // 允许移动（如果需要放入 vector）
    Task(Task&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
};

// 协程函数
Task task1() {
    std::cout << "Task 1 is running" << std::endl;
    co_await std::suspend_always{};
    std::cout << "Task 1 resumed" << std::endl;
}

Task task2() {
    std::cout << "Task 2 is running" << std::endl;
    co_await std::suspend_always{};
    std::cout << "Task 2 resumed" << std::endl;
}

class Scheduler {
public:
    void addTask(Task task) {
        tasks.push_back(std::move(task)); // 需要移动语义
    }

    void run() {
        // 第一次 resume：启动协程，执行到第一个 co_await
        for (auto& task : tasks) {
            task.resume();
        }
        // 第二次 resume：从 co_await 恢复，执行剩余部分
        for (auto& task : tasks) {
            if (!task.handle.done()) {
                task.resume();
            }
        }
    }

private:
    std::vector<Task> tasks; // 需要 Task 支持移动
};

int main() {
    Scheduler scheduler;
    scheduler.addTask(task1());
    scheduler.addTask(task2());
    scheduler.run();
    return 0;
}