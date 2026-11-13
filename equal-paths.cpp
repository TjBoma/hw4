#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathHeight(Node * n);

bool equalPaths(Node * root)
{
    // Add your code below
    if(root == nullptr) return true;
    return pathHeight(root) != -1 ;
    

}

int pathHeight(Node * n){
  if(n->left == nullptr && n->right == nullptr) return 0;
  else if(n->left != nullptr  & n->right == nullptr){
    int h = pathHeight(n->left);
    if (h==-1) return -1;
    return h+1;
  }else if(n->right != nullptr  && n->left == nullptr){
    int h = pathHeight(n->right);
    if (h==-1) return -1;
    return h+1;
  } else {
    int lh = pathHeight(n->left);    
    int rh = pathHeight(n->right);   
    if (lh == -1 || rh == -1) return -1;  
    if (lh != rh) return -1;              
    return lh + 1;                        
}

}