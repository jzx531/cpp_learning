/*
建造者模式是一种创建型设计模式，使用多个简单的对象一步一步构建出一个复杂的对象，能够将一个复杂的对象的构建与它的表示分离，提供一种创建对象的最佳方式。主要用于解决对象的构建过于复杂的问题。
建造者模式主要基于4个核心类实现：

抽象产品类；
具体产品类：一个具体的产品对象类；
抽象pubic类：创建一个产品对象所需的各个部件的抽象接口；
具体产品的Builder类：实现抽象接口，构建各个部件；
指挥者Director类：统一组建过程，提供给调用者使用，通过指挥者来构建产品；
*/

#include <iostream>
#include <memory>

/* 抽象电脑类 */
#include <iostream>
#include <memory>

class Computer
{
public:
    using ptr = std::shared_ptr<Computer>;

    Computer() {};

    void setBoard(const std::string &board) { _board = board ;}
    void setDisplay(const std::string &display) {_display = display;}
    virtual void setOs() = 0;
    std::string show()
    {
        std::string computer = "Computer:[\n";
        computer += "\tboard: " + _board + "\n";
        computer += "\tdisplay: " + _display + "\n";
        computer += "\tos: " + _os + "\n";
        computer += "]\n";
        return computer;
    }
protected:
    std::string _board;
    std::string _display;
    std::string _os;
};

/* 具体产品类 */
class MacBook : public Computer
{
public:
    using ptr = std::shared_ptr<MacBook>;
    MacBook() {}
    virtual void setOs() 
    {
        _os = "Mac Os X12";
    }
};

/* 抽象建造者类：包含创建一个产品对象各个部件的抽象接口 */
class Builder
{
public:
    using ptr = std::shared_ptr<Builder>;
    virtual void buildBoard(const std::string &board) = 0;
    virtual void buildDisplay(const std::string &display) = 0;
    virtual void buildOS() = 0;
    virtual Computer::ptr build() = 0;
};

/* 具体产品的具体建造者类：实现抽象接口，构建和组装各个部件 */
class MacBookBuilder : public Builder
{
public:
    using ptr = std::shared_ptr<MacBookBuilder>;
    MacBookBuilder() : _computer(new MacBook()) {}
    virtual void buildBoard(const std::string &board) 
    {
        _computer->setBoard(board);
    }

    virtual void buildDisplay(const std::string &display)
    {
        _computer->setDisplay(display);
    }

    virtual void buildOS()
    {
        _computer->setOs();
    }

    virtual Computer::ptr build()
    {
        return _computer;
    }
private:
    Computer::ptr _computer;
};

/* 指挥者类：提供给调用者使用，通过指挥者来构造复杂的产品 */
class Director
{
public:
    Director(Builder* builder) : _builder(builder) {}
    void construct(const std::string &board, const std::string &display)
    {
        _builder->buildBoard(board);
        _builder->buildDisplay(display);
        _builder->buildOS();
    }
private:
    Builder::ptr _builder;
};

int main()
{
    Builder* builder = new MacBookBuilder();
    std::unique_ptr<Director> pd(new Director(builder));
    pd->construct("华为主板", "VOC显示器");
    Computer::ptr computer = builder->build();
    std::cout << computer->show() ;

    return 0;
}