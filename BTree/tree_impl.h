#ifndef TREE_IMPL_TPP
#define TREE_IMPL_TPP

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
        auto index = node->findLowerBoundIndexOfVal(val);
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
    auto index = node->findLowerBoundIndexOfVal(val);
    if (index == node->getValsCount() || node->getValAtIndex(index) != val)
    {
        return {std::shared_ptr<BTreeNode<T>>(), 0};
    }
    return {node, index};
}
template <typename T>
void BTree<T>::insert(const T& val)
{
    std::shared_ptr<BTreeNode<T>> node = root;
    while (!node->isLeaf())
    {
        
        auto index = node->findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != node->getValsCount();
        if (lower_bound_in_node && node->getValAtIndex(index) == val) throw std::runtime_error("Value is already in BTree");
        node = lower_bound_in_node ? node->getChildAtIndex(index):
        node->getChildAtIndex(node->getChildrenCount() - 1);
    }
    node->insertVal(val);
}
template<typename T>
void BTree<T>::remove(const T &val){
    auto [node, index] = this->find(val);
    if (!node->isLeaf()){
        auto [predecessor, _] = findPredecessor(node, index);
        node->swapWithPredecessor(predecessor, index);
        node = predecessor;
    }
    node->removeValueByIndex(index);
}

template<typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::findPredecessor(const std::shared_ptr<BTreeNode<T>> &node, size_t index)const{
    std::shared_ptr<BTreeNode<T>> predecessor = node->getChildAtIndex(index);
    while (!predecessor->isLeaf()){
        predecessor = predecessor->getChildAtIndex(predecessor->getChildrenCount() - 1);
    }
    return {predecessor, predecessor->values.size()};
}
template<typename T>
std::pair<std::shared_ptr<BTreeNode<T>>, size_t> BTree<T>::findSuccessor(const std::shared_ptr<BTreeNode<T>> &node, size_t index)const{
    auto node_ptr = node.lock();
    std::shared_ptr<BTreeNode<T>> successor = node_ptr->getChildren()[index + 1];
    while (!successor->isLeaf()){
        successor = successor->getChildren().front();
    }
    return {successor, 0};
}
#endif
