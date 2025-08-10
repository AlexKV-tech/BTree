#ifndef NODE_H
#define NODE_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
template <typename T>
class BTree;
template <typename T>

class BTreeNode : public std::enable_shared_from_this<BTreeNode<T>>
{
    size_t t;
    std::vector<T> values;
    std::vector<std::shared_ptr<BTreeNode<T>>> children;
    std::weak_ptr<BTreeNode<T>> parent;
    bool is_root;
    bool is_leaf;

public:
    ~BTreeNode() = default;
    explicit BTreeNode(
        size_t t, std::vector<T> &&values,
        std::vector<std::shared_ptr<BTreeNode<T>>> &&children,
        std::weak_ptr<BTreeNode<T>> parent, bool is_root,
        bool is_leaf);
    BTreeNode(
        size_t t);
    bool isLeaf()const;
    void insertVal(const T &val);
    
    void fixUnderflow();
    void swapWithPredecessor(std::shared_ptr<BTreeNode<T>>leaf, size_t index);
    void split();
    void removeValueByIndex(size_t index);
    size_t findLowerBoundIndexOfVal(const T &val)const;
    size_t findIndexOfVal(const T &val)const;
    const T& getValAtIndex(size_t index)const;
    const std::shared_ptr<BTreeNode<T>> &getChildAtIndex(size_t index)const;
    size_t getChildrenCount()const;
    size_t getValsCount()const;
private:
    void splitRoot(std::vector<std::shared_ptr<BTreeNode<T>>>&&left_child_children,
                                 std::vector<std::shared_ptr<BTreeNode<T>>>&&right_child_children,
                                 std::vector<T> &&left_child_vals,
                                 std::vector<T> &&right_child_vals);
    void splitNode(std::vector<std::shared_ptr<BTreeNode<T>>>&&left_child_children,
                   std::vector<std::shared_ptr<BTreeNode<T>>>&&right_child_children,
                   std::vector<T> &&left_child_vals,
                   std::vector<T> &&right_child_vals);
    static void updateParent(std::vector<std::shared_ptr<BTreeNode<T>>> &children,
                             std::weak_ptr<BTreeNode<T>> new_parent);
    static size_t getChildIndex(std::vector<std::shared_ptr<BTreeNode<T>>> children, std::shared_ptr<BTreeNode<T>> child);
    void rotateLeft(std::shared_ptr<BTreeNode<T>>right_sibling, size_t index);
    void rotateRight(std::shared_ptr<BTreeNode<T>>left_sibling, size_t index);
    void mergeWithRight(std::shared_ptr<BTreeNode<T>>right_sibling, size_t index);
    void mergeWithLeft(std::shared_ptr<BTreeNode<T>>left_sibling, size_t index);
    void clearNode();
    friend class BTree<T>;
    
};
#include "node_impl.h"
#endif
