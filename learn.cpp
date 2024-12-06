// #include <iostream>

// // 递归版本：存储多个元素的tuple
// template<typename T, typename... Args>
// class tuple {
// private:
//     T data;               // 当前元素
//     tuple<Args...>next; // 下一个元素

// public:
//     tuple(T t, Args... args) : data(t), next(tuple<Args...>(args...)) {}
//     tuple(const tuple<T, Args...>& other) : data(other.data), next(other.next) {}


//     void print() const {
//         std::cout << data << " ";
//         next.print(); // 递归打印后续元素
//     }

//     T getdata() const {
//         return data;  // 返回当前元素
//     }

//     tuple<Args...> getnext() {
//         return next;  // 返回下一个元素
//     }
// };

// // 特化版本：只有一个元素的tuple
// template<typename T>
// class tuple<T> {
// private:
//     T data;

// public:
//     tuple(T t) : data(t) {}

//     void print() const {
//         std::cout << data << std::endl;  // 只有一个元素时，打印
//     }

//     T getdata() const {
//         return data;
//     }

// };


// int main() {
//     tuple<int, char, double, float> t(1, 'a', 3.14, 2.71);
    
//     // 打印所有元素
//     t.print();  // 输出: 1 a 3.14 2.71
//     // 访问第2个元素
    
//     return 0;
// }


// #include <iostream>
// #include "Stack.hpp"

// int main() {
//     Stack<int> s;
//     s.push(1);
//     s.push(2);
//     s.push(3);
//     s.push(4);
//     s.push(5);

//     std::cout<<s;

//     // s.peek(); // 输出栈顶元素
//     while (!s.isEmpty()) {
//         std::cout << s.rear() << " ";
//         s.peek();
//     }
//     std::cout << std::endl;

//     return 0;
// }

#include <iostream>
#include "threadBtree.hpp"

int main() {
    // threadBtree<int> t;
    // t.insert(5);
    // t.insert(2);
    // t.insert(7);
    // t.insert(1);
    // t.insert(3);
    // t.insert(6);
    // t.insert(4);
    // t.insert(8);
    // t.inOrderThread();
    // t.insert_after_inThreading(0);
    // t.insert_after_inThreading(9);
    // t.levelOrder();
    // std::cout << '\n';

    std::cout <<"normal inOrderThread:"<<'\n';
    threadBtree<int> newt;
    newt.insert(5);
    newt.insert(2);
    newt.insert(7);
    newt.insert(1);
    newt.insert(3);
    newt.insert(6);
    newt.insert(4);
    newt.insert(8);
    // std::cout << newt.remove(2)<<'\n';
    // std::cout << newt.remove(7)<<'\n';
    // std::cout << newt.remove(5)<<'\n';
    newt.postOrderThread();

    newt.levelOrder();
    newt.inOrder();

    return 0;
}
