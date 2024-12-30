#include <iostream>
#include "mingw.thread.h"
struct func{
    int  i;
    func(int i_):i(i_){};
    void operator()(){
        for(int j=0;j<=10000;j++){
            i++;
            if(i%1000==0)
            {
                std::cout<<i<<std::endl;
            }
        }
    }
};
void test(){
    int state=0;
    func f(state);
    std::thread t(f);
    t.detach();
}

int main(){
    test();
    return 0;
}
