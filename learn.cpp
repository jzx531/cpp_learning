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
// #include "Btree.hpp"
// #include "Huffmantree(Btree).hpp"

// #include "CirDoublyList.hpp"
// #include "Stack.hpp"

// int main() {

//     int a[] = {1,2,3,4};
//     HuffmanTree<int> ht(a, 4);
//     ht.inorder();
//     std::cout << '\n';
//     ht.levelOrder();
//     std::string code[4];
//     int freq[4] = {1,2,5,7};
//     Node<int>* OUT[10];
//     int length = 0;
//     ht.find(2, OUT,length);
//     std::cout << "length: " << length << '\n';
//     for (int i = 0; i < length; i++) {
//         std::cout << OUT[i]->value << " ";
//     }
//     std::cout << '\n';

//     printHuffmanCode(a, 4,code);

//     std::string decoder = "011101100";
//     std::string decoded = ht.decode(decoder, 9);
//     std::cout << "decoded: " << decoded << '\n';

//     return 0;
// }

// #include <iostream>
// // #include "SeqCirDoublyList.hpp"
// #include "Huffmantree.hpp"

// template<typename T>
// class Node{
//     T data;
// };

// template<typename T>
// class Seq{
//     T data;
// };

// int main() {
//     TriNode<int> node1(1);
//     TriNode<int> node2(2);
//     TriNode<int> node3(3);
//     std::cout<<(node1<node2);

//     node1.left = &node2;
//     node1.right = &node3;

//     TriNode<int> *nodeptr=nullptr;
//     nodeptr = new TriNode<int>(node1);
//     TriNode<int>* p = new TriNode<int>(node1);
//     std::cout<<nodeptr->data<<std::endl;
//     std::cout<<nodeptr->left->data<<std::endl;
//     std::cout<<nodeptr->right->data<<std::endl;

//     int a[] = {4,3,2,1};
//     Huffmantree <int> ht(a, 4);
//     ht.levelOrder();
//     std::string code [4];
//     printHuffmanCode(a, 4, code);

//     std::string decoder = "011101100";
//     std::string decoded = ht.decode(decoder, 9);
//     std::cout << "decoded: " << decoded << '\n';

//     return 0;
// }

#include <iostream>
#include "LZWcode.hpp"

int main() {
    LZWcode lzw;
    std::cout<<lzw.encode("abbababac\0")<<std::endl;
    std::cout<<lzw.decode("97 98 98 256 259 99\0");
    return 0;
}