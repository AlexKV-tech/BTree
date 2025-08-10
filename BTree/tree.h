#ifndef TREE_H
#define TREE_H
#include "node.h"
template <typename T>
class BTree
{
    size_t t;
    std::shared_ptr<BTreeNode<T>> root;

public:
    BTree(size_t t);
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> find(const T& val)const;
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> findPredecessor(const std::shared_ptr<BTreeNode<T>> &node, size_t idx)const;
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> findSuccessor(const std::shared_ptr<BTreeNode<T>> &node, size_t idx)const;
    void remove(const T &val);
    void insert(const T& val);
    ~BTree() = default;
};

#include "tree_impl.h"
#endif
