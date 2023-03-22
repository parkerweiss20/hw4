#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <vector>
#include <set>
#endif

#include "equal-paths.h"

using namespace std;


// You may add any prototypes of helper functions here
void dfs(Node* node, unsigned int depth, vector<unsigned int>& depths) {
    if (!node->left && !node->right) {
        depths.push_back(depth);
    }
    if (node->right) {
        dfs(node->right, depth+1, depths);
    }
    if (node->left) {
        dfs(node->left, depth+1, depths);
    }
}

bool equalPaths(Node * root)
{
    if(!root){
      return true;
    }

    vector<unsigned int> depths;

    dfs(root, 0, depths);

    return (set<unsigned int>(depths.begin(), depths.end()).size() == 1);
}

