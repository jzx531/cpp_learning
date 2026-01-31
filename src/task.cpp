#include "task.hpp"


void task1()
{
    std::cout<<"I am childprocess: "<<getpid()<<" running task1"<<std::endl;
}
void task2()
{
    std::cout<<"I am childprocess: "<<getpid()<<" running task2"<<std::endl;
}
void task3()
{
    std::cout<<"I am childprocess: "<<getpid()<<" running task3"<<std::endl;
}
void task4()
{
    std::cout<<"I am childprocess: "<<getpid()<<" running task4"<<std::endl;
}