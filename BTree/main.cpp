#include <iostream>
#include "tree.h"
#include <Windows.h>
#include <vector>
#include <cassert>
#include <format>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    int t = 2;
    BTree<int> a(t);
    for (int i = 0; i <= 20; i++){
        a.insert(i);
    }
    std::cout << a << '\n';
    for (int i = 1; i <= 19; i++){
        a.remove(i);
    }
    std::cout << a;
    std::shared_ptr<BTreeNode<int>> n = std::make_shared<BTreeNode<int>>(1);
    auto x = std::move(n);

    return 0;
}
