#pragma once
#include <iostream>
#include <utility>
#include <functional>

//把一个字符串转换为一个不唯一的整数
int stringToInt(const std::string& str) {
    int answer = 0;
    int length = str.length();
    if(length%2==1) {
        answer = str.at(length-1);
        length--;
    }
    for(int i=0;i<length;i+=2) {
        answer+=str.at(i);
        answer+=((int)str.at(i+1))<<8;
    }
    return answer<0?-answer:answer;
}
/* //MinGw编译器下，hash<string>无法正常工作，故注释掉
namespace std {
    template<>
    struct hash<string>
    {
        public:
            size_t operator()(const string& str) const {
            unsigned long hashValue = 0;
            int  length = (int) str.length();
            for(int i=0;i<length;i++) {
                    hashValue = 5*hashValue + str[i];
            }
            return (size_t)hashValue;
            }
    };
}*/

template<typename K,typename E>
class HashTable
{
    private:
        std::pair<K,E>** table;
        std::hash<K> hash;
        int dsize;
        int divisor;
    public:
        HashTable(int theDivisor=100) ;
        int search(const K& key) const ;
        std::pair<K,E>* find(const K& key) const ;
        void insert(const std::pair<K,E>& thePair) ;

};

template<typename K,typename E>
HashTable<K,E>::HashTable(int theDivisor)
{
    divisor = theDivisor;
    dsize = 0;
    table = new std::pair<K,E>*[divisor];
    for(int i=0;i<divisor;i++) {
        table[i] = nullptr;
    }
}

template<typename K,typename E>
int HashTable<K,E>::search(const K& key) const
{
    int index = this->hash(key) % divisor;
    int j = index;
    do
    {
       if(table[j]==nullptr||table[j]->first==key)
           return j;
        j = (j+1) % divisor;
    } while (j != index);
    return j;
}

template<typename K,typename E>
std::pair<K,E>* HashTable<K,E>::find(const K& key) const
{
    int b = search(key);
    if(table[b]==nullptr||table[b]->first!=key)
        return nullptr;
    return table[b];
}
    
template<typename K,typename E>
void HashTable<K,E>::insert(const std::pair<K,E>& thePair)
{
    int index = this->hash(thePair.first) % divisor;
    int j = index;
    do
    {
        if(table[j]==nullptr)
        {
            table[j] = new std::pair<K,E>(thePair);
            dsize++;
            return;
        }
        else if(table[j]->first==thePair.first)
        {
            table[j]->second = thePair.second;
            return;
        }
        else j = (j+1) % divisor;
    } while (j != index);
    std::cout<<"Hash table is full!"<<std::endl;
}