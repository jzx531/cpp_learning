#include <iostream>
#include <string>
/*房东要把⼀个房⼦通过中介租出去理解代理模式*/

/* 租房类 */
class RentHouse
{
public:
    virtual void rentHouse() = 0;
};

/* 房东类 */
class Landlord : public RentHouse
{
    public:
        void rentHouse() 
        {
            std::cout << "将房子租出去\n" << std::endl;
        }
};

/* 中介类 */
class Intermedirary : public RentHouse
{
    public:
        void rentHouse()
        {
            std::cout << "发布告示" << std::endl;
            std::cout << "带人看房" << std::endl;
            _landlord.rentHouse();
            std::cout << "负责租后维修" << std::endl;

        }
    private:
        Landlord _landlord;
};

int main()
{
    Intermedirary intermedirary;
    intermedirary.rentHouse();

    return 0;
}