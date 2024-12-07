#pragma once
#include <iostream>
#include "arrayQueue.hpp"


template<typename T>
class Node{
    public:
        // Node(T value){
        //     this->value = value;
        //     this->left = nullptr;
        //     this->right = nullptr;
        // }
        Node()
        {
            this->left = nullptr;
            this->right = nullptr;
        }
        Node(T value, Node<T>* lp=nullptr, Node<T>* rp=nullptr)
        {
            this->value = value;
            this->left = lp;
            this->right = rp;
        }

        Node<T>* left;
        Node<T>* right;
        T value;
};

template<typename T>
class Btree
{
    enum {LEFT=0, RIGHT=1};
    protected:
        Node<T>* root;
    public:
        Btree(){root = nullptr;}
        ~Btree(){}
        Node<T> *insert(Node<T> *&node, T value);
        void insert(T value){ insert(this->root, value); }
        void inorder(Node<T> *node);
        void inorder(){ inorder(this->root); }
        void preorder(Node<T> *node);
        void preorder(){ preorder(this->root); }
        void postorder(Node<T> *node);
        void postorder(){ postorder(this->root); }
        bool remove(T data);
        void levelOrder() ;
};

template<typename T>
Node<T> *Btree<T>::insert(Node<T> *&node, T value)
{
    if (node == nullptr)
    {
        node = new Node<T>(value);
        return node;
    }
    else if (value < node->value)
    {
        node->left = insert(node->left, value);
    }
    else if (value > node->value)
    {
        node->right = insert(node->right, value);
    }
    return node;
}

template<typename T>
void Btree<T>::inorder(Node<T> *node)
{
    if(node == nullptr)
        return;
    inorder(node->left);
    std::cout << node->value << " ";
    inorder(node->right);
}

template<typename T>
void Btree<T>::preorder(Node<T> *node)
{
    if(node == nullptr)
        return;
    std::cout << node->value << " ";
    preorder(node->left);
    preorder(node->right);
}

template<typename T>
void Btree<T> ::postorder(Node<T> *node)
{
    if(node == nullptr)
        return;
    postorder(node->left);
    postorder(node->right);
    std::cout << node->value << " ";
}

template<typename T>
bool Btree<T>::remove(T data){
    if (root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return false;
    }
    if (root->value == data) {
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            root = nullptr;
        }
        else if (root->right && root ->left) {
            Node<T>* temp = root->right;
            Node<T>* curr = temp->left;
            while(curr->left != nullptr)
            {   
                curr = curr ->left;
            }
            curr ->left = root ->left;
            delete root;
            root = temp;
        } 
        else if (root->left) {
            Node<T>* temp = root->left;
            delete root;
            root = temp;
        }
        else {
            Node<T>* temp = root->right;
            delete root;
            root = temp;
        }
        return true;
    }
    Node<T>* curr = root;
    Node<T>* prev = nullptr;
    int dir = LEFT;
    while (curr != nullptr) {
        if (curr->value > data) {
            prev = curr;
            curr = curr->left;
            dir = LEFT;
        }
        if (curr->value < data) {
            prev = curr;
            curr = curr->right;
            dir = RIGHT;
        }
        if (curr->value == data) 
        {
            std::cout<<prev->value<<(dir?"right":"left")<<std::endl;
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

//参次遍历查看序列化后的线索二叉树
template<typename T>
void Btree<T>::levelOrder() {
    if (root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return;
    }
    arrayQueue<Node<T>*> q;   
    q.enqueue(root);
    while (!q.isEmpty()) {
            Node<T>* curr = q.dequeue();
            std::cout << curr->value << "-";
            if(curr->left) std::cout << curr->left->value << "-";
            else std::cout << "-";
            if(curr->right) std::cout << curr->right->value ;
            else std::cout << "-";
            std::cout << '\n';
           
            if (curr->left ) {
                q.enqueue(curr->left);
            }
            if (curr ->right ) {
                q.enqueue(curr->right);
            }
        }
}







