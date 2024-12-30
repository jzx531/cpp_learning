#pragma once
#include <iostream>
#include "arrayQueue.hpp"

template<typename T>
class threadNode {
public:
    threadNode() {
        this->left = nullptr;
        this->right = nullptr;
        this->ltag = 0;
        this->rtag = 0;
    }
    threadNode(T data) {
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
        this->ltag = 0;
        this->rtag = 0;
    }
    T data;
    threadNode* left;
    threadNode* right;
    int ltag;
    int rtag;
};

template<typename T>
class threadBtree {
    public:
        enum {LEFT=0, RIGHT=1};
        threadBtree() {root = nullptr;};
        ~threadBtree() {};
        threadNode<T>*  insert(T data,threadNode<T> *&node); 
        void insert(T data){ insert(data, this->root); }
        bool remove(T data);
        void remove_after_inThreading(T data);
        //inOrderThread(); }
        void levelOrder();
        void inOrder(threadNode<T>* node);
        void inOrder() { inOrder(this->root); }
        void inOrderThread(threadNode<T>* node,threadNode<T> *&front);
        void inOrderThread() { 
            threadNode<T>* front = nullptr;
            inOrderThread(this->root,front); }
        void insert_after_inThreading(T data,threadNode<T>*&node);
        void insert_after_inThreading(T data) { insert_after_inThreading(data,this->root); }
        void preOrderThread(threadNode<T>* node,threadNode<T> *&front);
        void preOrderThread() {
            threadNode<T>* front = nullptr;
            preOrderThread(this->root,front); }
        void postOrderThread(threadNode<T>* node,threadNode<T> *&front);
        void postOrderThread() {
            threadNode<T>* front = nullptr;
            postOrderThread(this->root,front); }
    private:
    threadNode<T>* root;
};

template<typename T>
threadNode<T>* threadBtree<T>::insert(T data,threadNode<T> *&node) {
    if (node == nullptr) {
        node = new threadNode<T>(data);
        return node;
    }
    if(data <(node->data))
    {
        node->left = insert(data, node->left);
    }
    if(data > (node->data))
    {
        node->right = insert(data, node->right);
    }
    return node;
}

template<typename T>
bool threadBtree<T>::remove(T data) {
    if (root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return false;
    }
    if (root->data == data) {
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            root = nullptr;
        }
        else if (root->right && root ->left) {
            threadNode<T>* temp = root->right;
            threadNode<T>* curr = temp->left;
            while(curr->left != nullptr)
            {   
                curr = curr ->left;
            }
            curr ->left = root ->left;
            delete root;
            root = temp;
        } 
        else if (root->left) {
            threadNode<T>* temp = root->left;
            delete root;
            root = temp;
        }
        else {
            threadNode<T>* temp = root->right;
            delete root;
            root = temp;
        }
        return true;
    }
    threadNode<T>* curr = root;
    threadNode<T>* prev = nullptr;
    int dir = LEFT;
    while (curr != nullptr) {
        if (curr->data > data) {
            prev = curr;
            curr = curr->left;
            dir = LEFT;
        }
        if (curr->data < data) {
            prev = curr;
            curr = curr->right;
            dir = RIGHT;
        }
        if (curr->data == data) 
        {
            std::cout<<prev->data<<(dir?"right":"left")<<std::endl;
            if (curr->left == nullptr && curr->right == nullptr) 
            {
                if (dir == LEFT) {
                    prev->left = nullptr;
                }
                else {
                    prev->right = nullptr;
                }
                delete curr;
            }
            else if(curr ->right !=nullptr)
            {
                if(dir  == LEFT){
                    prev->left = curr->right;
                    prev->left->left = curr->left;
                }
                else{
                    prev->right = curr->right;
                    prev->right->left = curr->left;
                }
                delete curr;
            }
            else {
                if(dir == LEFT)
                {
                    prev->left = curr->left;
                }
                else
                {
                    prev->right = curr->left;
                }
                delete curr;
            }   
            return true;        
        }
    }
    return false;
}


template<typename T>
void threadBtree<T>::insert_after_inThreading(T data,threadNode<T> *&node) {
    if (node == nullptr) {
        node = new threadNode<T>(data);
        return ;
    }

    if(data <(node->data) && !node->ltag)
    {
        insert_after_inThreading(data, node->left);
    }
    if(data > (node->data) && !node->rtag)
    {
        insert_after_inThreading(data, node->right);
    }

    if(data <(node->data) && node->ltag)
    {
        threadNode<T>* newleft =new threadNode<T>(data); 
        newleft->left = node->left;
        newleft->ltag = 1;
        newleft->right = node;
        newleft->rtag = 1;
        node ->ltag = 0;
        node ->left = newleft;
    }
    
    if(data > (node->data) && node->rtag)
    {
        threadNode<T>* newright =new threadNode<T>(data); 
        newright->right = node->right;
        newright->rtag = 1;
        newright->left = node;
        newright->ltag = 1;
        node ->rtag = 0;
        node ->right = newright;
    }
}

//参次遍历查看序列化后的线索二叉树
template<typename T>
void threadBtree<T>::levelOrder() {
    if (root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return;
    }
    arrayQueue<threadNode<T>*> q;   
    q.enqueue(root);
    while (!q.isEmpty()) {
            threadNode<T>* curr = q.dequeue();
            std::cout << curr->data << "-"<< curr->ltag
            << "-";
            if(curr->left) std::cout << curr->left->data << "-";
            else std::cout << "-";

            std::cout << curr->rtag << "-";
            if(curr->right) std::cout << curr->right->data ;
            else std::cout << "-";
            std::cout << '\n';
           
            if (!curr->ltag ) {
                q.enqueue(curr->left);
            }
            if (!curr->rtag ) {
                q.enqueue(curr->right);
            }
        }
    }
        
template<typename T>
void threadBtree<T>::inOrder(threadNode<T>* node) {
    if (node == nullptr ) {
        // std::cout << "Empty tree" << std::endl;
        return;
    }
    else {
        if(!node ->ltag )inOrder(node->left);
        std::cout<<node->data<<" ";
        if(!node ->rtag )inOrder(node->right);
    }
}

// inOrderThread()是对普通Btree的一次性中序线索化
template<typename T>
void threadBtree<T>::inOrderThread(threadNode<T>* node,threadNode<T> *&front) {
    if(node != nullptr)
    {
        inOrderThread(node->left,front);//处理左子树
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;
        inOrderThread(node->right,front);//处理右子树
    }
    return;
}
// 前序线索化代码和中序线索化代码极为相似，
// 最大的区别就是把连接线索的代码提到了两递归入口前边，
// 这也符合先序递归遍历的框架。
template<typename T>
void threadBtree<T>::preOrderThread(threadNode<T>* node,threadNode<T> *&front) 
{
    if(node != nullptr)
    {
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;

        if(!node ->ltag ) preOrderThread(node->left,front);
        if(!node ->rtag ) preOrderThread(node->right,front);
    }
}
// 后序线索化代码和中序线索化代码极为相似，
// 最大的区别就是把连接线索的代码提到了两递归入口后边，
// 这也符合后序递归遍历的框架。
template<typename T>
void threadBtree<T>::postOrderThread(threadNode<T>* node,threadNode<T> *&front)
{
    if(node != nullptr){
        preOrderThread(node->left,front);
        preOrderThread(node->right,front);
        if(node ->left == nullptr)
        {
            node ->left = front;
            node ->ltag = 1;
        }
        if( node ->right == nullptr)
        {
            node -> rtag = 1;
        }
        if(front != nullptr&& front -> rtag )
        {
            front->right = node;
        }
        front = node;
    }
    return;
}