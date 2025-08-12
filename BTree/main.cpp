#include <iostream>
#include "tree.h"
#include <vector>
#include <cassert>
#include <format>

int main()
{
    BTree<int> a(7);
    for (int i = 0; i <= 10000; i++){
        a.insert(i);
    }
    for (int i = 0; i <= 10000; i++){
        a.remove(i);
    }
    return 0;
}
