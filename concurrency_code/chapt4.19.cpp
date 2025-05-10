#include <iostream>
#include <future>
#include <thread>
#include <utility>
#include <functional>

// 定义一个支持 then 的 future 类模板
template<typename T>
class MyFuture {
public:
    MyFuture(std::future<T>&& future) : future_(std::move(future)) {}

    template<typename Func>
    auto then(Func&& func) {
        using ResultType = decltype(func(future_.get()));
        std::promise<ResultType> promise;
        auto resultFuture = promise.get_future();

        std::thread t([p = std::move(promise), f = std::move(func), future = std::move(future_)]() mutable {
            try {
                p.set_value_at_thread_exit(f(future.get()));
            }
            catch (...) {
                p.set_exception_at_thread_exit(std::current_exception());
            }
            });

        t.detach();
        return MyFuture<ResultType>(std::move(resultFuture));
    }

    std::future<T>&& get_future() {
        return std::move(future_);
    }

private:
    std::future<T> future_;
};

// 辅助函数，用于创建 MyFuture 对象
template<typename Func>
auto make_my_future(Func&& func) {
    using ResultType = decltype(func());
    std::promise<ResultType> promise;
    auto future = promise.get_future();

    std::thread t([p = std::move(promise), f = std::forward<Func>(func)]() mutable {
        try {
            p.set_value_at_thread_exit(f());
        }
        catch (...) {
            p.set_exception_at_thread_exit(std::current_exception());
        }
        });

    t.detach();
    return MyFuture<ResultType>(std::move(future));
}

int main() {
    auto future = make_my_future([]() {
        std::cout << "First task running..." << std::endl;
        return 42;
        });

    auto nextFuture = future.then([](int result) {
        std::cout << "Second task running with result: " << result << std::endl;
        return result * 2;
        });

    std::cout << "Main thread continues..." << std::endl;

    // 获取最终结果
    try {
        int finalResult = nextFuture.then([](int result) {
            std::cout << "Third task running with result: " << result << std::endl;
            return result + 10;
            }).get_future().get();
        std::cout << "Final result: " << finalResult << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
