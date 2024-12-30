#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include "mingw.condition_variable.h"
#include <queue>
class data_chunk {
public:
    int data;
    data_chunk(int d) : data(d) {}
    
};
data_chunk prepare_data()
{
    return data_chunk(rand()%100);
}
std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;
void data_preparation_thread() {
    while(true)
    {
       data_chunk const data = prepare_data();
       {
        std::lock_guard<std::mutex> lock(mut);
        data_queue.push(data);
       }
       data_cond.notify_one();
    }
}

void data_processing_thread() {
    while(true)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[]{return!data_queue.empty();});
        data_chunk data = data_queue.front();
        data_queue.pop();
        lk.unlock();
        std::cout << "Processing data: " << data.data << std::endl;
    }
}
int main() {
    std::thread data_prep_thread(data_preparation_thread);
    std::thread data_proc_thread(data_processing_thread);
    data_prep_thread.detach();
    data_proc_thread.detach();
    return 0;
}