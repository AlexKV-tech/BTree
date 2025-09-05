#ifndef NODE_H
#define NODE_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <utility>


template <typename T>
class BTreeNode : public std::enable_shared_from_this<BTreeNode<T>>
{
private:
    struct Entry
    {
        T data;
        size_t count;
        Entry() : data(), count(0) {};
        Entry(T data) : data(data), count(1) {};
        void addData()
        {
            count++;
        }
        bool operator<(const Entry& other) const
        {
            return data < other.data;
        }

        bool operator==(const Entry& other) const
        {
            return data == other.data;
        }

        bool operator>(const Entry& other) const
        {
            return data > other.data;
        }

        bool operator>=(const Entry& other) const
        {
            return data >= other.data;
        }
        bool operator<=(const Entry& other) const
        {
            return data <= other.data;
        }
    };
    
    size_t t;
    std::vector<Entry> values;
    std::vector<std::shared_ptr<BTreeNode<T>>> children;
    std::weak_ptr<BTreeNode<T>> parent;
    bool is_root;
    bool is_leaf;

public:
    ~BTreeNode() = default;
    BTreeNode(
        size_t t, std::vector<Entry>&& values,
        std::vector<std::shared_ptr<BTreeNode<T>>>&& children,
        const std::weak_ptr<BTreeNode<T>>& parent, bool is_root,
        bool is_leaf) : t(t),
        values(values),
        children(children),
        parent(parent),
        is_root(is_root),
        is_leaf(is_leaf) {
    }
    BTreeNode(BTreeNode&& other) = delete;
    BTreeNode& operator=(BTreeNode&& other) = delete;
    BTreeNode(const BTreeNode& other) = delete;
    BTreeNode& operator=(const BTreeNode& other) = delete;
    BTreeNode(
        size_t t) :
        t(t),
        values(),
        children(t + 1, nullptr),
        parent(),
        is_root(true),
        is_leaf(true) 
    {
        values.reserve(t);
    }
    bool isLeaf()const 
    {
        return is_leaf;
    }
    void insertVal(const T& val) 
    {
        size_t index = findLowerBoundIndexOfVal(val);
        bool lower_bound_in_node = index != this->getValsCount();

        if (lower_bound_in_node && this->getValAtIndex(index) == val) 
        {
            std::cerr << std::format("WARNING: Value with key = {} is already in the tree", val) << '\n';
            this->increaseEntryCount(index);
            
        }
        else 
        {
            values.insert(values.begin() + index, Entry(val));
        }

        if (values.size() >= 2 * t - 1)
            this->split();
    }
    void fixUnderflow();
    void swapWithPredecessor(std::shared_ptr<BTreeNode<T>>leaf, size_t index) 
    {
        if (!leaf->is_leaf)
            throw std::runtime_error("swapWithLeaf called on non-leaf node");
        if (!checkValuesBounds(index))
            throw std::out_of_range("Value in leaf index is out of range");

        std::swap(this->values[index], leaf->values.back());
    }
    void split();
    void removeValueByIndex(size_t index) 
    {
        if (!checkValuesBounds(index))
            throw std::out_of_range("Attempt to delete invalid index from values");

        Entry& entry = this->values[index];
        if (entry.count == 1)
            this->values.erase(this->values.begin() + index);
        else
            entry.count--;

        if (this->getValsCount() < t - 1)
            this->fixUnderflow();
    }
    size_t findLowerBoundIndexOfVal(const T& val)const 
    {
        auto it = std::lower_bound(this->values.begin(), this->values.end(), val);
        if (it == this->values.end())
            return this->getValsCount();
        return std::distance(this->values.begin(), it);
    }
    size_t findIndexOfVal(const T& val)const 
    {
        auto index = this->findLowerBoundIndexOfVal(val);
        if (index == this->getValsCount() || this->values[index].data != val)
            throw std::runtime_error("Value not found");

        return index;
    }
    const T& getValAtIndex(size_t index)const 
    {
        if (!checkValuesBounds(index))
            throw std::out_of_range("Attempt to get a value with an invalid index");
        return this->values[index].data;
    }
    const std::shared_ptr<BTreeNode<T>>& getChildAtIndex(size_t index) const 
    {
        if (!checkChildrenBounds(index))
            throw std::out_of_range("Attempt to get a child with an invalid index");

        return this->children[index];
    }
    size_t getChildrenCount()const 
    {
        return this->children.size();
    }
    size_t getValsCount()const 
    {
        return this->values.size();
    }
    void increaseEntryCount(size_t index) 
    {
        if (!checkValuesBounds(index))
            throw std::length_error("Value index is out of range");
        this->values[index].count++;
    }
    size_t getEntryCount(size_t index) const 
    {
        if (!checkValuesBounds(index))
            throw std::length_error("Value index is out of range");
        return this->values[index].count;
    }
private:
    void splitRoot();
    void splitNode();
    static void updateParent(std::vector<std::shared_ptr<BTreeNode<T>>>& children,
        std::weak_ptr<BTreeNode<T>> new_parent) 
    {
        for (std::shared_ptr<BTreeNode<T>> c : children)
            c->parent = new_parent;
    }
    size_t getChildIndex(const std::shared_ptr<BTreeNode<T>>& child) 
    {
        auto it = std::find(this->children.begin(), this->children.end(), child);

        assert(it != this->children.end() && *it == child);
     
        return std::distance(this->children.begin(), it);
    }
    void rotateLeft(std::shared_ptr<BTreeNode<T>>right_sibling, size_t this_index_in_parent_children);
    void rotateRight(std::shared_ptr<BTreeNode<T>>left_sibling, size_t this_index_in_parent_children);
    void mergeWithRight(std::shared_ptr<BTreeNode<T>>right_sibling, size_t this_index_in_parent_children);
    void mergeWithLeft(std::shared_ptr<BTreeNode<T>>left_sibling, size_t this_index_in_parent_children);
    void clearNode() 
    {
        this->values.clear();
        this->children.clear();
    }
    bool checkValuesBounds(size_t index)const 
    {
        return index <= this->getValsCount();
    }
    bool checkChildrenBounds(size_t index)const
    {
        return index <= this->getChildrenCount();
    }
    
    
};
#include "node_impl.ipp"
#endif


