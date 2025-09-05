#ifndef TREE_H
#define TREE_H
#include "node.h"
#include <format>
#include <string>

template <typename T>
class BTree
{
    size_t t;
    std::shared_ptr<BTreeNode<T>> root;
    static constexpr int MinDegree = 2;

public:
    ~BTree() = default;
    BTree(BTree&& other) = delete;
    BTree& operator=(BTree&& other) = delete;
    BTree(const BTree& other) = delete;
    BTree& operator=(const BTree& other) = delete;
    explicit BTree(size_t t) : t(t), root(std::make_shared<BTreeNode<T>>(t)) 
    {
        if (t < MinDegree)
            throw std::length_error(std::format("t value must be at least {}", MinDegree));
    }
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> find(const T& val)const;
    void remove(const T &val);
    void insert(const T& val);
    friend std::ostream& operator<<(std::ostream& o, const BTree<T>& t) 
    {
        t.printBTree(t.root, o);
        return o;
    }
    
private:
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> findPredecessor(const std::shared_ptr<BTreeNode<T>>& node, size_t index) const 
    {
        std::shared_ptr<BTreeNode<T>> predecessor = node->getChildAtIndex(index);
        while (!predecessor->isLeaf())
            predecessor = predecessor->getChildAtIndex(predecessor->getChildrenCount() - 1);

        return { predecessor, predecessor->getValsCount() };
    }
    std::pair<std::shared_ptr<BTreeNode<T>>, size_t> findSuccessor(const std::shared_ptr<BTreeNode<T>>& node, size_t index) const 
    {
        std::shared_ptr<BTreeNode<T>> node_ptr = node.lock();
        std::shared_ptr<BTreeNode<T>> successor = node_ptr->getChildren()[index + 1];
        while (!successor->isLeaf())
            successor = successor->getChildren().front();

        return { successor, 0 };
    }
    void printBTree(std::shared_ptr<BTreeNode<T>> node, std::ostream& o, const std::string& prefix = "", bool is_last = true) const;
};

#include "tree_impl.ipp"
#endif
