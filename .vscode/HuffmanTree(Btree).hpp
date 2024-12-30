#pragma once
#include <iostream>
#include "BinarySortTree.hpp"
// #include "CirDoublyList.hpp"
// #include "Stack.hpp"


template<typename T>
class HuffmanTree:public BinarySortTree<T>{
    public:
        typedef Node<T>* NodeType;
        HuffmanTree(T value[], int n);
        HuffmanTree(){this->root = nullptr;}
        ~HuffmanTree(){};
        void buildHuffmanTree(T sorted_value[], int n,Node<T>* &p);
        void buildHuffmanTree(T sorted_value[], int n)
        {
            buildHuffmanTree(sorted_value, n,this->root);
        }
        // std::string encode(T element,std::string code,Node<T>* p);
        void find(T element,NodeType p,NodeType s[],int index,
            NodeType out[],int &length);
        void  find(T element,NodeType out[],int &length)
        {   
            NodeType s[this->nsize];
            find(element,this->root,s,0,out,length);  
        }
        void find(T element){
            NodeType s[this->nsize];
            NodeType out[this->nsize];
            int length ;
            find(element,this->root,s,0,out,length);
        }
        std::string encode(T element);
        std::string decode(std::string code,int length);
    private:
         int nsize;
};

template<typename T>
HuffmanTree<T>::HuffmanTree(T value[], int n){
    nsize = n*2;
    this->buildHuffmanTree(value, n);
}

template<typename T>
void HuffmanTree<T>::buildHuffmanTree(T sorted_value[], int n,Node<T>* &p){
    // for (int i = 0; i < n; i++)
    // {
    //     std::cout << sorted_value[i] << " ";
    // }
    // std::cout << std::endl;
    
    if(n == 0)
    {
        // std::cout << "Empty array" << std::endl;
        return ;
    }

    if(p == nullptr){
        p = new Node<T>(*(sorted_value), nullptr, nullptr);
        buildHuffmanTree((sorted_value+1), n-1, p);
        return ;
    }

    if(n == 1)
    {
        Node<T>* temp = new Node<T>(*(sorted_value)+p->value, nullptr, nullptr);
        if(p->value >= *(sorted_value))
        {
            temp->left = new Node<T>(*(sorted_value), nullptr, nullptr);
            temp->right = p;
            p = temp;
        }
        else
        {
            temp->right = new Node<T>(*(sorted_value), nullptr, nullptr);
            temp->left = p;
            p = temp;
        }
    }
    else {
        Node<T>* temp = new Node<T>(*(sorted_value)+p->value, nullptr, nullptr);
        if(p->value > *(sorted_value))
        {
            temp->left = new Node<T>(*(sorted_value), nullptr, nullptr);
            temp->right = p;
            p = temp;
        }
        else
        {
            temp->right = new Node<T>(*(sorted_value), nullptr, nullptr);
            temp->left = p;
            p = temp;
        }
    }       
    buildHuffmanTree((sorted_value+1), n-1, p);
}

template<typename T>
void HuffmanTree<T>::find(T element,NodeType p,NodeType s[],int index,
NodeType out[],int &length){
    if(p == nullptr)
    {
        return ;
    }
    if(p->left == nullptr && p->right == nullptr)
    {
        if(p->value == element)
        {
            s[index] = p;
            length = index+1;
            for (int i = index; i >= 0; i--)
            {
                // std::cout << s[i]->value << " ";
                out[i] = s[i];
            }
        }
        return ;
    }
    s[index++] = p;
    find(element,p->left,s,index,out,length);
    find(element,p->right,s,index,out,length); 
}


template<typename T>
std::string HuffmanTree<T>::encode(T element){
    std::string code = "";
    NodeType s[this->nsize];
    NodeType out[this->nsize];
    int length ;
    find(element,this->root,s,0,out,length);
    Node<T>* curr = this->root;
    for (int i = 1; i < length; i++)
    {
        if(curr->left == out[i])
        {
            code += "0";
            curr = curr->left;
        }
        else
        {
            code += "1";
            curr = curr->right;
        }
    }
    return code;
}


template<typename T>
void printHuffmanCode(T value[],int n,std::string code[]){
    HuffmanTree<T> tree(value, n);
    tree.levelOrder();
    std::cout << std::endl;
    for (int i = 0; i < n; i++)
    {
        std::cout << value[i] << " ";
        code[i] = tree.encode(value[i]);
        std::cout << code[i] << std::endl;
    }
}
    
template<typename T>
std::string HuffmanTree<T>::decode(std::string code,int length)
{
    std::string decoded = "";
    for (int i = 0; i < length; i++)
    {
        std::cout << code[i];
    }
    Node<T>* curr = this->root;
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
            decoded += char(curr->value+'0');
            curr = this->root;
        }
    }

    return decoded;
}