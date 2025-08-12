#include <iostream>
#include "tree.h"

#include <vector>
#include <cassert>
#include <format>

int main()
{
    BTree<int> a(2);
    for (int i = 0; i <= 1000; i++){
        a.insert(i);
    }
    //std::cout << a << '\n';
    for (int i = 1001; i >= 1; i--){
        a.remove(i);
    }
    std::cout << a;
    return 0;
}
