#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;


protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}



/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key); // TODO
protected:
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int diff);
    void rightRotate(AVLNode<Key, Value>* n);
    void leftRotate(AVLNode<Key, Value>* n);
    AVLNode<Key, Value>* successor(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* node);

    

};


template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    
    if (this->root_ == nullptr)
    {
        this->root_ = node;
        node->setBalance(0);
        return;
    }
    
    node->setBalance(0);
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_);
    
    while (!(curr == nullptr))
    {
        parent = curr;
        if (node->getKey() > curr->getKey()) curr = curr->getRight();
        else if (node->getKey() < curr->getKey()) curr = curr->getLeft();
        else
        {
            curr->setValue(node->getValue());
            return;
        }
    }

    node->setParent(parent);

    if (parent->getKey() < node->getKey()) parent->setRight(node);
    else parent->setLeft(node);

    if (parent->getBalance() == 1 || parent->getBalance() == -1)
    {
        parent->setBalance(0);
    }
    else
    {
        int balanceUpdate = parent->getKey() > node->getKey() ? -1 : 1;
        parent->updateBalance(balanceUpdate);
        insertFix(parent, node);
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node)
{
    if (parent == nullptr) return;
    if (parent->getParent() == nullptr) return;

    AVLNode<Key,Value>* grandparent = parent->getParent(); // grandparent node

    switch (grandparent->getLeft() == parent) // check if RR or LR
    {
        case true: // RR or LR
            grandparent->updateBalance(-1); // grandparent has new child on left side so update balance
            switch (grandparent->getBalance())
            {
                case 0: // don't need to fix
                    return;
                case -1: // great-grandparent may be off balance, so recurse and check
                    insertFix(grandparent, parent);
                    break;
                case -2: // off balance so rotate
                    switch (parent->getBalance())
                    {
                        case -1: // RR rotation
                            rightRotate(grandparent);
                            parent->setBalance(0);
                            grandparent->setBalance(0); // reset balances because node, grandparent, child all have 2 or 0 children
                            break;
                        case 1: // LR rotation
                            leftRotate(parent);
                            rightRotate(grandparent);

                            switch (node->getBalance())
                            {
                                case -1:
                                    grandparent->setBalance(1);
                                    parent->setBalance(0);
                                    
                                    break;
                                case 0:
                                    grandparent->setBalance(0);
                                    parent->setBalance(0);
                                    
                                    break;
                                case 1:
                                    grandparent->setBalance(0);
                                    parent->setBalance(-1);
                                    
                                    break;
                            }
                            node->setBalance(0);
                            break;
                    }
                    break;
            }
            break;
        case false: // LL AND RL rotation, logic is mirrored from one above
            grandparent->updateBalance(1);
            switch (grandparent->getBalance())
            {
                case 0: // don't need to fix
                    return;
                case 1: // great-grandparent may be off balance, so recurse and check
                    insertFix(grandparent, parent);
                    break;
                case 2: // off balance so rotate
                    switch (parent->getBalance())
                    {
                        case 1: // LL rotation
                            leftRotate(grandparent);
                            parent->setBalance(0);
                            grandparent->setBalance(0);
                            
                            break;
                        case -1: // RL rotation
                            rightRotate(parent);
                            leftRotate(grandparent);

                            switch (node->getBalance())
                            {
                                case 1:
                                    grandparent->setBalance(-1);
                                    parent->setBalance(0);
                                    
                                    break;
                                case 0:
                                    grandparent->setBalance(0);
                                    parent->setBalance(0);
                                    
                                    break;
                                case -1:
                                    grandparent->setBalance(0);
                                    parent->setBalance(1);
                                    
                                    break;
                            }
                            node->setBalance(0);
                            break;
                    }
                    break;
            }
            break;
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    
    if (target == NULL)
    {
        return;
    }

    if (target->getLeft() && target->getRight())
    {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(target));
        nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* child = target->getLeft() ? target->getLeft() : target->getRight();
    AVLNode<Key, Value>* parent = target->getParent();

    if (child != NULL)
    {
        child->setParent(parent);
    }

    if (parent == NULL)
    {
        this->root_ = child;
    }
    else
    {
        int difference;
        switch (target == parent->getLeft()) {
            case true:
                parent->setLeft(child);
                difference = 1;
                break;
            case false:
                parent->setRight(child);
                difference = -1;
                break;
        }

        delete target;
        removeFix(parent, difference);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    if (!node) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int ndiff = (parent && node == parent->getLeft()) ? 1 : -1;

    switch (diff) {
        case -1:
            node->updateBalance(diff);

            if (node->getBalance() == -2) {
                AVLNode<Key, Value>* current = node->getLeft();

                switch (current->getBalance()) {
                    case -1: // Case 1a: Zig-zig
                        rightRotate(node);
                        node->setBalance(0);
                        current->setBalance(0);
                        removeFix(parent, ndiff);
                        break;
                    case 0: // Case 1b: Zig-zig
                        rightRotate(node);
                        node->setBalance(-1);
                        current->setBalance(1);
                        break;
                    case 1: // Case 1c: Zig-zag
                        AVLNode<Key, Value>* grandparent = current->getRight();
                        leftRotate(current);
                        rightRotate(node);

                        if (grandparent->getBalance() == 1) {
                            node->setBalance(0);
                            current->setBalance(-1);
                            grandparent->setBalance(0);
                        } else if (grandparent->getBalance() == 0) {
                            node->setBalance(0);
                            current->setBalance(0);
                            grandparent->setBalance(0);
                        } else {
                            node->setBalance(1);
                            current->setBalance(0);
                            grandparent->setBalance(0);
                        }

                        removeFix(parent, ndiff);
                        break;
                }
            } else if (node->getBalance() == -1) {
                return;
            } else {
                removeFix(parent, ndiff);
            }
            break;
        case 1:
            node->updateBalance(diff);

            if (node->getBalance() == 2) {
                AVLNode<Key, Value>* current = node->getRight();

                switch (current->getBalance()) {
                    case 1: // Case 1a: Zig-zig
                        leftRotate(node);
                        node->setBalance(0);
                        current->setBalance(0);
                        removeFix(parent, ndiff);
                        break;
                    case 0: // Case 1b: Zig-zig
                        leftRotate(node);
                        node->setBalance(1);
                        current->setBalance(-1);
                        break;
                    case -1: // Case 1c: Zig-zag
                        AVLNode<Key, Value>* grandparent = current->getLeft();
                        rightRotate(current);
                        leftRotate(node);

                        if (grandparent->getBalance() == -1) {
                            node->setBalance(0);
                            current->setBalance(1);
                            grandparent->setBalance(0);
                        } else if (grandparent->getBalance() == 0) {
                            node->setBalance(0);
                            current->setBalance(0);
                            grandparent->setBalance(0);
                        } else {
                            node->setBalance(-1);
                            current->setBalance(0);
                            grandparent->setBalance(0);
                        }

                        removeFix(parent, ndiff);
                        break;
                }
            } else if (node->getBalance() == 1) {
                return;
            } else {
               removeFix(parent, ndiff);
            }
            break;
    }
}


template <class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* leftChild = n->getLeft();
    AVLNode<Key, Value>* parent = n->getParent();

    n->setLeft(leftChild->getRight());
    if (leftChild->getRight()) {
        leftChild->getRight()->setParent(n);
    }

    leftChild->setRight(n);
    n->setParent(leftChild);

    if (!parent) {
        this->root_ = leftChild;
    } else if (parent->getLeft() == n) {
        parent->setLeft(leftChild);
    } else {
        parent->setRight(leftChild);
    }
    leftChild->setParent(parent);
}

template <class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* rightChild = n->getRight();
    AVLNode<Key, Value>* parent = n->getParent();

    n->setRight(rightChild->getLeft());
    if (rightChild->getLeft()) {
        rightChild->getLeft()->setParent(n);
    }

    rightChild->setLeft(n);
    n->setParent(rightChild);

    if (!parent) {
        this->root_ = rightChild;
    } else if (parent->getLeft() == n) {
        parent->setLeft(rightChild);
    } else {
        parent->setRight(rightChild);
    }
    rightChild->setParent(parent);
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::successor(AVLNode<Key, Value>* node) {
    if (!node) {
        return nullptr;
    }

    // If the right subtree is not null,
    // the successor is the leftmost node in the right subtree
    if (node->getRight() != nullptr) {
        AVLNode<Key, Value>* temp = node->getRight();
        while (temp->getLeft() != nullptr) {
            temp = temp->getLeft();
        }
        return temp;
    }

    // Else it is the lowest ancestor of the node whose left child is also an ancestor of the node.
    AVLNode<Key, Value>* parent = node->getParent();
    while (parent != nullptr && node == parent->getRight()) {
        node = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* node)
{
    if (node == nullptr) return nullptr;

    // If the left subtree is not empty, find the maximum element in the left subtree
    if (node->getLeft() != nullptr)
    {
        AVLNode<Key, Value>* currentNode = node->getLeft();
        while (currentNode->getRight() != nullptr)
        {
            currentNode = currentNode->getRight();
        }
        return currentNode;
    }

    // If the left subtree is empty, the predecessor is the nearest ancestor for which the given node is in the right subtree
    AVLNode<Key, Value>* parent = node->getParent();
    while (parent != nullptr && node == parent->getLeft())
    {
        node = parent;
        parent = node->getParent();
    }
    return parent;
}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
