#pragma once
#include <iostream>
#include <utility>
#include <cmath>
#include <cstdlib>
#include <climits>
#include <cfloat>
#include <ctime>



template<typename K,typename E>
struct skipNode
{
    typedef std::pair<K,E>  pairtype;
    pairtype element;
    skipNode<K,E> **next;
    skipNode(const pairtype& thePair,int size)
    :element(thePair){next=new skipNode<K,E>*[size];}
};

template<typename K,typename E>
class skipList
{
public:
    float cutoff;            //用来确定层数
    int levels;              //当前最大的非空链表
    int dsize;               //每个链表的大小
    int maxLevel;            //允许的最大链表层数
    K tailkey;               //尾节点的key
    skipNode<K,E> *head;    //头指针数组
    skipNode<K,E> *tail;     //尾指针
    skipNode<K,E> **last;    //last[i]表示第i层的最后节点
    skipList(K largeKey,int maxpairs , float prob);
    std::pair<K,E>* find(const K& key) const;
    int level() const;
    skipNode<K,E>*search(const K& key) const;
    void insert(const std::pair<K,E>& thePair);
    void erase(const K& key);
    void print() const;
};

template<typename K,typename E>
skipList<K,E>::skipList(K largeKey,int maxpairs , float prob)
{
    cutoff = prob*RAND_MAX;
    maxLevel =(int)  ceil(logf((float)maxpairs)/logf(1.0/prob))-1;
    levels = 0;
    dsize = 0;
    tailkey = largeKey;

    //生成头节点，尾节点和数组last
    std::pair<K,E> tailPair;
    tailPair.first = tailkey;
    head = new skipNode<K,E>(tailPair,maxLevel+1);
    tail = new skipNode<K,E>(tailPair,0);
    last = new skipNode<K,E>*[maxLevel+1];
    //链表为空时，任意级链表中的头结点都指向尾节点
    for(int i=0;i<=maxLevel;i++)
    {
        head->next[i] = tail;
    }
}

template<typename K,typename E>
std::pair<K,E>* skipList<K,E>::find(const K& key) const
{
   if(key >= tailkey) return nullptr;//key比尾节点大，不存在
   //位置不饿佛热Node是关键字为thekey的节点之前最右的位置
   skipNode<K,E> *beforeNode = head;
   for(int i=levels;i>=0;i--)
   {
       while(beforeNode->next[i]->element.first < key)
       {
           beforeNode = beforeNode->next[i];
       }
   
       if(beforeNode->next[0]->element.first == key)
       {
            return &beforeNode->next[0]->element;
       }
   }
   return nullptr; //无匹配的数对
}

template<typename K,typename E>
int skipList<K,E>::level() const
{
    srand(static_cast<unsigned int>(time(0))); 
    int lev = 0;
    while(rand() <= cutoff)
    {
        lev++;
    }
    return (lev<=maxLevel)?lev:maxLevel;//级的分配
}

template<typename K,typename E>
skipNode<K,E>* skipList<K,E>::search(const K& key) const
{
    //搜索关键字thekey，把每一级链表中要查看的最后一个节点存储在数组last中
    //返回包含关键字thekey的节点
    skipNode<K,E> *beforeNode = head;
    for(int i=levels;i>=0;i--)
    {
        while(beforeNode->next[i]->element.first < key)
        {
            beforeNode = beforeNode->next[i];
        }
        last[i] = beforeNode;//把每一层最后记录的节点记录在last中
    }
    return beforeNode->next[0];
}

template<typename K,typename E>
void skipList<K,E>::insert(const std::pair<K,E>& thePair)
{
    if(thePair.first >= tailkey) 
    {
        // std::stringstream oss;
        // oss << "the key is greater than or equal to the tail key: " << tailkey;
        // throw std::invalid_argument(oss.str());
        std::cout << "the key is greater than or equal to the tail key: " << tailkey << std::endl;
        return;
    }
    //查看关键字为theKey的节点是否存在
    skipNode<K,E>* theNode = search(thePair.first);
    if(theNode->element.first == thePair.first)
    {
        std::cout<<(theNode->element.second)<<std::endl;
        theNode->element.second = thePair.second;
        return; //关键字已经存在，更新其值
    }
    //生成新节点

    int theLevel = level();
    //使级the level为<=leves+1
    if(theLevel > levels)
    {
        theLevel = ++levels;
        last[theLevel] = head;
    }

    //在节点theNode之后插入新节点
    skipNode<K,E>* newNode = new skipNode<K,E>(thePair,theLevel+1);
    for(int i=0;i<=theLevel;i++)
    {
        newNode->next[i] = last[i]->next[i];
        last[i]->next[i] = newNode;//指针共享
    }
    dsize++;
    return;
}

template<typename K,typename E>
void skipList<K,E>::erase(const K& key)
{
    if(key >= tailkey) return; //关键字比尾节点大，不删除
    //搜索关键字为theKey的节点
    skipNode<K,E>* theNode = search(key);
    if(theNode->element.first != key) return; //关键字不存在，不删除
    //删除节点theNode
    for(int i=0;i<=levels&&last[i]->next[i] == theNode;i++)
    {
        last[i]->next[i] = theNode->next[i];
    }
    while(levels>0 && head->next[levels] == tail)
    {
        levels -- ;
        delete theNode;
        dsize--;
    }
}

template<typename K,typename E>
void skipList<K,E>::print() const
{
    
    for(int i=levels;i>=0;i--)
    {
        std::cout << "level " << i << " : ";
        skipNode<K,E> *beforeNode = head;
        while(beforeNode->next[i] != tail)
        {
            std::cout << "<" <<beforeNode->next[i]->element.first << ","<<beforeNode->next[i]->element.second << "> "<<" ";
            beforeNode = beforeNode->next[i];
        }
        std::cout << std::endl;
    }
}


