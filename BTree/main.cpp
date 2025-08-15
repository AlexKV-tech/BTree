#include <iostream>
#include "tree.h"

#include <vector>
#include <cassert>
#include <format>

int main()
{
    int t = 3;
    BTree<int> a(t);
    for (int i = 0; i <= 2000; i++){
        a.insert(i);
    }
    std::cout << a << '\n';
    for (int i = 1; i <= 1999; i++){
        a.remove(i);
    }
    std::cout << a;
    return 0;
}
