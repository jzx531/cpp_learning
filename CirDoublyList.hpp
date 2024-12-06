#pragma once

#include <iostream>

template <typename T>
struct DoubleNode
{
    T data;
    DoubleNode<T> *prev;
    DoubleNode<T> *next;
    DoubleNode(T data) : data(data), prev(nullptr), next(nullptr) {}
    DoubleNode(){prev = nullptr; next = nullptr;}
};

template <typename T>
class CirDoublyList
{
    public:
     DoubleNode<T> *head;
     CirDoublyList();
     CirDoublyList(T values[],int n);
     CirDoublyList(const CirDoublyList<T> & list);
     ~CirDoublyList();

     bool empty() const;
     int count() const;
     T & get(int i);
     virtual void set(int i, T x);
        template <typename U>
        friend std::ostream& operator<<(std::ostream& os, const CirDoublyList<U>& list);
    //  friend std::ostream & operator<<(std::ostream & os, 
    //                                  const CirDoublyList<T> & list);
     void printPreviews();
     virtual DoubleNode<T> * insert(T x);
     virtual DoubleNode<T> * insert(int i,T x);
     T remove(int i);
     void removeAll();
     DoubleNode<T> * search(T x);
     virtual void operator+=(CirDoublyList<T> & list);
     T operator[](int i);
};

template <typename T>
CirDoublyList<T>::CirDoublyList()
{
    this->head = new DoubleNode<T>();
    this->head->prev = this->head;
    this->head->next = this->head;
}

template <typename T>
CirDoublyList<T>::CirDoublyList(T values[], int n)
{
    this->head = new DoubleNode<T>();
    this->head->prev = this->head;
    this->head->next = this->head;

    DoubleNode<T> *p = new DoubleNode<T> ();
    for(int i=0;i<n;i++)
    {
        p=this->insert(values[i]);
    }
}

template <typename T>
CirDoublyList<T>::CirDoublyList(const CirDoublyList<T> & list)
{
    this->head = new DoubleNode<T>();
    this->head->prev = this->head;
    this->head->next = this->head;

    DoubleNode<T> *p = new DoubleNode<T> ();
    DoubleNode<T> *q = list.head->next;
    while(q!= list.head)
    {
        p = this->insert(q->data);
        q = q->next;
    }
}

template <typename T>
CirDoublyList<T>::~CirDoublyList()
{
    DoubleNode<T> *p = this->head->next;
    while(p!= this->head)
    {
        DoubleNode<T> *tmp = p;
        p = p->next;
        delete tmp;
    }
    delete this->head;
}

template <typename T>
bool CirDoublyList<T>::empty() const
{
    return this->head->next == this->head;
}

template <typename T>
int CirDoublyList<T>::count() const
{
    int cnt = 0;
    DoubleNode<T> *p = head->next;
    while(p!= this->head)
    {
        cnt++;
        p = p->next;
    }
    return cnt;
}

template <typename T>
T & CirDoublyList<T>::get(int i)
{
    DoubleNode<T> *p = this->head->next;
    if(i<0 || i>=this->count())
    {
        throw "Index out of range";
        delete  p;
        // return T();
    }
    else 
    {
        
        for (int j = 0; j < i; j++)
        {
            p = p->next;
        }
    }
    T outdata = p->data;
    return p->data;
};

template <typename T>
void CirDoublyList<T>::set(int i, T x)
{
    if(i<0 || i>=this->count())
    {
        throw "Index out of range";
    }
    else 
    {
        DoubleNode<T> *p = this->head->next;
        for (int j = 0; j < i; j++)
        {
            p = p->next;
        }
        p->data = x;
    }
};

template <typename T>
std::ostream & operator<<(std::ostream & os, 
                                 const CirDoublyList<T> & list)
{
    if(list.empty())
    {
        os << "empty list\n";
        return os;
    }
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head)
    {
        os << p->data << " ";
        p = p->next;
    }
    os << '\n';
    return os;
}

template <typename T>
void CirDoublyList<T>::printPreviews()
{
     DoubleNode<T> *p = this->head->prev;
     while(p!= this->head)
     {
         std::cout << p->data << " ";
         p = p->prev;
     }
     std::cout << '\n';
}

template <typename T>
DoubleNode<T> * CirDoublyList<T>::insert(T x)
{
    DoubleNode<T> *p = new DoubleNode<T>(x);
    // DoubleNode<T> *tail = new DoubleNode<T>();
    // tail = head->prev;
    DoubleNode<T> *tail = head->prev;
    p->prev = tail;
    p->next = head;
    tail->next = p;
    head->prev = p;
    return p;
}

template <typename T>
DoubleNode<T> * CirDoublyList<T>::insert(int i,T x)
{
    if(i<0 || i>count())
    {
        throw "Index out of range";
        return nullptr;
    }
    else 
    {
        if(head->next == head && i == 0)
        {
            return insert(x);
        }
        else {
            DoubleNode<T> *p = this->head->next;
            for (int j = 0; j < i; j++)
            {
                p = p->next;
            }
            DoubleNode<T> *tmp = new DoubleNode<T>(x);
            tmp -> prev = p->prev;
            tmp -> next = p;
            p->prev->next = tmp;
            return tmp;
        }
    }
}

template <typename T>
T CirDoublyList<T>::remove(int i)
{
    if(i<0 || i>=count())
    {
        throw "Index out of range";
        return T();
    }
    else 
    {
        DoubleNode<T> *p = head->next;
        for (int j = 0; j < i; j++)
        {
            p = p->next;
        }
        T data = p->data;
        p->prev->next = p->next;
        p->next->prev = p->prev;
        delete p;
        return data;
    }
}

template <typename T>
void CirDoublyList<T>::removeAll()
{
    DoubleNode<T> *p = head->next;
    while(p!= head)
    {
        DoubleNode<T> *tmp = p;
        p = p->next;
        delete tmp;
    }
    head->next = head;
    head->prev = head;
}

template <typename T>
DoubleNode<T> * CirDoublyList<T>::search(T x)
{
    DoubleNode<T> *p = this->head->next;
    while(p!= this->head)
    {
        if(p->data == x)
        {
            return p;
        }
        p = p->next;
    }
    return nullptr;
}

template <typename T>
void CirDoublyList<T>::operator+=(CirDoublyList<T> & list)
{
    DoubleNode<T> *p = list.head->next;
    while(p!= list.head)
    {
        this->insert(p->data);
        p = p->next;
    }
}

template <typename T>
T CirDoublyList<T>::operator[](int i)
{
    return get(i);
}





