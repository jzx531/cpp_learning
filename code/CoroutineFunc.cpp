#include <iostream>
#include <coroutine>
#include <chrono>
#include <thread>

struct AsyncResult {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) const noexcept {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    int await_resume() const noexcept { return 42; }
    
};
struct Task {
    struct promise_type {
        Task get_return_object() { 
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; 
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}

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

};

Task asyncFunction() {
    std::cout << "Before co_await" << std::endl;
    int result = co_await AsyncResult();
    std::cout << "After co_await, result: " << result << std::endl;
}

int main() {
    auto task = asyncFunction();  // 协程初始挂起
    task.resume();                // 第一次 resume：执行到 co_await，挂起
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 确保 await_suspend 完成
    task.resume();                // 第二次 resume：从 co_await 恢复，执行后续代码
    return 0;
}