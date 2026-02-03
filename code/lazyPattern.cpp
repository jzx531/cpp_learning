#include <iostream>
#include <string>

// 懒汉模式
//懒汉模式在第一次使用单例对象的时候创建实例对象，如果单例对象构造特别耗时或者耗费资源
// 可以选择懒汉模式，在第一次使用的时候才创建对象
class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    static Singleton &getInstance()
    {
        static Singleton _eton; 
        return _eton;
    }
    std::string &getData()
    {
        return _data;
    }
private:
    Singleton() : _data("Singleton")
    {
        std::cout << "单例对象构造" << std::endl;
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