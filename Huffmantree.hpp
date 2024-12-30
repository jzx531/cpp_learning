#pragma once
#include <iostream>

#include "SeqCirDoublyList.hpp"
#include "arrayQueue.hpp"

template<typename T>
class TriNode
{
    public:
    T data;
    TriNode<T>* left;
    TriNode<T>* right;
    TriNode<T>* parent;
    TriNode(T data, TriNode<T>* left = nullptr, TriNode<T>* right = nullptr, TriNode<T>* parent = nullptr)
    {
        this->data = data;
        this->left = left;
        this->right = right;
        this->parent = parent;
    }
    TriNode()
    {
        this->left = nullptr;
        this->right = nullptr;
        this->parent = nullptr;
    }
    bool operator < (const TriNode<T>& other) const
    {
        return this->data < other.data;
    }
    bool operator >(const TriNode<T>& other) const
    {
        return this->data > other.data;
    }
    bool operator == (const TriNode<T>& other) const
    {
        return this->data == other.data;
    }
    void deepCopy(TriNode<T>& copy,const TriNode<T>& other)
    { 
        copy.data = other.data;
        if(other.left!= nullptr)
        {
            copy.left = new TriNode<T>();
            copy.left->parent = &copy;
            copy.left->deepCopy(*copy.left, *other.left);
        }
        if(other.right!= nullptr)
        {
            copy.right = new TriNode<T>();
            copy.right->parent = &copy;
            copy.right->deepCopy(*copy.right, *other.right);
        }
    }
    TriNode(const TriNode<T>& other)
    {
        this -> left = nullptr;
        this -> right = nullptr;
        this -> parent = nullptr;
        this->deepCopy(*this, other);
    }
};

template<typename T>
class Huffmantree 
{
    protected:
        TriNode<T>* root;
    public:
        Huffmantree(){this->root = nullptr;}
        Huffmantree(T array[],int n);
        void BuildHuffmanTree(T array[], int n);
        void levelOrder();
        std::string encode(T num);
        void find(T num,TriNode<T>*p,TriNode<T>* &goal);
        TriNode<T>* find(T num)
        {
            TriNode<T>* goal = nullptr;
            find(num,this->root,goal);
            return goal;
        }
        std::string decode(std::string code,int length);

};

template<typename T>
Huffmantree<T>::Huffmantree(T array[], int n)
{
    this->root = nullptr;
    this->BuildHuffmanTree(array, n);
}

template<typename T>
void Huffmantree<T>::BuildHuffmanTree(T array[], int n)
{
    SeqCirDoublyList<TriNode<T>> Q;
    for(int i=0;i<n;i++)
    {   
        Q.insert(TriNode<T>(array[i]));
    }

    while ( Q.count() > 1 )
    {
        TriNode<T> v1 = Q.pop_head();
        TriNode<T> v2 = Q.pop_head();
        TriNode<T> parent = TriNode<T>(v1.data + v2.data);
        parent.left = &v1;
        parent.right = &v2;
        v1.parent = &parent;
        v2.parent = &parent;
        this -> root = new TriNode<T>(parent);
        Q.insert(parent);
    }
}

template<typename T>
void Huffmantree<T>::levelOrder(){
if (root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return;
    }
    arrayQueue<TriNode<T>*> q;   
    q.enqueue(root);
    while (!q.isEmpty()) {
            TriNode<T>* curr = q.dequeue();
            std::cout << curr->data << "-";
            if(curr->left) std::cout << curr->left->data << "-";
            else std::cout << "-";
            if(curr->right) std::cout << curr->right->data ;
            else std::cout << "-";
            std::cout << " "<<"parent:";
            if(curr->parent!= nullptr){
                std::cout<<curr->parent->data;
            }
            std::cout << '\n';
           
            if (curr->left ) {
                q.enqueue(curr->left);
            }
            if (curr ->right ) {
                q.enqueue(curr->right);
            }
        }
}

template<typename T>
void Huffmantree<T>::find(T num,TriNode<T>*p,TriNode<T>* &goal)
{
    if(p == nullptr)
    {
        return ;
    }
    if(p->left == nullptr && p->right == nullptr)
    {
        if(p->data == num)
        {
            goal = p;
        }
        return ;
    }
    find(num,p->left,goal);
    find(num,p->right,goal); 
}

template<typename T>
std::string Huffmantree<T>::encode(T num)
{
    std::string code = "";
    TriNode<T>* curr = this->root;
    TriNode<T>* goal = find(num);
    std::cout << "goal:" << goal->data << std::endl;
    while(goal->parent!= nullptr)
    {
        if(goal->parent->left == goal)
        {
            code += "0";
        }
        else
        {
            code += "1";
        }
        goal = goal->parent;
    }
    return code;
}

template<typename T>
std::string Huffmantree<T>::decode(std::string code,int length)
{
    std::string decoded = "";
    for (int i = 0; i < length; i++)
    {
        std::cout << code[i];
    }
    TriNode<T>* curr = this->root;
    for (int i = 0; i < length; i++)
    {
        if(curr == nullptr)
        {
            curr = this->root;
        }
        if(code[i] == '0')
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
        if(curr->left == nullptr && curr->right == nullptr)
        {
            decoded += char(curr->data+'0');
            curr = this->root;
        }
    }

    return decoded;
}

template<typename T>
void printHuffmanCode(T value[],int n,std::string code[]){
    Huffmantree<T> tree(value, n);
    tree.levelOrder();
    std::cout << std::endl;
    for (int i = 0; i < n; i++)
    {
        std::cout << value[i] << " ";
        code[i] = tree.encode(value[i]);
        std::cout << code[i] << std::endl;
    }
}