#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include <vector>
#include <atomic>

#define DATA_SIZE 10
std::shared_mutex smtx;
std::mutex mtx;
std::atomic<bool> ready(false);
std::vector<int> data;
void init_data() {
    for (int i = 0; i < DATA_SIZE; i++) {
        data.push_back(i);
    }
    ready .store(true, std::memory_order_release);
}



void thread_func(int id) {
    // std::lock_guard<std::mutex> lock(mtx);
    while(! ready.load(std::memory_order_acquire))
    while(true) {
        std::shared_lock<std::shared_mutex> lock(smtx);
        // for (int i = 0; i < DATA_SIZE; i++) {
            int i = rand() % DATA_SIZE;
            std::cout << id << "THREAD "<< data[i] << std::endl;
            std::this_thread::yield();
        // }
    }
}

int main() {
    std::thread t(init_data);
    std::thread t1(thread_func,1);
    std::thread t2(thread_func,2);
    
    t.join();
    t1.join();
    t2.join();
    
   
    return 0;
}