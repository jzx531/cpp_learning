#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "Cache.h"
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>

template<typename T>
class LFUCache;

// template<typename T>
class FreqNode{
    // friend class LFUCache<T>;
    public:
        int key;
        int freq;
    public:
        FreqNode():key(0), freq(0){};
        FreqNode(int k, int f):key(k), freq(f){};
        bool operator <(const FreqNode& other) const{
            return freq < other.freq;
        }
        bool operator ==(const FreqNode& other) const{
            return key == other.key;
        }
        bool operator <=(const FreqNode& other) const{
            return freq <= other.freq;
        }
};


class Compare{
    public:
    bool operator()(const FreqNode* a, const FreqNode* b) const{
        return *a < *b;
    }
};

template<typename T>
class LFUCache : public Cache<T> {
    private:
        std::unordered_map<int,T> cache_map;
        std::unordered_map<int,FreqNode*> freq_map;
        std::vector<FreqNode*> freq_queue;

        
        
    public:
        LFUCache(int capacity);
        T get(int key) override;
        void put(int key, const T& value) override;
};

template<typename T>
LFUCache<T>::LFUCache(int capacity) : Cache<T>(capacity) {}


template<typename T>
T LFUCache<T>::get(int key) {
    if (!cache_map.count(key)) {
        return T(-1);
    }
    else{
        T value = cache_map[key];
        freq_map[key]->freq++;
        return value;
    }
}

template<typename T>
void LFUCache<T>::put(int key, const T& value){
    if(this->cache_map.count(key)){
        this->cache_map[key] = value;
        this->freq_map[key]->freq++;
        return;
    }
    else{
        if(this->size >= this->capacity){
            std::sort(freq_queue.begin(), freq_queue.end(), Compare());
            FreqNode *node = freq_queue.front();
            this->freq_queue.erase(this->freq_queue.begin());
            this->cache_map.erase(node->key);
            this->freq_map.erase(node->key);
            this->size--;
        }
        FreqNode *node = new FreqNode(key, 1);
        this->cache_map[key] = value;
        this->freq_map[key] = node;
        this->freq_queue.push_back(node);
        this->size++;
    }
};

#endif

