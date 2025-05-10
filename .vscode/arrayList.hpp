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



