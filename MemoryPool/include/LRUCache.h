#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "Cache.h"

template<typename T>
class LRUCache;

template<typename T>
class DLinkedNode{
   friend class LRUCache<T>;
   private: 
      int key;
      T value;
      DLinkedNode* prev;
      DLinkedNode* next;
      DLinkedNode(): key(0), value(T()), prev(nullptr), next(nullptr) {}
      DLinkedNode(int k, const T& v): key(k), value(v), prev(nullptr), next(nullptr) {}
};

template<typename T> 
class LRUCache : public Cache<T> {
    private:
        std::unordered_map<int, DLinkedNode<T>*> cache_map;
        DLinkedNode<T>* head;
        DLinkedNode<T>* tail;
    public:
        LRUCache(int capacity);        
        T get(int key) override;
        void put(int key, const T& value) override;
    private:
        void addToHead(DLinkedNode<T>* node);
        void removeNode(DLinkedNode<T>* node);
        void moveToHead(DLinkedNode<T>* node);
        DLinkedNode<T>* removeTail();
};


template<typename T>
LRUCache<T>::LRUCache(int capacity) : Cache<T>(capacity) {
    this->head = new DLinkedNode<T>();
    this->tail = new DLinkedNode<T>();
    this->head->next = this->tail;
    this->tail->prev = this->head;
}


template<typename T>
void LRUCache<T>::addToHead(DLinkedNode<T>* node) {
    node->prev = this->head;
    node->next = this->head->next;
    this->head->next->prev = node;
    this->head->next = node;
}

template<typename T>
void LRUCache<T>::removeNode(DLinkedNode<T> *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

template<typename T>
void LRUCache<T>::moveToHead(DLinkedNode<T>* node) {
    removeNode(node);
    addToHead(node);
}

template<typename T>
DLinkedNode<T> *LRUCache<T>::removeTail()
{
    DLinkedNode<T> *node = this->tail->prev;
    removeNode(node);
    return node;
}

template<typename T>
T LRUCache<T>::get(int key)
{
    if(! this->cache_map.count(key)) {
        return T(-1);
    }
    DLinkedNode<T> *node = this->cache_map[key];
    moveToHead(node);
    return node->value;
}

template<typename T>
void LRUCache<T>::put(int key, const T& value)
{
    if(this->cache_map.count(key)){
        DLinkedNode<T> *node = this->cache_map[key];
        node->value = value;
        moveToHead(node);
    }
    else{
        DLinkedNode<T> *node = new DLinkedNode<T>(key, value);
        addToHead(node);
        this->cache_map[key] = node;
        this->size++;
        if(this->size > this->capacity){
            DLinkedNode<T> *removed = removeTail();
            this->cache_map.erase(removed->key);
            delete removed;
            this->size--;
        }
    }
}




#endif



