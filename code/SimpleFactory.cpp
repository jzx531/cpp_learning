/*
    简单工厂模式：通过参数控制可以生产任何商品
        优点：简单粗暴、直观易懂。使用一个工厂生产同一等级结构下的任意商品
        缺点：
            1.所有东西生产在一起，产品太多会导致代码量庞大；
            2.开闭原则遵守不是太好，要新增产品就必须修改工厂方法。
*/
#include <iostream>

class Fruit
{
public:
    Fruit() {}
    virtual void name() = 0;
};

class Apple : public Fruit
{
public:
    Apple() {}
    virtual void name()
    {
        std::cout << "苹果" << std::endl;
    }
};

class Banana : public Fruit
{
public:
    Banana() {}
    virtual void name()
    {
        std::cout << "香蕉" << std::endl;
    }
};

class FruitFactory
{
public:
    static std::shared_ptr<Fruit> create(const std::string &name)
    {
        if (name == "苹果")
            return std::make_shared<Apple>();
        else if (name == "香蕉")
            return std::make_shared<Banana>();
        return std::shared_ptr<Fruit>();
    }
};

int main()
{
    std::shared_ptr<Fruit> ff = FruitFactory::create("苹果");
    ff->name();

    ff = FruitFactory::create("香蕉");
    ff->name();
    return 0;
}