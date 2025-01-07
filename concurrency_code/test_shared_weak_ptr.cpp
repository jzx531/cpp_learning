#include <iostream>
#include <utility>
#include <memory>

struct node_weak;
struct node{
    std::shared_ptr<node_weak> next;
    // std::shared_ptr<node> next;
    ~node() {
        std::cout<<"delete node"<<std::endl;
    }
};

struct node_weak{
    std::weak_ptr<node> next;
    ~node_weak() {
        std::cout<<"delete node_weak"<<std::endl;
    }
};

int main() {
    std::shared_ptr<node> head = std::make_shared<node>();
    std::shared_ptr<node_weak> tail=std::make_shared<node_weak>();
    // std::shared_ptr<node> tail = std::make_shared<node>();
    tail->next = head;
    head->next = tail;
    head.reset(); 
    tail.reset();
    std::cout<<head.use_count()<<std::endl; // 1
    std::cout<<tail.use_count()<<std::endl; // 2
    return 0;
}