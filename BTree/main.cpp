#include <iostream>
#include "tree.h"
#include <vector>
#include <cassert>

int main()
{
    BTree<int> a(2);
    for (int i = 0; i <= 40; i++) a.insert(i);
    for (int i = 40; i >= 3; i--) a.remove(i);
    
    std::shared_ptr<int>c  = std::make_shared<int>(10);
    std::shared_ptr<int>b;
    
    std::cout << b.use_count() << '\n';
    std::vector<int> s = {1,2,312};
    std::vector<int> q = s;
    s.clear();
    assert(q.size() == 3);
    return 0;
}
