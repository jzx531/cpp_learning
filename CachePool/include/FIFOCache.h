#ifndef FIFOCACHE_H
#define FIFOCACHE_H

#include <iostream>
#include <unordered_map>

template<typename T>
class FIFOCache;


template<typename T>
class CacheNode{
    friend class FIFOCache<T>;
    private:
        int key;
        T value;
        CacheNode<T> *next;
        CacheNode<T> *prev;
        CacheNode(): key(0), value(T()), next(nullptr), prev(nullptr) {}
        CacheNode(int k, T v): key(k), value(v), next(nullptr), prev(nullptr) {}
};

template<typename T>
class FIFOCache : public Cache<T>{
    private:
        CacheNode<T> *head;
        CacheNode<T> *tail;
        std::unordered_map<int, CacheNode<T>*> cache;

    public:
        FIFOCache(int capacity);
        T get(int key) override;
        void put(int key, const T& value) override;
    
    private:
        void addToHead(CacheNode<T> *node);
        CacheNode<T> * removeTail();
        void removeNode(CacheNode<T> *node);
};

template<typename T>
FIFOCache<T>::FIFOCache(int capacity) : Cache<T>(capacity) {
    this->head = new CacheNode<T>();
    this->tail = new CacheNode<T>();
    head->next = tail;
    tail->prev = head;
}

template<typename T>
void FIFOCache<T>::addToHead(CacheNode<T>* node) {
    node->prev = this->head;
    node->next = this->head->next;
    this->head->next->prev = node;
    this->head->next = node;
}

template<typename T>
void FIFOCache<T>::removeNode(CacheNode<T> *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    // delete node;
}

template<typename T>
CacheNode<T> * FIFOCache<T>::removeTail() {
    CacheNode<T> *node = this->tail->prev;
    removeNode(node);
    // delete node;
    return node;
}

template<typename T>
T FIFOCache<T>::get(int key) {
    if (!cache.count(key)) {
        return T(-1);
    }
    CacheNode<T> *node = cache[key];
    return node->value;
}

template<typename T>
void FIFOCache<T>::put(int key, const T& value) {
    if (cache.count(key)) {
        CacheNode<T> *node = cache[key];
        node->value = value;
        removeNode(node);
        addToHead(node);
    }
    else {
        if(this->size >= this->capacity)
        {
            CacheNode<T> *tail = removeTail();
            this->cache.erase(tail->key);
            delete tail;
            this->size--;
        }
        CacheNode<T> *node = new CacheNode<T>(key, value);
        addToHead(node);
        cache[key] = node;
        this->size++;
    }
}
    

#endif // FIFOCACHE_H

