#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <iomanip>
using namespace std;

template<typename T>
struct AVLNode {
    T key;
    int height;
    AVLNode<T>* left;
    AVLNode<T>* right;
    AVLNode(T k, AVLNode<T>* l = nullptr, AVLNode<T>* r = nullptr) : 
    key(k), height(1), left(l),  right(r) {}
};

template<typename T>
class AVLTree {
private:
    AVLNode<T>* root;
public:
    AVLTree();
    ~AVLTree();
    void insert(T key);
    int max(int a, int b);
    void remove(T key);
    void destroy();
    void print();
    int height();
    void inorder();
    void preorder();
    void postorder();  
    void insert(AVLNode<T>*& node, T key);
    AVLNode<T>* maxValueNode(AVLNode<T>* node);
    AVLNode<T>* minValueNode(AVLNode<T>* node);
private:
    int height(AVLNode<T>* node);
    AVLNode<T>* search(AVLNode<T>* node,T key);
    AVLNode<T>* leftLeftRotation(AVLNode<T>* node);
    AVLNode<T>* leftRightRotation(AVLNode<T>* node);
    AVLNode<T>* rightRightRotation(AVLNode<T>* node);
    AVLNode<T>* rightLeftRotation(AVLNode<T>* node);
    void destroy(AVLNode<T>* &node);
    void inorder(AVLNode<T>* node) const;
    void preorder(AVLNode<T>* node);
    void postorder(AVLNode<T>* node);
    void print(AVLNode<T>* tree, T key, int direction);
    AVLNode<T>* remove(AVLNode<T>* &node, T key);
};

template<typename T>
AVLTree<T>::AVLTree() : root(nullptr) {}

template<typename T>
AVLTree<T>::~AVLTree() {
    destroy(root);
}

template<typename T>
int AVLTree<T>::max(int a, int b) {
    return (a > b)? a : b;
}

template<typename T>
void AVLTree<T>::destroy(AVLNode<T>*& node) {
    if (node == NULL) return;
    if(node->left!= NULL) destroy(node->left);
    if(node->right!= NULL) destroy(node->right);
    delete node;
}
template<typename T>
void AVLTree<T>::destroy() {
    destroy(root);
    root = nullptr;
}

template<typename T>
int AVLTree<T>::height(AVLNode<T>* node) {
    if (node == NULL) return 0;
    return node->height;
}

template<typename T>
int AVLTree<T>::height() {
    return height(root);
}

template<typename T>
void AVLTree<T>::inorder(AVLNode<T>* node) const{
    if (node == NULL) return;
    inorder(node->left);
    cout << node->key << " ";
    inorder(node->right);
}

template<typename T>
void AVLTree<T>::preorder(AVLNode<T>* node) {
    if (node == NULL) return;
    cout << node->key << " ";
    preorder(node->left);
    preorder(node->right);
}

template<typename T>
void AVLTree<T>::postorder(AVLNode<T>* node) {
    if (node == NULL) return;
    postorder(node->left);
    postorder(node->right);
    cout << node->key << " ";
}

template<typename T>
void AVLTree<T>::inorder() {
    inorder(root);
}

template<typename T>
void AVLTree<T>::preorder() {
    preorder(root);
}

template<typename T>
void AVLTree<T>::postorder() {
    postorder(root);
}
template <typename T>
void AVLTree<T>::print(AVLNode<T>* tree, T key, int direction)
{
	if(tree != NULL)
	{
		if(direction==0)	// tree是根节点
			cout << setw(2) << tree->key << " is root" << endl;
		else				// tree是分支节点
			cout << setw(2) << tree->key << " is " << setw(2) << key << "'s "  << setw(12) << (direction==1?"right child" : "left child") << endl;

		print(tree->left, tree->key, -1);
		print(tree->right,tree->key,  1);
	}
}

template <typename T>
void AVLTree<T>::print()
{
	if (root != NULL)
		print(root, root->key, 0);
}

template<typename T>
AVLNode<T>* AVLTree<T>::search(AVLNode<T>* node, T key) {
    if (node == NULL || node->key == key) {
        return node;
    }
    if (key < node->key) {
        return search(node->left, key);
    }
    else {
        return search(node->right, key);
    }
}

template<typename T>
void AVLTree<T>::insert(AVLNode<T>* &node, T key) {
    if (node == NULL) {
        // cout << "create new node " << key << endl;
        node = new AVLNode<T>(key);
        return;
    }
    // cout << "insert " << key << endl;
    if (key < node->key) {
        insert(node->left, key);
        if(height(node->left) - height(node->right) == 2) {
            if(key < node->left->key) {
                node = leftLeftRotation(node);
            }
            else {
                node = leftRightRotation(node);
            }
        }
    }
    else if (key > node->key) {
        insert(node->right, key);
        if(height(node->right) - height(node->left) == 2) {
            if(key > node->right->key) {
                node = rightRightRotation(node);
            }
            else {
                node = rightLeftRotation(node);
            }
        }
    }
    node->height = max(height(node->left), height(node->right)) + 1;
}

template<typename T>
void AVLTree<T>::insert(T key) {
    insert(root, key);
}


template<typename T>
AVLNode<T>* AVLTree<T>::leftLeftRotation(AVLNode<T>* node) {
    AVLNode<T>* temp = node->left;
    node->left = temp->right;
    temp->right = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    temp->height = max(height(temp->left), height(temp->right)) + 1;
    return temp;
}

template<typename T>
AVLNode<T>* AVLTree<T>::leftRightRotation(AVLNode<T>* node) {
    node->left = rightRightRotation(node->left);
    return leftLeftRotation(node);
}

template<typename T>
AVLNode<T>* AVLTree<T>::rightRightRotation(AVLNode<T>* node) {
    AVLNode<T>* temp = node->right;
    node->right = temp->left;
    temp->left = node;
    node->height = max(height(node->left), height(node->right)) + 1;
    temp->height = max(height(temp->left), height(temp->right)) + 1;
    return temp;
}

template<typename T>
AVLNode<T>* AVLTree<T>::rightLeftRotation(AVLNode<T>* node) {
    node->right = leftLeftRotation(node->right);
    return rightRightRotation(node);
}

/*
template<typename T>
AVLNode<T>* AVLTree<T>::remove(AVLNode<T>* &node, T key) {
    if (node == NULL) {
        return node;
    }
    if (key < node->key) {
        node->left = remove(node->left, key);
        if(height(node->left) - height(node->right) == 2) {
            if(height(node->left->left) >= height(node->left->right)) {
                node = leftLeftRotation(node);
            }
            else {
                node = leftRightRotation(node);
            }
        }
    }
    else if (key > node->key) {
        node->right = remove(node->right, key);
        if(height(node->right) - height(node->left) == 2) {
            if(height(node->right->right) >= height(node->right->left)) {
                node = rightRightRotation(node);
            }
            else {
                node = rightLeftRotation(node);
            }
        }
    }
    else {
        if (node->left == NULL || node->right == NULL) {
            AVLNode<T>* temp = node->left ? node->left  : node->right;
            if (temp == NULL) {
                temp = node;
                node = NULL;
            }
            else {
                *node = *temp;
            }
            delete temp;
        }
        else {
            AVLNode<T>* temp = minValueNode(node->right);
            node->key = temp->key;
            node->right = remove(node->right, temp->key);
            if(height(node->right) - height(node->left) == 2) {
                if(height(node->right->right) >= height(node->right->left)) {
                    node = rightRightRotation(node);
                }
                else {
                    node = rightLeftRotation(node);
                }
            }
        }
    }   
    return node;
}
*/
template<typename T>
AVLNode<T>* AVLTree<T>::remove(AVLNode<T>* &node, T key) {
    if(node == NULL) return node;
    if(key < node->key)
    {
        node->left = remove(node->left, key);
        if(height(node->right) - height(node->left) == 2)//若移除左子树节点导致右子树更高
        {
            AVLNode<T>* temp = node->right;
            if(height(temp->right) > height(temp->left))
            {
                node = rightRightRotation(node);  //右-右
            }
            else
            {
                node = rightLeftRotation(node);  //右-左
            }
        }
    }
    else if(key > node->key)
    {
        node->right = remove(node->right, key);
        if(height(node->left) - height(node->right) == 2)
        {
            AVLNode<T>* temp = node->left;
            if(height(temp->left) > height(temp->right))
            {
                node = leftLeftRotation(node);  //左-左
            }
            else
            {
                node = leftRightRotation(node);  //左-右
            }
        }
    }
    else//找到节点
    {   
        if(node->left != NULL || node->right != NULL)//若节点左右节点均不为空
        {
            if(height(node->left) > height(node->right))
            {
                AVLNode<T>* max=maxValueNode(node->left);
                node->key = max->key;
                node->left = remove(node->left, max->key);
            }
            else 
            {
                AVLNode<T>* min=minValueNode(node->right);
                node->key = min->key;    
                node->right = remove(node->right, min->key);
            }    
        }
        else//若节点左右节点均为空
        {
            AVLNode<T>* temp = node;
            node = (node->left != NULL) ? node->left  : node->right;
            delete temp;
        }

    }
    return node;    

}

            
       



template<typename T>
void AVLTree<T>::remove(T key) {
AVLNode<T>* temp = search(root, key);
if(temp!= NULL){
    root=remove(root, key);
    }
    else{
        cout << "key not found" << endl;
        return; 
        }
}

template<typename T>
AVLNode<T>* AVLTree<T>::minValueNode(AVLNode<T>* node) {
    AVLNode<T>* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

template<typename T>
AVLNode<T>* AVLTree<T>::maxValueNode(AVLNode<T>* node) {    
    AVLNode<T>* current = node;
    while (current->right != NULL) {
        current = current->right;
    }
    return current;
}
    
#endif // AVL_H