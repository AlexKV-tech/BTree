#ifndef TREE_IMPL_TPP
#define TREE_IMPL_TPP
#include <format>

template <typename T>
BTree<T>::BTree(size_t t)
    : t{t}, root{std::make_shared<BTreeNode<T>>(t)}
{
}
template <typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::find(const T& val)const
{
    std::shared_ptr<BTreeNode<T>> node = root;
    while (!node->isLeaf())
    {
        size_t index = node->findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != node->getValsCount();
        if (lower_bound_in_node && node->getValAtIndex(index) == val)
        {
            return {node, index};
        }
        else
        {
            node = lower_bound_in_node ? node->getChildAtIndex(index): node->getChildAtIndex(node->getChildrenCount() - 1);
        }
    }
    size_t index = node->findLowerBoundIndexOfVal(val);
    if (index == node->getValsCount() || node->getValAtIndex(index) != val)
    {
        return {nullptr, 0};
    }
    return {node, index};
}
template <typename T>
void BTree<T>::insert(const T& val)
{
    std::shared_ptr<BTreeNode<T>> node = root;
    while (!node->isLeaf())
    {
        
        size_t index = node->findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != node->getValsCount();
        if (lower_bound_in_node && node->getValAtIndex(index) == val) throw std::runtime_error(std::format("Value with key = {} is already in the tree", val));
        node = lower_bound_in_node ? node->getChildAtIndex(index):
        node->getChildAtIndex(node->getChildrenCount() - 1);
    }
    node->insertVal(val);
}
template<typename T>
void BTree<T>::remove(const T &val){
    auto [node, index] = this->find(val);
    if (!node){
        throw std::runtime_error(std::format("Node with key = {} was not found in the tree", val));
    }
    if (!node->isLeaf()){
        auto [predecessor, _] = findPredecessor(node, index);
        node->swapWithPredecessor(predecessor, index);
        node = predecessor;
        index = node->findIndexOfVal(val); // recalculating index of val -> another vector
    }
    
    node->removeValueByIndex(index);
}

template<typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::findPredecessor(const std::shared_ptr<BTreeNode<T>> &node, size_t index)const{
    std::shared_ptr<BTreeNode<T>> predecessor = node->getChildAtIndex(index);
    while (!predecessor->isLeaf()){
        predecessor = predecessor->getChildAtIndex(predecessor->getChildrenCount() - 1);
    }
    return {predecessor, predecessor->getValsCount()};
}
template<typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::findSuccessor(const std::shared_ptr<BTreeNode<T>> &node, size_t index)const{
    std::shared_ptr<BTreeNode<T>> node_ptr = node.lock();
    std::shared_ptr<BTreeNode<T>> successor = node_ptr->getChildren()[index + 1];
    while (!successor->isLeaf()){
        successor = successor->getChildren().front();
    }
    return {successor, 0};
}
template<typename T>
void BTree<T>::printBTree(std::shared_ptr<BTreeNode<T>> node, std::ostream &o, const std::string& prefix, bool is_last) const{
    o << prefix;
    o << (is_last ? "└── " : "├── ");
    
    
    for (size_t i = 0; i < node->getValsCount(); ++i) {
        o << node->getValAtIndex(i);
        if (i < node->getValsCount() - 1) o << ", ";
    }
    o << '\n';

    for (size_t i = 0; i < node->getChildrenCount(); ++i) {
        bool last_child = (i == node->getChildrenCount() - 1);
        printBTree(node->getChildAtIndex(i), o, prefix + (is_last ? "    " : "│   "), last_child);
    }
}
template <typename T>
inline std::ostream &operator<<(std::ostream &o, const BTree<T> &t){
    t.printBTree(t.root, o);
    return o;
}

#endif
