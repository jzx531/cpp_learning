#pragma once
#include <iostream>
#include "CirDoublyList.hpp"

template<typename T>
class Stack:public CirDoublyList<T>{
public:
    Stack():CirDoublyList<T>(){};
    ~Stack();
    bool isEmpty() const;
    // bool isFull() const;
    void push(int data);
    T pop();
    T peek();
    T top() const;
    T rear() const;

    template<typename U>
    friend std::ostream& operator<<(std::ostream& os,  Stack<U> stack);
};

template<typename T>
Stack<T>::~Stack(){
//     while(!isEmpty()){
//         pop();
//     }
}
template<typename T>
bool Stack<T>::isEmpty() const{
    return this->empty();
}

template<typename T>
void Stack<T>::push(int data){
    this->insert(data);
}

template<typename T>
T Stack<T>::pop(){
    if(isEmpty()){
        std::cout << "Stack is empty!" << std::endl;
        return T();
    }
    T data = this->remove(this->count() - 1);
    return data;
}

template<typename T>
T Stack<T>::peek() {
    if(isEmpty()){
        std::cout << "Stack is empty!" << std::endl;
        return T();
    }
    T data = this->remove(0);
    return data;
}

template<typename T>
T Stack<T>::top() const{
    if(isEmpty()){
        std::cout << "Stack is empty!" << std::endl;
        return T();
    }
    return this->head->prev->data;
}

template<typename T>
T Stack<T>::rear() const{
    if(isEmpty()){
        std::cout << "Stack is empty!" << std::endl;
        return T();
    }
    return this->head->next->data;
}

template<typename T>
std::ostream& operator<<(std::ostream& os,  Stack<T> stack){
    os << "Stack: ";
    while (!stack.isEmpty())
    {
        os << stack.top() << " ";
        stack.pop();
    }
    
    os << '\n';
    return os;
}






