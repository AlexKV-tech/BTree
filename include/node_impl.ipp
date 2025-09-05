#ifndef NODE_IMPL_TPP
#define NODE_IMPL_TPP
#include <cassert>
#include "node.h"




template <typename T>
void BTreeNode<T>::splitNode()
{
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {
        size_t mid = this->values.size() / 2;
        
        // decouple the right part(children and values) of the current node and leave the left part intact
        std::vector<std::shared_ptr<BTreeNode<T>>> right_child_children;
        if (!this->is_leaf)
            right_child_children.assign(this->children.begin() + mid + 1, this->children.end()); 
        std::vector<Entry> right_child_vals(this->values.begin() + mid + 1, this->values.end());
        
        // lift the mid to the parent
        size_t this_index_in_parent_children = parent_ptr->getChildIndex(this->shared_from_this());
        parent_ptr->values.insert(parent_ptr->values.begin() + this_index_in_parent_children, Entry(this->values[mid]));

        // remove the right part and the mid from the current node
        this->values.erase(this->values.begin() + mid, this->values.end());
        if (!this->is_leaf)
            this->children.erase(this->children.begin() + mid + 1, this->children.end());
        
        // construct the right sibling of the current node(a new child of the parent of the current node right to the this)
        auto parent_right_child = std::make_shared<BTreeNode<T>>(this->t, std::move(right_child_vals),
                                                                 std::move(right_child_children),
                                                                 this->parent, false, this->is_leaf);
        updateParent(parent_right_child->children, parent_right_child);
        parent_ptr->children.insert(parent_ptr->children.begin() + this_index_in_parent_children + 1,
                                      parent_right_child); // add the right sibling right to the current node in the parent's children
    }
    else
    {
        throw std::runtime_error("splitNode called on an invalid parent");
    }
}
template <typename T>
void BTreeNode<T>::splitRoot()
{
    size_t mid = this->values.size() / 2;
    T mid_val = this->values.at(mid).data;
    
    //prepare the left and right parts of the root
    std::vector<std::shared_ptr<BTreeNode<T>>> left_child_children, right_child_children;
    if (!this->is_leaf)
    {
        left_child_children.assign(this->children.begin(), this->children.begin() + mid + 1);
        right_child_children.assign(this->children.begin() + mid + 1, this->children.end());
    }
    std::vector<Entry> left_child_vals(this->values.begin(), this->values.begin() + mid);
    std::vector<Entry> right_child_vals(this->values.begin() + mid + 1, this->values.end());
    

    this->clearNode();
    
    // construct the left and right children of the root from the parts
    auto new_left_child =
        std::make_shared<BTreeNode<T>>(this->t, std::move(left_child_vals),
                                       std::move(left_child_children),
                                       this->weak_from_this(), false,
                                       this->is_leaf);
    auto new_right_child =
        std::make_shared<BTreeNode<T>>(this->t, std::move(right_child_vals),
                                       std::move(right_child_children),
                                       this->weak_from_this(), false,
                                       this->is_leaf);
    updateParent(new_left_child->children, new_left_child);
    updateParent(new_right_child->children, new_right_child);

   
    this->children = {new_left_child, new_right_child};
    this->values = {Entry(mid_val)}; // leave the middle intact(was removed after calling clearNode())
    this->is_leaf = false; // the root is not a leaf anymore
    // NOTE: this methods leaves the root pointer intact, so the reassignment in the actual Tree class is unnecessary
}



template <typename T>
void BTreeNode<T>::split()
{
    if (this->values.size() < 2 * this->t- 1)
        return;
    
    if (this->is_root)
    {
        this->splitRoot();
    }
    else
    {
        this->splitNode();
        if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
            parent_ptr->split();
        else
            throw std::runtime_error("Split called on node with empty or expired parent");
    }
    
}

template <typename T>
void BTreeNode<T>::fixUnderflow()
{
    if (this->values.size() >= t - 1 || (this->is_root && this->values.size() > 0)) // the node is valid
    {
        return;
    }
    else if (this->is_root)
    {
        this->is_leaf = true; // underflow in root causes merge with child and leaves only the root as the only node in a B-Tree 
        
        if (this->children.empty())
            return;
        // underflow in root can occur with only one child
        assert(this->children.size() == 1);
        std::shared_ptr<BTreeNode<T>> child = this->children.front();

        this->values = child->values;
        this->children = child->children;
        updateParent(this->children, this->weak_from_this());
        this->is_leaf = child->is_leaf;
        child->clearNode();
        
        return ;
    }
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {
        size_t this_index_in_parent_children = parent_ptr->getChildIndex(this->shared_from_this());
        
        
        std::shared_ptr<BTreeNode<T>>right_sibling{nullptr}, left_sibling{nullptr};
        
        if (this_index_in_parent_children < parent_ptr->children.size() - 1)
            right_sibling = parent_ptr->children.at(this_index_in_parent_children + 1);
        
        if (this_index_in_parent_children > 0)
            left_sibling = parent_ptr->children.at(this_index_in_parent_children - 1);
        
        if (left_sibling && right_sibling)
        {
            if (left_sibling->values.size() >= t) 
                this->rotateRight(left_sibling, this_index_in_parent_children); // can borrow from the left sibling
            else if (right_sibling->values.size() >= t) 
                this->rotateLeft(right_sibling, this_index_in_parent_children); // can borrow from the right sibling
            else
                this->mergeWithRight(right_sibling, this_index_in_parent_children); // both siblings don't have enough elements to share
        }
        else if (left_sibling)
        {
            if (left_sibling->values.size() >= t)
                this->rotateRight(left_sibling, this_index_in_parent_children); // can borrow from the left sibling
            else
                this->mergeWithLeft(left_sibling, this_index_in_parent_children); // left sibling don't have enough elements to share
        }
        else if (right_sibling)
        {
            if (right_sibling->values.size() >= t)
                this->rotateLeft(right_sibling, this_index_in_parent_children); // can borrow from the right sibling
            else
                this->mergeWithRight(right_sibling, this_index_in_parent_children); // right sibling don't have enough elements to share
        }
        
        parent_ptr->fixUnderflow();
    }
    else
    {
        throw std::runtime_error("fixUnderflow called on node with empty or expired parent");
    }
}

// Perform clockwise rotation considering the current node a center
template <typename T>
void BTreeNode<T>::rotateRight(std::shared_ptr<BTreeNode<T>>left_sibling, size_t this_index_in_parent_children)
{
    if (this_index_in_parent_children < 1)
        throw std::runtime_error("rotateRight called on most left node");
    
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {
        /* Since in a correct node every node has one more child than values the index of the current node 
        in parent's children is shifted on 1 to the right.
        So, to borrow the separator between the current node(located to the right of the separator) and 
        it's left sibling 1 must be subtracted from the index current node's index in parent's children vector
        */ 
        T parent_val = parent_ptr->values.at(this_index_in_parent_children - 1).data; 
        this->values.insert(std::lower_bound(this->values.begin(), this->values.end(),parent_val), Entry(parent_val)); // borrow the separator from the parent
        
        parent_ptr->values.at(this_index_in_parent_children - 1) = left_sibling->values.back(); // a "donor" element from the left sibling becomes a new separator
        left_sibling->values.pop_back();

        if (!left_sibling->is_leaf)
        {
            this->children.insert(this->children.begin(), left_sibling->children.back()); // attach the child from the "donor" element to the current node

            this->children.front()->parent = this->shared_from_this();
            left_sibling->children.pop_back();
        }
    }
    else
    {
        throw std::runtime_error("rotateRight called on node with empty or expired parent");
    }
}

// Perform counterclockwise rotation considering the current node a center
template <typename T>
void BTreeNode<T>::rotateLeft(std::shared_ptr<BTreeNode<T>>right_sibling, size_t this_index_in_parent_children){
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {


        /*
        Since in a correct node every node has one more child than values 
        the index of the current node in parent's children is shifted on 1 to the right.
        So, to borrow the separator between the current node(located to the left of the separator) 
        and it's right sibling nothing must be subtracted from the index current node's index in parent's children vector
        */ 
        T parent_val = parent_ptr->values.at(this_index_in_parent_children).data; 
        this->values.insert(std::lower_bound(this->values.begin(), this->values.end(),
                                             parent_val), Entry(parent_val)); // borrow the separator from the parent
        
        parent_ptr->values.at(this_index_in_parent_children) = right_sibling->values.front(); // a "donor" element from the right sibling becomes a new separator
        
        right_sibling->values.erase(right_sibling->values.begin());
        if (!right_sibling->is_leaf)
        {
            this->children.push_back(right_sibling->children.front()); // attach the child from the "donor" element to the current node
            this->children.back()->parent = this->shared_from_this();
            right_sibling->children.erase(right_sibling->children.begin());
        }
        
    }
    else
    {
        throw std::runtime_error("rotateLeft called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::mergeWithRight(std::shared_ptr<BTreeNode<T>>right_sibling, size_t index)
{
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {
        this->values.push_back(parent_ptr->values.at(index));
        parent_ptr->values.erase(parent_ptr->values.begin() + index);
        
        std::move(right_sibling->values.begin(), right_sibling->values.end(),
                  std::back_inserter(this->values));
        std::move(right_sibling->children.begin(), right_sibling->children.end(),
                  std::back_inserter(this->children));
        updateParent(this->children, this->weak_from_this());
        
        right_sibling->clearNode();
        
        parent_ptr->children.erase(parent_ptr->children.begin() + index + 1);
    }
    else
    {
        throw std::runtime_error("mergeWithRight called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::mergeWithLeft(std::shared_ptr<BTreeNode<T>>left_sibling, size_t index)
{
    if (index < 1)
        throw std::runtime_error("mergeWithLeft called on most left node");
    
    if (std::shared_ptr<BTreeNode<T>> parent_ptr = this->parent.lock())
    {
        this->values.insert(this->values.begin(), parent_ptr->values.at(index - 1));
        parent_ptr->values.erase(parent_ptr->values.begin() + index - 1);
        
        this->values.insert(this->values.begin(), std::make_move_iterator(left_sibling->values.begin()), std::make_move_iterator(left_sibling->values.end()));
        this->children.insert(this->children.begin(), std::make_move_iterator(left_sibling->children.begin()), std::make_move_iterator(left_sibling->children.end()));
        updateParent(this->children, this->weak_from_this());
        
        left_sibling->clearNode();
        
        parent_ptr->children.erase(parent_ptr->children.begin() + index - 1);
    }
    else
    {
        throw std::runtime_error("mergeWithLeft called on node with empty or expired parent");
    }
}





#endif
