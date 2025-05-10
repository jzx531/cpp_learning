#include<atomic>
#include<iostream>
#include<vector>
#include "mingw.thread.h"
std::atomic<int> count (0);
std::vector<int> queue_data;

void populate_queue()
{
    unsigned const number_of_items = 20;
    queue_data.clear();
    for(unsigned i=0;i<number_of_items;++i)
    {
        queue_data.push_back(i);
    }
    count.store(number_of_items,std::memory_order_release);
}

void consume_queue_items()
{
    while(true)
    {
        int item_index;
        if(item_index=count.fetch_sub(1,std::memory_order_acquire)<=0)
        {
            continue;
        }
        std::cout<<"Consumed item: "<<queue_data[item_index-1]<<std::endl;
    }
}

int main()
{
    std::thread t1(populate_queue);
    std::thread t2(consume_queue_items);
    std::thread t3(consume_queue_items);
    t1.join();
    t2.join();
    t3.join();
    return 0;   
}

