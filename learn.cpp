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
// #include "CirDoublyList.hpp"

// int main() {
//     int a[] = {1, 2, 3, 4, 5};
//     CirDoublyList<int> list(a, 5);
//     std::cout<<list;
//     list.reverse();
//     std::cout<<list;
//     return 0;
// }

#include <iostream>
#include "sort.hpp"
int main() {
    int a[] = {5, 3, 8, 6, 2, 7, 1, 4};
    int n = sizeof(a) / sizeof(a[0]);
    QuickSort(a, 0,n-1);
    PrintArray(a, n);
    return 0;
}



