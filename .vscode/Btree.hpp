#pragma once
#include<iostream>
#define MAX_KEYS 4
#define MAX_CHILDEN (MAX_KEYS + 1)

template<typename T>
class BTree;
template<typename T>
class  BNode
{
public:
    int n;//current number of keys
    T *keys;//键值数组
    BNode<T> **child;//子节点数组
    BNode<T>* parent;//父节点
    bool leaf;//判断是否是叶子节点
    BNode(bool isleaf = true)
    {
        n = 0;
        leaf = isleaf;
        child = new BNode<T>*[MAX_CHILDEN];
        for(int i = 0; i < MAX_CHILDEN; i++)
        {
            child[i] = nullptr;
        }
        keys = new T[MAX_KEYS+1];   
        parent = nullptr;
    }
    BNode(T key,bool isleaf = true)
    {
        n = 1;
        leaf = isleaf;
        child =new BNode<T>*[MAX_CHILDEN];
        for(int i = 0; i < MAX_CHILDEN; i++)
        {
            child[i] = nullptr;
        }
        keys =new T[MAX_KEYS+1];
        keys[0] = key;
        parent = nullptr;
    }
    bool isLeaf()
    {
        leaf = true;
        for(int i = 0; i < (this->n+1); ++i)
        {
            if(child[i]!= nullptr)
            {
                leaf = false;
                break;
            }
        }
        return leaf;
    }
    void clear_child()
    {
        for (int i = MAX_KEYS; i >= this->n; i--)
        {
            this->keys[i] = T();
        }
        for (size_t i = n+1; i < MAX_CHILDEN; i++)
        {
            child[i] = nullptr;
        }
    }
    friend class BTree<T>;
};

template<typename T>
class BTree
{
    protected:
        BNode<T>* root;
    public:
        BTree();
        // ~BTree();
        BNode<T>* search(BNode<T>* node, T thekey);
        BNode<T>* search(T thekey)
        {
            return search(this->root,thekey);
        }
        BNode<T>* search_insertPos(BNode<T>* &node, T thekey);
        void insert(BNode<T>* &node,T thekey);
        void insert(T thekey)
        {
            return insert(this->root, thekey);
        }
        BNode<T>* remove(BNode<T>*& node, T thekey);
        BNode<T>* remove(T thekey)
        {   
            return remove(this->root, thekey);
        }
    private: 
        void insert_root(BNode<T>* &insert_node);
        void insert_node(BNode<T>* &node,BNode<T>* insert_node)
};


template<typename T>
BTree<T>::BTree()
{
    root = new BNode<T>();
    // root ->leaf = false;
}

template<typename T>
BNode<T>* BTree<T>::search(BNode<T>* node, T thekey)
{
    if(node == nullptr)
        return nullptr;
    int index =0;
    while(index < node->n&&node->keys[index]<thekey)
    {
        if(node ->keys[index] == thekey)
        {
            return node;
        }
        ++index;
    }
    if(node->isLeaf())
    {
        return nullptr;
    }
    return search(node->child[index],thekey);
}

template<typename T>
BNode<T>* BTree<T>::search_insertPos(BNode<T>* &node, T thekey)
{
    if(node == nullptr)
    {
        return nullptr;
    }
    int index =0;
    while(index < node->n&&node->keys[index]<thekey)
    {
        if(node->keys[index] == thekey)
        {
            return nullptr;
        }
        ++index;
    }
    if(node->isLeaf())
    {
        return node;
    }
    return search_insertPos(node->child[index], thekey);
}

template<typename T>
void BTree<T>::insert_root(BNode<T>* &insert_node)
{
    if(root->n == 0)
    {
        root = insert_node;
        return;
    }
    root->keys[root->n++] = insert_node->keys[0];
    root->child[root->n] = insert_node->child[1];
    int index = root->n-1;
    while(
        index >= 1 && root->keys[index] < root->keys[index-1]
    )
    {
        T temp = root->keys[index];
        root->keys[index] = root->keys[index-1];
        root->keys[index-1] = temp;
        BNode<T>* temp_child = root->child[index+1];
        root->child[index+1] = root->child[index];
        root->child[index] = temp_child;
        index--;
    }
    //需要分裂
    if(root->n > MAX_KEYS) 
    {
        int mid = MAX_KEYS  / 2;
        BNode<T>* newleft= new BNode<T>();
        BNode<T>* newright = new BNode<T>();
        for(int i = 0; i < mid; i++)
        {
            newleft->keys[i] = pos->keys[i];
        }
        for(int i = mid+1; i < pos->n; i++)
        {
            newright->keys[i-mid-1] = pos->keys[i];
        }
        for (int i = 0; i < mid; i++)
        {
            newleft->child[i] = pos->child[i];
        }
        for(int i = mid; i < pos->n; i++)
        {
            newright->child[i-mid] = pos->child[i];
        }
        pos->n = 1;
        pos ->keys[0] = pos->keys[mid];
        pos->child[0] = newleft;
        pos->child[1] = newright;
        newleft->parent = pos;
        newright->parent = pos;
        newleft ->leaf = newleft->isLeaf();
        newright->leaf = newright->isLeaf();
    }
}

template<typename T>
void BTree<T>::insert_node(BNode<T>* &node,BNode<T>* insert_node)
{
    if(node == nullptr)
    {
        node = insert_node;
        return;
    }
    node->keys[node->n++] = insert_node->keys[0];
    node->child[node->n] = insert_node->child[1];
    int index = node->n-1;
    while(
        index >= 1 && node->keys[index] < node->keys[index-1]
    )
    {
        T temp = node->keys[index];
        node->keys[index] = node->keys[index-1];
        node->keys[index-1] = temp;
        BNode<T>* temp_child = node->child[index+1];
        node->child[index+1] = node->child[index];
        node->child[index] = temp_child;
        index--;
    }

    if(node->n > MAX_KEYS) //分裂节点
    {
        int mid = MAX_KEYS / 2;
        BNode<T>* newNode= new BNode<T>();
        newNode->keys[0] = node->keys[mid];
        newNode->child[1] = new BNode<T>();
        newNode->child[1][0] = node->child[mid+1];
        newNode->child[1][1] = node->child[mid+2];
        node->n = mid;
        node->clear_child();
        if (node->parent->parent == nullptr)
        {
            insert_root(newNode);
        }
        
        else{
            insert_node(node->parent, newNode);
        }
    }
}


template <typename T>
void BTree<T>::insert(BNode<T>* &node, T thekey)
{
    //插入都从根节点开始查找可以插入的叶子节点
    if(node == nullptr)
    {
        node = new BNode<T>(thekey);
        return ;
    }
    BNode<T> *pos = search_insertPos(node, thekey);
    if(pos== nullptr)
    {
        return ;
    }
    BNode<T>* newNode = new BNode<T>(thekey);
    if(pos->parent == nullptr)
    {
        insert_root(newNode);
    }
    else{
        insert_node(pos->parent, newNode);
    }
}





