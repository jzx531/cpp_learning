/* 
    工厂方法模式：定义一个创建对象的接口，但是由子类来决定创建哪种对象，使用多个工厂分别生产指定的固定产品
        优点：
            1.减轻了工厂类的负担，将某产品的生产交给指定的工厂来进行；
            2.开闭原则遵循较好，添加新产品只需要新增产品的工厂即可，不需要修改原先的工厂类。
        缺点：
            对于某种可以形成一组产品族的情况处理较为复杂，需要创建大量的工厂类。
    
*/
#include <iostream>
#include <string>
#include <memory>

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
private:
    std::string _color;
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
    virtual std::shared_ptr<Fruit> create() = 0;
};

class AppleFactory : public FruitFactory
{
public:
    virtual std::shared_ptr<Fruit> create()
    {
        return std::make_shared<Apple>();
    }
};

class BananaFactory : public FruitFactory
{
public:
    virtual std::shared_ptr<Fruit> create()
    {
        return std::make_shared<Banana>();
    }
};

int main()
{
    std::shared_ptr<FruitFactory> factory(new AppleFactory());
    std::shared_ptr<Fruit> fruit = factory->create();
    fruit->name();

    factory.reset(new BananaFactory());
    fruit = factory->create();
    fruit->name();

    return 0;
}