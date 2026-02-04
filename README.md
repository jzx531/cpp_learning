# Coroutine

[TOC]

协程允许在执行过程中主动让出执行权，将执行流程暂时转移到其他任务上，并且在适当的时候能够从暂停的位置继续执行 。

协程则打破了这种线性执行的模式，它可以在执行过程中通过特定的语法（如co_await、co_yield等关键字）暂停执行，并在之后的某个时刻恢复执行。

```c++
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
```

协程是一种用户态的轻量级线程，它的调度完全由用户程序控制，不需要操作系统的介入 。协程之间的上下文切换只需要保存和恢复少量的寄存器信息，开销极低，通常在纳秒级别，远远小于线程上下文切换的开销 


co_yield关键字主要用于生成器模式，它允许协程向调用者返回一个值，并暂停执行 。每次执行到co_yield时，协程会将当前值返回给调用者，同时保存自身的执行状态，以便下次恢复执行时能够继续从暂停的位置开始

```c++
Generator counter() {
    for (int i = 0; i < 5; ++i) {
        co_yield i;
    }
}
```

co_return关键字用于终止协程的执行，并返回一个值（如果协程有返回类型的话）给调用者 。当协程执行到co_return时，会调用promise_type的return_value（如果有返回值）或return_void（如果返回类型是void）函数，然后执行final_suspend进行最后的清理工作，最后将控制权返回给调用者

```c++
Result calculate() {
    int sum = 1 + 2;
    co_return sum;
}
```

## 关键组件

### 1. Promise类型:Promise类型在协程中起着定义协程行为的关键作用，每个协程都关联这个一个Promise，该对象包含一系列成员函数，用于控制协程的生命周期和行为逻辑

* get_return_object函数：用于返回一个代表协程的对象，这个对象通常包含协程的句柄等信息，通过它可以对协程进行控制，如恢复、暂停、销毁等操作 。​
* initial_suspend函数：决定协程在初始执行时是否立即暂停。如果返回一个表示暂停的对象（如std::suspend_always），则协程在开始执行后会立即暂停；如果返回std::suspend_never，则协程会立即开始执行 。​
* final_suspend函数：在协程结束时被调用，用于定义协程结束时的行为，比如是否需要进行最后的暂停操作。通常返回std::suspend_always表示在协程结束时进行暂停，以便进行一些清理工作；返回std::suspend_never则表示协程结束后立即返回 。​
* return_value函数（如果协程有返回值）：当协程执行到co_return语句并带有返回值时，会调用这个函数将返回值存储到 Promise 对象中 。​
* yield_value函数（如果协程使用co_yield）：当协程执行到co_yield语句时，会调用这个函数将co_yield后面的值存储到 Promise 对象中，并返回一个表示暂停的对象，暂停协程执行。​

### 2.协程句柄:协程句柄(std::coroutine_handle)用于管理协程的生命周期，它是一个指向协程内部状态的指针。通过这个句柄就可以对协程进行操作
* resume方法：用于恢复协程的执行。当协程处于暂停状态时，调用resume方法可以让协程从上次暂停的位置继续执行 。​
* suspend方法：用于暂停协程的执行（在一些情况下，协程也可以自动暂停，如遇到co_await关键字） 。​
* destroy方法：用于销毁协程，释放协程占用的资源 。当协程不再需要时，应该调用destroy方法来确保资源的正确释放，避免内存泄漏等问题 。​
* done方法：用于检查协程是否已经执行完毕或被销毁。如果协程已经结束或被销毁，done方法返回true；否则返回false 。​

### 3.可等待对象：可等待对象（Awaitable）是实现await_ready、await_suspend、await_resume这三个方法的对象，它与co_await关键字紧密配合

* await_ready方法：用于判断可等待对象是否已经准备好。如果返回true，表示可等待对象已经准备就绪，协程可以继续执行，不会被挂起；如果返回false，则协程会被挂起，等待可等待对象准备好 。​
* await_suspend方法：当await_ready返回false，协程被挂起时，会调用这个方法 。它负责将协程的执行权交还给调用者，并可以进行一些额外的操作，比如注册回调函数，以便在可等待对象准备好时恢复协程执行 。​
* await_resume方法：当可等待对象准备好，协程恢复执行时，会调用这个方法 。它返回可等待对象的结果，这个结果会作为co_await表达式的值，供协程后续使用 。例如，在前面的asyncFunction协程中，AsyncResult就是一个可等待对象，通过实现这三个方法，实现了协程的暂停和恢复等待异步操作完成的功能。​

## 异步I/O
传统的同步 I/O 方式会导致线程在等待 I/O 完成时被阻塞，无法执行其他任务，这在高并发场景下会严重影响程序的性能和响应速度 。而协程在异步 I/O 方面具有独特的优势，它可以将 I/O 操作的等待过程封装为同步风格的代码，使代码逻辑更加清晰，同时避免了线程上下文切换的开销 。​

以网络请求为例，假设我们要使用 C++ 协程实现一个简单的 HTTP 客户端，向服务器发送请求并获取响应 。

## 任务调度

与传统的线程调度方式相比，协程调度具有以下优势：​

* 开销极低：线程上下文切换需要保存和恢复大量的 CPU 寄存器、栈指针等信息，涉及内核态和用户态的切换，开销较大 。而协程上下文切换只需要保存和恢复少量的寄存器信息，并且完全在用户态进行，开销通常在纳秒级别，远远小于线程上下文切换的开销 。​
* 高并发处理能力：线程数量过多时，会消耗大量的系统资源，如内存、CPU 时间等，导致上下文切换频繁，系统性能下降 。而协程非常轻量级，可以在单个线程中创建大量的实例，实现高并发任务处理，不会像线程那样受到系统资源的限制 。​
* 编程模型简洁：使用协程进行任务调度，可以使代码逻辑更加清晰，以同步的方式编写异步任务，避免了复杂的回调函数嵌套和状态管理，提高了代码的可读性和可维护性 。​


Scheduler类实现了一个简单的任务调度器 。addTask方法用于将任务添加到任务队列中，run方法负责调度任务的执行 。​
在run方法中，首先依次启动所有任务，使它们开始执行到第一个co_await处暂停 。然后再次遍历任务队列，恢复那些尚未完成的任务继续执行 。

```c++
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
```




