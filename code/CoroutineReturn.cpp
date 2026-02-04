#include <iostream>
#include <coroutine>

struct Result {
    struct promise_type {
        int value;
        Result get_return_object() { return Result{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int v) { value = v; }
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> handle;
    Result(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Result() { if (handle) handle.destroy(); }
    int get_result() {
        if (!handle.done()) {
            handle.resume(); // 执行协程体直到 co_return 或结束
        }
        return handle.promise().value;
    }
};

Result calculate() {
    int sum = 1 + 2;
    co_return sum;
}

int main() {
    Result res = calculate();
    std::cout << "Calculation result: " << res.get_result() << std::endl;
    return 0;
}

