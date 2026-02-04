#include <iostream>
#include <coroutine>

struct Task {
    struct promise_type {
        Task get_return_object() { 
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; 
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
    };

    // 成员变量：协程句柄
    std::coroutine_handle<promise_type> handle;

    // 构造函数
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}

    // 提供 resume 接口
    void resume() {
        if (handle && !handle.done()) {
            handle.resume();
        }
    }

    // 可选：析构时销毁协程（避免内存泄漏）
    ~Task() {
        if (handle) {
            handle.destroy();
        }
    }

    // 禁用拷贝（协程句柄不可拷贝）
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // 允许移动（可选，根据需求）
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

Task asyncTask() {
    std::cout << "Start of asyncTask" << std::endl;
    co_await std::suspend_always{};
    std::cout << "Resumed in asyncTask" << std::endl;
}

int main() {
    auto task = asyncTask();
    std::cout << "Back in main" << std::endl;
    task.resume();  // ✅ 现在可以调用了
    return 0;
}