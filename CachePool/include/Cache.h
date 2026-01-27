#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>

template<typename T>
class Cache;

template<typename T>
class DLinkedNode{
   friend class Cache<T>;
   public: 
      int key;
      T value;
      DLinkedNode* prev;
      DLinkedNode* next;
      DLinkedNode(): key(0), value(T()), prev(nullptr), next(nullptr) {}
      DLinkedNode(int k, const T& v): key(k), value(v), prev(nullptr), next(nullptr) {}
};

template<typename T>
class  Cache
{
    protected:
    
        std::unordered_map<int, DLinkedNode<T>*> cache_map;
        DLinkedNode<T>* head;
        DLinkedNode<T>* tail;
        int size;
        int capacity;

    public:
        Cache(int cap):capacity(cap), size(0), head(nullptr), tail(nullptr) {};
        ~Cache()=default;
        virtual T get(int key)=0;
        virtual void put(int key, const T& value)=0;
};

#endif