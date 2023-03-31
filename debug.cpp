#include <iostream>
#include <string>
#include "avlbst.h"


int main()
{
    AVLTree<std::string, std::string> testTree;

    testTree.insert(std::make_pair("blah", "blah"));
    testTree.insert(std::make_pair("bluh", "blug"));


    testTree.remove("supercalifragilisticexpialodocious");


    return 0;
}