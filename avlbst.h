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
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* p, int8_t diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == nullptr){
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }

    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            curr->setValue(new_item.second);   // overwrite
            return;
        }
    }
 
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(n);
        parent->updateBalance(-1);
    } else {
        parent->setRight(n);
        parent->updateBalance(1);
    }
 

    if (parent->getBalance() != 0) {
        insertFix(parent, n);
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == nullptr) {
        return;
    }
 

    if (n->getLeft() != nullptr && n->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(n));
        nodeSwap(n, pred);
    }
 
    
    AVLNode<Key, Value>* p = n->getParent();
    int8_t diff = 0;
    if (p != nullptr) {
        if (p->getLeft() == n) {
            diff = 1;    // left side shrunk
        } else {
            diff = -1;   // right side shrunk
        }
    }
 
    AVLNode<Key, Value>* child = nullptr;
    if (n->getLeft() != nullptr) {
        child = n->getLeft();
    } else {
        child = n->getRight();
    }
 
    if (p == nullptr) {
        this->root_ = child;
    } else if (p->getLeft() == n) {
        p->setLeft(child);
    } else {
        p->setRight(child);
    }
    if (child != nullptr) {
        child->setParent(p);
    }
 

    delete n;
    removeFix(p, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
  AVLNode<Key, Value>* p = n->getParent(); //parent
  AVLNode<Key, Value>* c = n->getLeft();//chld
  AVLNode<Key, Value>* mid = c->getRight();

  n->setLeft(mid);
  if(mid){
    mid->setParent(n);
  }
  c->setRight(n);
  n->setParent(c);

  c->setParent(p);
  if(p == nullptr){
    this->root_ = c;
  } else if(p->getLeft() == n){
    p->setLeft(c);
  } else{
    p->setRight(c);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
  AVLNode<Key, Value>* p = n->getParent(); //parent
  AVLNode<Key, Value>* c = n->getRight();//chld
  AVLNode<Key, Value>* mid = c->getLeft();

  n->setRight(mid);
  if(mid){
    mid->setParent(n);
  }
  c->setLeft(n);
  n->setParent(c);

  c->setParent(p);
  if(p == nullptr){
    this->root_ = c;
  } else if(p->getLeft() == n){
    p->setLeft(c);
  } else{
    p->setRight(c);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff)
{
    if (n == nullptr) {
        return;
    }
 
    // Compute the next level BEFORE any rotation moves things around
    AVLNode<Key, Value>* p = n->getParent();
    int8_t ndiff = 0;
    if (p != nullptr) {
        if (n == p->getLeft()) {
            ndiff = 1;
        } else {
            ndiff = -1;
        }
    }
 
    if (diff == -1) {
        // right side shrank, so n leans left
        if (n->getBalance() + diff == -2) {
            AVLNode<Key, Value>* c = n->getLeft();   // taller child
            if (c->getBalance() == -1) {
                // zig-zig: rotate, subtree got shorter, keep going
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            } else if (c->getBalance() == 0) {
                // zig-zig, but height unchanged after rotation: done
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            } else {
                // zig-zag: c leans right
                AVLNode<Key, Value>* gc = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if (gc->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                } else if (gc->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                } else {
                    n->setBalance(1);
                    c->setBalance(0);
                }
                gc->setBalance(0);
                removeFix(p, ndiff);
            }
        } else if (n->getBalance() + diff == -1) {
            n->setBalance(-1);   // was even, now leans left: height unchanged, done
        } else {
            n->setBalance(0);    // was leaning right, now even: got shorter, keep going
            removeFix(p, ndiff);
        }
    } else {
        // mirror: left side shrank, so n leans right
        if (n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();  // taller child
            if (c->getBalance() == 1) {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            } else if (c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            } else {
                AVLNode<Key, Value>* gc = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (gc->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                } else if (gc->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                } else {
                    n->setBalance(-1);
                    c->setBalance(0);
                }
                gc->setBalance(0);
                removeFix(p, ndiff);
            }
        } else if (n->getBalance() + diff == 1) {
            n->setBalance(1);
        } else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == nullptr) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();
    if (g == nullptr) {
        return;   // p is the root
    }
 
    if (p == g->getLeft()) {
        g->updateBalance(-1);
 
        if (g->getBalance() == 0) {
            return;                       
        } else if (g->getBalance() == -1) {
            insertFix(g, p);             
        } else {                          
            if (n == p->getLeft()) {
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else {
                    p->setBalance(-1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    } else {
        g->updateBalance(1);
 
        if (g->getBalance() == 0) {
            return;
        } else if (g->getBalance() == 1) {
            insertFix(g, p);
        } else {                          
            if (n == p->getRight()) {
                // zig-zig 
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // zig-zag
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else {
                    p->setBalance(1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}


#endif
