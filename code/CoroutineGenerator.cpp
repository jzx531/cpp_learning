#include <iostream>
#include <coroutine>

struct Generator {
    struct promise_type {
        int current_value;
        std::suspend_always yield_value(int v) {
            current_value = v;
            return {};
        }
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> handle;
    Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }
    bool next() {
        handle.resume();
        return!handle.done();
    }
    int value() { return handle.promise().current_value; }
};

Generator counter() {
    for (int i = 0; i < 5; ++i) {
        co_yield i;
    }
}

int main() {
    Generator gen = counter();
    while (gen.next()) {
        std::cout << "Generated value: " << gen.value() << std::endl;
    }
    return 0;
}