/*
    抽象工厂：围绕一个超级工厂去创建其他工厂。每个生成的工厂按照工厂模式提供对象。
        思想：将工厂抽象成两层，抽象工厂 & 具体工厂子类，在工厂子类中生产不同类型的子产品
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
    void name() override
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
    void name() override
    {
        std::cout << "香蕉" << std::endl;
    }
};

class Animal
{
public:
    virtual void voice() = 0;
};

class Cat : public Animal
{
public:
    void voice() override
    {
        std::cout << "喵喵喵" << std::endl;
    }
};

class Dog : public Animal
{
public:
    void voice() override
    {
        std::cout << "汪汪汪" << std::endl;
    }
};

class Factory
{
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};

class FruitFactory : public Factory
{
public:
    std::shared_ptr<Animal> getAnimal(const std::string &name) override
    {
        return std::shared_ptr<Animal>();
    }

    std::shared_ptr<Fruit> getFruit(const std::string &name) override
    {
        if (name == "苹果")
            return std::make_shared<Apple>();
        else if (name == "香蕉")
            return std::make_shared<Banana>();
        return std::shared_ptr<Fruit>();
    }
};

class AnimalFactory : public Factory
{
public:
    std::shared_ptr<Fruit> getFruit(const std::string &name) override
    {
        return std::shared_ptr<Fruit>();
    }

    std::shared_ptr<Animal> getAnimal(const std::string &name) override
    {
        if (name == "小猫")
            return std::make_shared<Cat>();
        else if (name == "小狗")
            return std::make_shared<Dog>();
        return std::shared_ptr<Animal>();
    }
};

class FactoryProducer
{
public:
    static std::shared_ptr<Factory> getFactory(const std::string &name)
    {
        if (name == "水果")
            return std::make_shared<FruitFactory>();
        else if(name == "动物")
            return std::make_shared<AnimalFactory>();
    }
};

int main()
{
    std::shared_ptr<Factory> fruit_factory = FactoryProducer::getFactory("水果");
    std::shared_ptr<Fruit> fruit = fruit_factory->getFruit("苹果");
    fruit->name();
    fruit = fruit_factory->getFruit("香蕉");
    fruit->name();

    std::shared_ptr<Factory> animal_factory = FactoryProducer::getFactory("动物");
    std::shared_ptr<Animal> animal = animal_factory->getAnimal("小猫");
    animal->voice();
    animal = animal_factory->getAnimal("小狗");
    animal->voice();
    
    return 0;
}