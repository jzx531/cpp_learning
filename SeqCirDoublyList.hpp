#pragma once

#include "CirDoublyList.hpp"

template<typename T>
class SeqCirDoublyList : public CirDoublyList<T> {
public:
    SeqCirDoublyList():CirDoublyList<T>() {}
    SeqCirDoublyList(T values[],int n);
    SeqCirDoublyList(const SeqCirDoublyList<T> & list);
    SeqCirDoublyList<T> & operator=(const SeqCirDoublyList<T> & list);
    ~SeqCirDoublyList(); 
    virtual void set(int i, T x);
    virtual DoubleNode<T> * insert(T x);
    // virtual DoubleNode<T> * insert(int i,T x);
    virtual void operator+=(CirDoublyList<T> & list);
    virtual void operator+=(SeqCirDoublyList<T> & list);
};

template<typename T>
SeqCirDoublyList<T>::SeqCirDoublyList(T values[],int n) {
    this->head = new DoubleNode<T>();
    this->head->next = this->head;
    this->head->prev = this->head;
    for(int i=0;i<n;i++) {
        this->insert(values[i]);
    }
}

template<typename T>
SeqCirDoublyList<T>::SeqCirDoublyList(const SeqCirDoublyList<T> & list) {
    this->head = new DoubleNode<T>();
    this->head->next = this->head;
    this->head->prev = this->head;
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head) {
        this->insert(p->data);
        p = p->next;
    }
}

template<typename T>
SeqCirDoublyList<T> & SeqCirDoublyList<T>::operator=(const SeqCirDoublyList<T> & list) {
    if(this == &list) {
        return *this;
    }
    this->clear();
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head) {
        this->insert(p->data);
        p = p->next;
    }
    return *this;
}

template <typename T>
SeqCirDoublyList<T>::~SeqCirDoublyList() {
    //子类会调用父类的析构函数，所以这里不用再写
}

template<typename T>
void SeqCirDoublyList<T>::set(int i, T x) {
    if(i<0 || i>=this->count()) {
        throw "Index out of range";
    }
    DoubleNode<T> *p = this->head->next;
    for(int j=0;j<i;j++) {
        p = p->next;
    }
    p->data = x;
}

template<typename T>
DoubleNode<T> * SeqCirDoublyList<T>::insert(T x) {
    DoubleNode<T> *p = new DoubleNode<T>(x);
    if (this->head->next == this->head))
    {
        this->head->next = p;
        this->head->prev = p;
        p->next = this->head;
        p->prev = this->head;
        return p;
    }
    
    DoubleNode<T> *current = this->head->next;
    while( current->data < x) {
        current = current->next;
        if(current == this->head) {
            break;
        }
    }
    p->prev = current->prev;
    p->next = current;
    current->prev->next = p;
    current->prev = p;
    return p;
}

template<typename T>
void SeqCirDoublyList<T>::operator+=(CirDoublyList<T> & list) {
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head) {
        // std::cout<<"+="<<" ";
        this->insert(p->data);
        p = p->next;
    }
}

template<typename T>
void SeqCirDoublyList<T>::operator+=(SeqCirDoublyList<T> & list) {
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head) {
        this->insert(p->data);
        p = p->next;
    }
}




