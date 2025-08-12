#include <iostream>
#include "tree.h"
#include <vector>
#include <cassert>
#include <format>

int main()
{
    BTree<int> a(4);
    a.insert(0);
    a.insert(1);
    a.insert(2);
    a.insert(3);
    a.insert(4);
    a.insert(5);
    a.insert(6);
    a.insert(7);
    a.insert(8);
    a.insert(9);
    a.insert(10);
    a.insert(11);
    a.insert(12);
    a.insert(13);
    a.insert(14);
    a.insert(15);
    a.insert(16);
    a.insert(17);
    a.insert(18);
    a.insert(19);
    a.insert(20);
    a.insert(21);
    a.insert(22);
    a.insert(23);
    a.insert(24);
    a.insert(25);
    a.insert(26);
    a.insert(27);
    a.insert(28);
    a.insert(29);
    a.insert(30);
    a.insert(31);
    a.insert(32);
    a.insert(33);
    a.insert(34);
    a.insert(35);
    a.insert(36);
    a.insert(37);
    a.insert(38);
    a.insert(39);
    a.insert(40);
    
    
    a.remove(0);
    a.remove(1);
    a.remove(2);
    a.remove(3);
    a.remove(4);
    a.remove(5);
    a.remove(6);
    a.remove(7);
    a.remove(8);
    a.remove(9);
    a.remove(10);
    a.remove(11);
    a.remove(12);
    a.remove(13);
    a.remove(14);
    a.remove(15);
    a.remove(16);
    a.remove(17);
    a.remove(18);
    a.remove(19);
    a.remove(20);
    a.remove(21);
    a.remove(22);
    a.remove(23);
    a.remove(24);
    a.remove(25);
    a.remove(26);
    a.remove(27);
    a.remove(28);
    a.remove(29);
    a.remove(30);
    a.remove(31);
    a.remove(32);
    a.remove(33);
    a.remove(34);
    a.remove(35);
    a.remove(36);
    a.remove(37);
    a.remove(38);
    a.remove(39);
    a.remove(40);
    

    std::weak_ptr<int>b;
    //{
        std::shared_ptr<int>c  = std::make_shared<int>(10);
        
        b = c;
    //}
    std::cout << (!b.lock());
    return 0;
}
