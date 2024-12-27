#pragma once
#include <iostream>

template<typename K, typename E>
class dictionary 
{
    public:
        virtual ~dictionary() {}
        virtual bool empty() const = 0;
        virtual int size() const = 0;
        virtual pair<const K, E>* find(const K& key) const = 0;
        virtual void erase(const K& key) = 0;
        virtual void insert(const pair<const K, E>& p)=0;

};

