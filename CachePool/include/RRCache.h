#ifndef RR_CACHE_H
#define RR_CACHE_H
#include <vector>
#include "Cache.h"
#include <ctime>

template<typename T> 
class RRCache : public Cache<T> {
    private:
        std::unordered_map<int,T> cache_map;
        std::vector<int> keys;
    public:
        RRCache(int capacity);        
        T get(int key) override;
        void put(int key, const T& value) override;
    private:
        void removeRandom();
};

template<typename T>
RRCache<T>::RRCache(int capacity) : Cache<T>(capacity) {
    keys.reserve(capacity);
    std::srand(static_cast<unsigned>(std::time(nullptr))); // 初始化随机种子
}

template<typename T>
void RRCache<T>::removeRandom() {
    int index = rand() % keys.size();
    int key = keys[index];
    cache_map.erase(key);
    keys.erase(keys.begin() + index);
}

template<typename T>
T RRCache<T>::get(int key) {
    if (!cache_map.count(key)) {
        return T(-1); // or some sentinel value indicating not found
    }
    return cache_map[key];
}

template<typename T>
void RRCache<T>::put(int key, const T& value) {
    if (this->cache_map.count(key)) {
        cache_map[key] = value;
        return;
    }
    if (this->size >= this->capacity) {
        removeRandom();
        this->size--;
    }
    cache_map[key] = value;
    this->keys.push_back(key);
    this->size++;
}

#endif

