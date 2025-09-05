#ifndef TREE_IMPL_TPP
#define TREE_IMPL_TPP
#include <format>

template <typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::find(const T& val)const
{
    std::shared_ptr<BTreeNode<T>> node = root;
    while (node)
    {
        size_t index = node->findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != node->getValsCount();
        if (lower_bound_in_node && node->getValAtIndex(index) == val)
            return { node, index };
        else if (!node->isLeaf())
            node = lower_bound_in_node ? node->getChildAtIndex(index) : node->getChildAtIndex(node->getChildrenCount() - 1);
        else
            return ((index < node->getValsCount() && node->getValAtIndex(index) == val) ? std::make_pair(node, index): std::make_pair(nullptr, 0));
    }
    return std::make_pair(nullptr, 0);
    
}
template <typename T>
void BTree<T>::insert(const T& val)
{
    /*
    Try to insert a value val into the leaf node
    Warning is output, when attempting to insert a value that is already in the tree
    */
    std::shared_ptr<BTreeNode<T>> node = root;
    while (!node->isLeaf())
    {
        size_t index = node->findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != node->getValsCount();
        if (lower_bound_in_node && node->getValAtIndex(index) == val) {
            node->insertVal(val);
            return;
        }
        if (lower_bound_in_node)
            node = node->getChildAtIndex(index);
        else
            node = node->getChildAtIndex(node->getChildrenCount() - 1);
       
    }
    node->insertVal(val);
}
template<typename T>
void BTree<T>::remove(const T &val){
    auto [node, index] = this->find(val);
    if (!node)
        throw std::runtime_error(std::format("Node with key = {} was not found in the tree", val));
    
    if (!node->isLeaf()){
        auto [predecessor, _] = findPredecessor(node, index);
        node->swapWithPredecessor(predecessor, index);
        node = predecessor;
        index = node->findIndexOfVal(val);
    }
    
    node->removeValueByIndex(index);
}

template<typename T>
void BTree<T>::printBTree(std::shared_ptr<BTreeNode<T>> node, std::ostream &o, const std::string& prefix, bool is_last) const{
    o << prefix;
    o << (is_last ? "└── " : "├── ");
    
    
    for (size_t i = 0; i < node->getValsCount(); ++i) {
        
        o << '[';
        for (size_t j = 0; j < node->getEntryCount(i); j++)
            o << (j > 0 ? ", " : "") << node->getValAtIndex(i);
        o << ']';
        if (i < node->getValsCount() - 1)
            o << ", "; 
        
        
    }
    o << '\n';

    for (size_t i = 0; i < node->getChildrenCount(); ++i) {
        bool last_child = (i == node->getChildrenCount() - 1);
        printBTree(node->getChildAtIndex(i), o, prefix + (is_last ? "    " : "│   "), last_child);
    }
}

#endif
