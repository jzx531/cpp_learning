#pragma once
#include <iostream>

template<typename T>
class  arrayQueue
{
    private:
        T* arr;
        int front;
        int rear;
        int length;
    public:
        arrayQueue(int size = 10)
        {
            arr = new T[size];
            front = 0;
            rear = 0;
            this->length = size;
        }
        ~arrayQueue();
        bool isEmpty() const;
        bool isFull() const;
        
        bool enqueue(T data);
        T dequeue();
        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const arrayQueue<U>& q) ;
        void clear();
};

template<typename T>
arrayQueue<T>::~arrayQueue()
{
    delete[] arr;
}

template<typename T>
bool arrayQueue<T>::isEmpty() const
{
    return (this->front ==this->rear);
}

template<typename T>
bool arrayQueue<T>::isFull() const
{
    return ((rear+1)%length == front);
}

template<typename T>
bool arrayQueue<T>::enqueue(T data)
{
    if(this->isFull())
    {
        // std::cout<<"Queue is full"<<std::endl;
        // return false;
        T* new_arr = new T[2*length];
        int j = 0;
        for(int i = front; i!= rear; i=(i+1)%length)
        {
            new_arr[j++] = arr[i];
        }
        delete[] arr;
        arr = new_arr;
        length *= 2;
        front = 0;
        rear = j;
    }
    arr[this->rear] = data;
    this->rear = (this->rear+1)%length;
    return true;

}

template<typename T>
T arrayQueue<T>::dequeue()
{
    if(this->isEmpty())
    {
        std::cout<<"Queue is empty"<<std::endl;
        return T();
    }
    else
    {
        T data = arr[this->front];
        this->front = (this->front+1)%length;
        return data;
    }
}

template<typename U>
std::ostream& operator<<(std::ostream& os, const arrayQueue<U>& q) 
{
    os<<"Queue: ";
    for(int i=q.front;i!=q.rear;i=(i+1)%q.length)
    {
        os<<q.arr[i]<<" ";
    }
    // os<<q.arr[q.rear]<<std::endl;
}

template<typename T>
void arrayQueue<T>::clear()
{
    this->front = 0;
    this->rear = 0;
}
