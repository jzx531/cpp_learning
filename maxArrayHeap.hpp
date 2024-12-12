#pragma once
#include <iostream>
#include "arrayQueue.hpp"

template <typename T>
class MaxArrayHeap {
private:
    //使用数组实现的堆方便直接变为完全二叉树
    T* heap;
    int heapSize;
    int arraylength;
public:
    MaxArrayHeap(int arraylength=0):heapSize(0),arraylength(arraylength){};
    MaxArrayHeap(T* theHeap, int thesize);
    void initialize(T* theHeap, int thesize); 
    void push(const T& value);
    void pop();
    void levelOrder() const;
    void print() const; 
};

template<typename T>
MaxArrayHeap<T>::MaxArrayHeap(T* theHeap, int thesize) 
{  
    arraylength = thesize;
    heap = new T[arraylength];
    for (size_t i = 0; i < thesize; i++)
    {
        heap[i] = theHeap[i];
    }
    
    heapSize = thesize;
    //堆化
    for(int root = heapSize/2-1;root>=0;--root)
    {
        T rootElement = heap[root];
        //为元素rootElement寻找最佳位置
        int parent = root;
        int child = 2*parent+1;
        while(child<=heapSize)
        {
            if(child<heapSize  && heap[child]<heap[child+1])
                ++child;
            if(rootElement>=heap[child])
                break; //可以
            //不可以
            heap[parent] = heap[child];//把孩子上一到下一层
            parent = child;
            child = 2*parent+1;
        }
        heap[parent] = rootElement;
    }
}

template <typename T>
void MaxArrayHeap<T>::initialize(T* theHeap, int thesize) 
{
    if(heap!=nullptr){   
        delete[] heap;
    }
    heap = theHeap;
    heapSize = thesize;
    arraylength = thesize;
    //堆化
    heapSize = thesize;
    //堆化
    for(int root = heapSize/2-1;root>=0;--root)
    {
        T rootElement = heap[root];
        //为元素rootElement寻找最佳位置
        int parent = root;
        int child = 2*parent+1;
        while(child<=heapSize)
        {
            if(child<heapSize  && heap[child]<heap[child+1])
                ++child;
            if(rootElement>=heap[child])
                break; //可以
            //不可以
            heap[parent] = heap[child];//把孩子上一到下一层
            parent = child;
            child = 2*parent+1;
        }
        heap[parent] = rootElement;
    }
}

template <typename T>
void MaxArrayHeap<T>::push(const T& value) 
{
    if(heapSize==arraylength)
    {
        arraylength = 2*arraylength;
        T* newHeap = new T[arraylength];
        for(int i=0;i<heapSize;++i)
            newHeap[i] = heap[i];
        delete[] heap;
        heap = newHeap;
        // this->print();
    }
    int child = ++heapSize -1;
    std::cout<<arraylength<<std::endl;
    std::cout<<heapSize<<std::endl;
    std::cout<<child<<std::endl;
    while(child>0 && heap[(child-1)/2]<value)
    {
        heap[child] = heap[(child-1)/2];
        child = (child-1)/2;
    }
    heap[child] = value;
}

template <typename T>
void MaxArrayHeap<T>::pop() 
{
    if(heap == nullptr)
        return;
    heap[0] .~T();
    heapSize--;
    T lastelement = heap[heapSize];
    int current = 0, child = 2*current+1;
    while(child<heapSize)
    {
        if(child<heapSize && heap[child]<heap[child+1])
            ++child;
        if(lastelement>=heap[child])
            break;
        heap[current] = heap[child];
        current = child;
        child = 2*current+1;
    }
    heap[current] = lastelement;
}


template <typename T>
void MaxArrayHeap<T>::levelOrder() const 
{   
    arrayQueue<int> q; 
    q.enqueue(0);
    while(!q.isEmpty())
    {
        int root = q.dequeue();
        std::cout<<heap[root]<<' ';
        if(2*root+1<heapSize)
            q.enqueue(2*root+1);
        if(2*root+2<heapSize)
            q.enqueue(2*root+2);
    }
}
template <typename T>
void MaxArrayHeap<T>::print() const 
{
    for(int i=0;i<heapSize;++i)
        std::cout<<heap[i]<<' ';
    std::cout<<std::endl;
}
