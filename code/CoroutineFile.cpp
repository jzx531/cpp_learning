#include <iostream>
#include <fstream>
#include <coroutine>
#include <string>

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

Task readFileAsync(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    co_return;
}

int main() {
    auto task = readFileAsync("./Makefile");
    task.resume();
    return 0;
}