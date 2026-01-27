#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>


template<typename T>
class  Cache
{
    protected:
        int size;
        int capacity;

    public:
        Cache(int cap):capacity(cap), size(0) {};
        ~Cache()=default;
        virtual T get(int key)=0;
        virtual void put(int key, const T& value)=0;
};

#endif