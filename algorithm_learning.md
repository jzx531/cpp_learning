# C++数据结构与算法分析

[TOC]

## 预备知识
```c++
#include <iostream>
using namespace std;

class a
{
    public:
    int num;
    const int& getNum() const;
    
};

const int& a::getNum() const
{
    a *const fakethis = const_cast<a*>(this);
    return fakethis->num;
}

int main()
{
    a obj;
    obj.num = 10;
    cout << obj.getNum() << endl;
    return 0;
}

```
使用const_cast将this指针转换为常量指针，从而实现const修饰的getNum()方法。
如果没有关键字mutable,为了让编译器同意在const member functions内修改data members使用冒牌this指针

```c++
#include <iostream>
using namespace std;

void printNum(const int& num)
{
    cout << num << endl;
    throw "Error";
}

void usingprintNum(int num)
{
    try {
        printNum(num);
    }
    catch(const char* e)
    {
        cout << e << endl;
        exit(1);
    }
}

int main()
{
    int num = 10;
    usingprintNum(num);
    return 0;
}
```


递归算法求解排列组合数
```c++
#include <iostream>
#include <algorithm>
#include <iterator>
template<typename T>
void swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
};

template<typename T>
void permutions(T arr[], int s, int e)
{
    if (s == e)
    {
        std::copy(arr, arr + e + 1, std::ostream_iterator<T>(std::cout, " "));
        std::cout << '\n';
        return;
    }
    else 
    {
        for (int i = s; i <= e; i++)
        {
            swap(arr[s], arr[i]);
            permutions(arr, s+1, e);
            swap(arr[s], arr[i]);
        }

    }
};

int main()
{
    int arr[] = {1, 2, 3, 4};
    int n = sizeof(arr)/sizeof(arr[0]);
    // std::cout<<n<<std::endl;
    permutions(arr, 0, n-1);
    return 0;
}
```

双向循环链表
```c++
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
     DoubleNode<T> * insert(T x);
     virtual DoubleNode<T> * insert(int i,T x);
     T remove(int i);
     void removeAll();
     DoubleNode<T> * search(T x);
     virtual void operator+=(CirDoublyList<T> & list);
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
    //DoubleNode<T> p =head->next;
    // for(int i=0;i<n;i++)
    // {
    //     p = insert(values[i]);
    // }
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
```

顺序双向循环表
```c++
#pragma once

#include "CirDoublyList.hpp"

template<typename T>
class SeqCirDoublyList : public CirDoublyList<T> {
public:
    SeqCirDoublyList():CirDoublyList<T>() {}
    SeqCirDoublyList(T values[],int n);
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

```

名次排序算法
```c++
#include <iostream>
#include <iterator>
#include <algorithm>

template<typename T>
void rank(T a[],int n,int r[])
{
    for (int i = 0; i < n; i++)
    {
        r[i] = 0;
    }
    for(int i = 0 ;i<n;i++){
     for(int j =0 ;j<i;j++){
      if(a[j] > a[i])
       r[i]++;
       else
        r[j]++;
     }
    }
}

template<typename T>
void rearrange(T a[],int n,int r[])
{
    T* u = new T[n];
    for (int i =0;i<n;i++)
    {
        u[r[i]] = a[i];
    }
    //把u中元素移回a
    // for(int i = 0;i<n;i++)
    // {
    //     a[i] = u[i];
    // }
    memcpy(a,u,n*sizeof(T));
    delete[] u;
}

int main()
{
    int a[] = {6,7,8,9,10};
    const int n = sizeof(a)/sizeof(a[0]);
    std::cout<<n<<std::endl;
    int r[n];
    rank(a,n,r);
    std::copy(r,r+n-1,std::ostream_iterator<int>(std::cout," "));
    std::cout<<std::endl;
    rearrange(a,n,r);
    std::copy(a,a+n,std::ostream_iterator<int>(std::cout," "));
    return 0;
}
```

选择排序
```c++
#include <iostream>
#include <algorithm>

template<typename T>
int indexofMax(T arr[], int n) {
    int maxIndex = 0;
    for(int i=1; i<n; i++) {
        if(arr[i] > arr[maxIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
}

template<typename T>
void selectionSort(T arr[], int n) {
    for(int i=0; i<n-1; i++) {
        int maxIndex = indexofMax(arr, n-i);
        std::swap(arr[maxIndex], arr[n-i-1]);
    }
}

int main() {
    int arr[] = {6,7,8,9,10};
    selectionSort(arr, 5);
    for(int i=0; i<5; i++) {
        std::cout<< arr[i]<<" ";
    }
    return 0;
}
```

及时终止的选择排序
```c++
#include <iostream>
#include <algorithm>

template<typename T>
void selectSort(T arr[], int n) {
    bool sorted = false;
    for(int size =n ;!sorted && size > 1 ; size--) {
        sorted = true;
        int indexofMax = 0;
        for(int i = 0 ; i < size; i++) {
            if(arr[indexofMax] <= arr[i]) {
                indexofMax = i;//判断每次循环的最大值索引是否都是最大值
                //如果新的值都比indexofMax大，则不需要交换，sorted=true，退出排序
            }
            else {
                sorted = false;
            }
        }
        std::swap(arr[indexofMax], arr[size-1]);
    }
}

int main() {
    int arr[] = {8,7,9,6,10};
    int n = sizeof(arr)/sizeof(arr[0]);
    selectSort(arr, n);
    for(int i = 0 ; i < n ; i++) {
        std::cout<<arr[i]<<" ";
    }
    return 0;
}
```
插入排序
```c++
#include <iostream>
#include <algorithm>

template<typename T>
void insert(T arr[],int n,const T& x)
{
    int i;
    for(i=n-1;i>=0&&x<arr[i];i--)
    {
        arr[i+1]=arr[i];
    }
    arr[i+1] =x;
}

template<typename T>
void insertsort(T arr[],int n)
{
    for(int i=1;i<n;i++)
    {
        T t = arr[i];
        insert(arr,i,t);
    }
}

template <typename T>
void insertionSort(T a[],int n)
{
    for(int i =1 ; i<n ; i++)
    {
        T t = a[i];
        int j ;
        for(j=i-1;j>=0 && a[j]>t;j--)
        {
            a[j+1] = a[j];
        }
        a[j+1] = t;
    }
}

int main() {
    int arr[] = {6,7,8,9,10};
    int n = sizeof(arr)/sizeof(arr[0]);
    insertsort(arr,n);
    for(int i=0;i<n;i++)
    {
        std::cout<<arr[i]<<" ";
    }
    std::cout<<std::endl;
    return 0;
}
```

使用circle doubly list实现的stack
```c++
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

```

使用基类linearList实现的arrayList

```c++
#pragma once
#include <iostream>

template <typename T>
class linearList{
    public:
    //  virtual ~linearList() = 0;
     virtual bool empty() const = 0;
     virtual int size() const = 0;
     virtual T& get(int theIndex) const = 0;
     virtual int indexOf(const T& theElement) const = 0;
     virtual void erase(int theIndex) = 0;
     virtual void insert(int theIndex ,const T & theElement)=0;
     virtual void output(std::ostream& out) const = 0;
};

template <typename T>
class arrayList : public linearList<T>
{
    public:
     arrayList(int initialCapacity = 10);
     arrayList(const arrayList<T>& theList);
     
     //ADT functions
     virtual ~arrayList();
     virtual bool empty() const{return listSize == 0;};
     virtual int size() const{return listSize;};
     virtual T& get(int theIndex) const;
     virtual int indexOf(const T& theElement) const;
     virtual void erase(int theIndex);
     virtual void insert(int theIndex, const T& theElement);
     virtual void output(std::ostream& out) const;
    // 特有方法
     int capacity() const{return arrayLength;};
    protected:
     T* element;
     int listSize; //元素个数
     int arrayLength; //数组容量
};

template <typename T>
arrayList<T>::arrayList(int initialCapacity)
{
    if(initialCapacity < 1)
     {
         std::cerr << "Error: initialCapacity must be greater than 0" << std::endl;
         exit(1);
     }
    arrayLength = initialCapacity;
    element = new T[arrayLength];
    listSize = 0;
}


template <typename T>
arrayList<T>::arrayList(const arrayList<T>& theList)
{
    arrayLength = theList.arrayLength;
    element = new T[arrayLength];
    listSize = theList.listSize;
    std::copy(theList.element, theList.element + listSize, element);
}

template <typename T>
arrayList<T>::~arrayList()
{
    delete[] element;
}

template <typename T>
T& arrayList<T>::get(int theIndex) const
{
    if(theIndex < 0 || theIndex >= listSize)
    {
        std::cerr << "Error: index out of range" << std::endl;
        exit(1);
    }
    return element[theIndex];
}

template <typename T>
int arrayList<T>::indexOf(const T& theElement) const
{
    int i;
    for(i = 0; i < listSize; i++)
    {
        if(element[i] == theElement)
        {
            return i;
        }
    }
    return -1;
}

template <typename T>
void arrayList<T>::erase(int theIndex)
{
    if(theIndex < 0 || theIndex >= listSize)
    {
        std::cerr << "Error: index out of range" << std::endl;
        exit(1);
    }
    for(int i = theIndex; i < listSize - 1; i++)
    {
        element[i] = element[i + 1];
    }
    listSize--;
}

template <typename T>
void arrayList<T>::insert(int theIndex, const T& theElement)
{
    if(theIndex < 0 || theIndex > listSize)
    {
        std::cerr << "Error: index out of range" << std::endl;
        exit(1);
    }
    if(listSize == arrayLength)
    {
        T* newArray = new T[arrayLength * 2];
        std::copy(element, element + arrayLength, newArray);
        delete[] element;
        element = newArray;
        arrayLength *= 2;
    }
    for(int i = listSize; i > theIndex; i--)
    {
        element[i] = element[i - 1];
    }
    element[theIndex] = theElement;
    listSize++;
}

template <typename T>
void arrayList<T>::output(std::ostream& out) const
{
    for(int i = 0; i < listSize; i++)
    {
        out << element[i] << " ";
    }
    out << std::endl;
}
```


```c++
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
```

三种不同线索二叉树的线索化方法
```c++
// inOrderThread()是对普通Btree的一次性中序线索化
template<typename T>
void threadBtree<T>::inOrderThread(threadNode<T>* node,threadNode<T> *&front) {
    if(node != nullptr)
    {
        inOrderThread(node->left,front);//处理左子树
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;
        inOrderThread(node->right,front);//处理右子树
    }
    return;
}
// 前序线索化代码和中序线索化代码极为相似，
// 最大的区别就是把连接线索的代码提到了两递归入口前边，
// 这也符合先序递归遍历的框架。
template<typename T>
void threadBtree<T>::preOrderThread(threadNode<T>* node,threadNode<T> *&front) 
{
    if(node != nullptr)
    {
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;

        if(!node ->ltag ) preOrderThread(node->left,front);
        if(!node ->rtag ) preOrderThread(node->right,front);
    }
}
// 后序线索化代码和中序线索化代码极为相似，
// 最大的区别就是把连接线索的代码提到了两递归入口后边，
// 这也符合后序递归遍历的框架。
template<typename T>
void threadBtree<T>::postOrderThread(threadNode<T>* node,threadNode<T> *&front)
{
    if(node != nullptr){
        preOrderThread(node->left,front);
        preOrderThread(node->right,front);
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;
    }
    return;
}
```
汉诺塔问题
```c++
#include<iostream>
#include"Stack.hpp"

Stack<int> tower[4];
void moveAndShow(int n,int x,int y,int z)
{
    if(n>0)
    {
        
        moveAndShow(n-1,x,z,y);
        int d=tower[x].top();
        tower[x].pop();
        tower[y].push(d);
        for (int i = 1; i <4; i++)
        {
            if (!tower[i].empty())
            {
                std::cout << i << ": ";
                std::cout << tower[i];
            }
        }
        moveAndShow(n-1,z,y,x);
    }
    return;
}

void towerOfHanoi(int n)
{
    for(int i=n;i>0;i--)
    {
        tower[1].push(i);
    }
    for (int i = 1; i <= 3; i++)
    {
        std::cout << "Tower " << i << ": " << tower[i] << std::endl;
    }
    moveAndShow(n,1,2,3);
}
```