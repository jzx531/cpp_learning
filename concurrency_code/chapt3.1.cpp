#include <iostream>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include<algorithm>
#include<list>
std::mutex some_mtx;
std::list<int> some_list;
void add_to_list(int new_value)
{
    std::lock_guard<std::mutex> guard(some_mtx);
    some_list.push_back(new_value);
}

bool list_contains(int value)
{
    std::lock_guard<std::mutex> guard(some_mtx);
    return std::find(some_list.begin(), some_list.end(), value) != some_list.end();
}

int main()
{
    add_to_list(1);
    add_to_list(2);
    add_to_list(3);
    add_to_list(4);
    add_to_list(5);
    std::cout << "List contains 3: " << list_contains(3) << std::endl;
    std::cout << "List contains 6: " << list_contains(6) << std::endl;
    return 0;
}