#include <iostream>
#include <coroutine>
#include <httplib.h>

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

Task httpRequest() {
    httplib::Client cli("example.com", 80);
    auto res = cli.Get("/");
    if (res) {
        std::cout << "Response Status: " << res->status << std::endl;
        std::cout << "Response Body: " << res->body << std::endl;
    } else {
        std::cout << "Request failed" << std::endl;
    }
    co_return;
}

int main() {
    auto task = httpRequest();
    task.resume();
    return 0;
}