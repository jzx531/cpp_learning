#include <iostream>
#include <string>

//使用饿汉模式在程序启动时就会创建一个唯一的实例对象，因为单例对象已经确定
// 所以比较适用于多线程环境中，多线程获取单例对象不需要加锁，可以有效的避免资源的竞争，提高性能

// 饿汉模式
class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static Singleton& getInstance()
    {
        return _eton;
    }
    std::string &getData()
    {
        return _data;
    }
private:
    Singleton() :_data("Singleton") 
    {
        std::cout << "获取单例对象" <<std::endl;
    }
    ~Singleton() {}
private:
    static Singleton _eton;
    std::string _data;
};

Singleton Singleton::_eton;

int main()
{
    std::cout << Singleton::getInstance().getData() << std::endl;
    return 0;
}