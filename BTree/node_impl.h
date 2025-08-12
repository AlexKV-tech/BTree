#ifndef NODE_IMPL_TPP
#define NODE_IMPL_TPP

/*
 1) Root reassignment
 2) Structure improvement: 1. Parents update in a separate function 2. Values swaps in a separate function(in BTree)
 3) Flags update
 
 */

template <typename T>
BTreeNode<T>::BTreeNode(size_t t):
t{t}, is_root{true},
is_leaf{true}
{
    
}
template <typename T>
BTreeNode<T>::BTreeNode(size_t t, std::vector<T> &&values,
                        std::vector<std::shared_ptr<BTreeNode<T>>> &&children,
                        std::weak_ptr<BTreeNode<T>> parent, bool is_root,
                        bool is_leaf)
    : t{t},
      values{values},
      children{children},
      parent{parent},
      is_root{is_root},
      is_leaf{is_leaf}
{
}
template<typename T>
const T& BTreeNode<T>::getValAtIndex(size_t index)const{
    if (index >= this->values.size()) throw std::out_of_range("Value index is out of range");
    return this->values[index];
}
template <typename T>
size_t BTreeNode<T>::findLowerBoundIndexOfVal(const T &val)const{
    auto it =
    std::lower_bound(this->values.begin(), this->values.end(), val);
    if (it == this->values.end()) return this->values.size();
    return std::distance(this->values.begin(), it);
}
template<typename T>
size_t BTreeNode<T>::findIndexOfVal(const T &val)const{
    auto index = this->findLowerBoundIndexOfVal(val);
    if (index != this->values.size() && this->values[index]) return index;
    else throw std::runtime_error("Value not found");
}

template <typename T>
void BTreeNode<T>::swapWithPredecessor(std::shared_ptr<BTreeNode<T>>leaf, size_t index){
    if (!leaf->is_leaf) throw std::runtime_error("swapWithLeaf called on non-leaf node");
    if (index >= this->values.size()) throw std::out_of_range("Value in leaf index is out of range");
    std::swap(this->values[index], leaf->values.back());
}

template <typename T>
bool BTreeNode<T>::isLeaf()const{
    return is_leaf;
}
template <typename T>
void BTreeNode<T>::insertVal(const T &val) 
{
    auto index = findLowerBoundIndexOfVal(val);
    values.insert(values.begin() + index, val);
    if (values.size() >= 2 * t - 1)
    {
        this->split();
    }
}
template <typename T>
void BTreeNode<T>::updateParent(std::vector<std::shared_ptr<BTreeNode<T>>> &children, std::weak_ptr<BTreeNode<T>> new_parent){
    for (auto &c: children){c->parent = new_parent;}
}

template <typename T>
size_t BTreeNode<T>::getChildIndex(std::shared_ptr<BTreeNode<T>> child){
    auto it =
        std::lower_bound(this->children.begin(),
                         this->children.end(), child);
    if (it == this->children.end() || *it != child) {
        throw std::runtime_error("Node not found in parent's children");
    }
    return std::distance(this->children.begin(), it);
}
template <typename T>
void BTreeNode<T>::splitNode(std::vector<std::shared_ptr<BTreeNode<T>>>&&left_child_children,
               std::vector<std::shared_ptr<BTreeNode<T>>>&&right_child_children,
               std::vector<T> &&left_child_vals,
                             std::vector<T> &&right_child_vals){
    if (auto parent_ptr = this->parent.lock()){
        auto mid = this->values.size() / 2;
        auto index = parent_ptr->getChildIndex(this->shared_from_this());
        parent_ptr->values.insert(parent_ptr->values.begin() + index, this->values[mid]);
        this->values.erase(this->values.begin() + mid, this->values.end());
        if (!this->is_leaf)
            this->children.erase(this->children.begin() + mid + 1, this->children.end());
        
        auto parent_right_child =
            std::make_shared<BTreeNode<T>>(this->t, std::move(right_child_vals),
                                           std::move(right_child_children),
                                           this->parent, false, this->is_leaf);
        updateParent(parent_right_child->children, parent_right_child);
        parent_ptr->children.insert(parent_ptr->children.begin() + index + 1,
                                      parent_right_child);
    }else{
        throw std::runtime_error("splitNode called on empty parent");
    }
    
    
}
template <typename T>
void BTreeNode<T>::splitRoot(std::vector<std::shared_ptr<BTreeNode<T>>>&&left_child_children,
                             std::vector<std::shared_ptr<BTreeNode<T>>>&&right_child_children,
                             std::vector<T> &&left_child_vals,
                             std::vector<T> &&right_child_vals){
    auto mid = this->values.size() / 2;
    T mid_val = this->values.at(mid);
    this->clearNode();
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
    this->values = {mid_val};
    this->is_leaf = false;
}
template<typename T>
const std::shared_ptr<BTreeNode<T>> & BTreeNode<T>::getChildAtIndex(size_t index)const{
    if (index >= this->children.size()) throw std::out_of_range("Child index is out of range");
    return this->children[index];
}
template<typename T>
size_t BTreeNode<T>::getChildrenCount()const{
    return this->children.size();
}
template<typename T>
size_t BTreeNode<T>::getValsCount()const{
    return this->values.size();
}
template <typename T>
void BTreeNode<T>::split()
{
    if (this->values.size() < this->t * 2 - 1)
        return;
    
    auto mid = this->values.size() / 2;
    std::vector<std::shared_ptr<BTreeNode<T>>> left_child_children,right_child_children;
    if (!this->is_leaf){
        left_child_children.assign(this->children.begin(), this->children.begin() + mid + 1);
        right_child_children.assign(this->children.begin() + mid + 1, this->children.end());
    }
    std::vector<T> left_child_vals(this->values.begin(), this->values.begin() + mid);
    std::vector<T> right_child_vals(this->values.begin() + mid + 1, this->values.end());
    if (this->is_root)
    {
        this->splitRoot(std::move(left_child_children),
                        std::move(right_child_children),
                        std::move(left_child_vals),
                        std::move(right_child_vals));
    }
    else
    {
        this->splitNode(std::move(left_child_children),
                        std::move(right_child_children),
                        std::move(left_child_vals),
                        std::move(right_child_vals));
        if (auto parent_ptr = this->parent.lock())
            parent_ptr->split();
        else
            throw std::runtime_error("Split called on node with empty or expired parent");
    }
    
}
template<typename T>
void BTreeNode<T>::removeValueByIndex(size_t index){
    if (index >= this->values.size()) throw std::out_of_range("Attempt to delete invalid index from values");
    this->values.erase(this->values.begin() + index);
    if (this->values.size() < t - 1)
        this->fixUnderflow();

}
template <typename T>
void BTreeNode<T>::fixUnderflow(){
    if (this->values.size() >= t - 1 || (this->is_root && this->values.size() > 0)){
        return;
    }else if (this->is_root){
        this->is_leaf = true;
        if (this->children.empty()){
            return;
        }
        
        auto child = this->children.front();
        auto new_children = child->children;
        this->values = child->values;
        this->children = child->children;
        updateParent(this->children, this->weak_from_this());
        this->is_leaf = child->is_leaf;
        child->clearNode();
        return ;
    }
    if (auto parent_ptr = this->parent.lock()){
        auto index_in_parent_children = parent_ptr->getChildIndex(this->shared_from_this());
        std::shared_ptr<BTreeNode<T>>right_sibling{nullptr}, left_sibling{nullptr};
        if (index_in_parent_children < parent_ptr->children.size() - 1){
            right_sibling = parent_ptr->children.at(index_in_parent_children + 1);
        }
        if (index_in_parent_children > 0){
            left_sibling = parent_ptr->children.at(index_in_parent_children - 1);
        }
        if (left_sibling && right_sibling){
            
            if (left_sibling->values.size() >= t){
                this->rotateRight(left_sibling, index_in_parent_children);
            }else if (right_sibling->values.size() >= t){
                this->rotateLeft(right_sibling, index_in_parent_children);
            }else{
                this->mergeWithRight(right_sibling, index_in_parent_children);
            }
        }else if (left_sibling){
            if (left_sibling->values.size() >= t){
                this->rotateRight(left_sibling, index_in_parent_children);
            }else{
                this->mergeWithLeft(left_sibling, index_in_parent_children);
            }
        }else if (right_sibling){
            if (right_sibling->values.size() >= t){
                this->rotateLeft(right_sibling, index_in_parent_children);
            }else{
                this->mergeWithRight(right_sibling, index_in_parent_children);
            }
        }
        parent_ptr->fixUnderflow();
    }else{
        throw std::runtime_error("fixUnderflow called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::rotateRight(std::shared_ptr<BTreeNode<T>>left_sibling, size_t index_in_parent_children){
    if (auto parent_ptr = this->parent.lock()){
        
        auto parent_val = parent_ptr->values.at(index_in_parent_children - 1);
        this->values.insert(std::lower_bound(this->values.begin(), this->values.end(),parent_val), parent_val);
        parent_ptr->values.at(index_in_parent_children - 1) = left_sibling->values.back();
        left_sibling->values.erase(left_sibling->values.end());
        if (!left_sibling->is_leaf){
            this->children.insert(this->children.begin(), left_sibling->children.back());
            this->children.front()->parent = this->shared_from_this();
            left_sibling->children.pop_back();
        }
        
    }
    else{
        throw std::runtime_error("rotateRight called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::rotateLeft(std::shared_ptr<BTreeNode<T>>right_sibling, size_t index_in_parent_children){
    if (auto parent_ptr = this->parent.lock()){
        
        auto parent_val = parent_ptr->values.at(index_in_parent_children);
        this->values.insert(std::lower_bound(this->values.begin(), this->values.end(),
                                             parent_val), parent_val);
        parent_ptr->values.at(index_in_parent_children) = right_sibling->values.front();
        right_sibling->values.erase(right_sibling->values.begin());
        if (!right_sibling->is_leaf){
            this->children.push_back(right_sibling->children.front());
            this->children.back()->parent = this->shared_from_this();
            right_sibling->children.erase(right_sibling->children.begin());
        }
        
    }else{
        throw std::runtime_error("rotateLeft called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::mergeWithRight(std::shared_ptr<BTreeNode<T>>right_sibling, size_t index){
    if (auto parent_ptr = this->parent.lock()){
        this->values.push_back(parent_ptr->values.at(index));
        parent_ptr->values.erase(parent_ptr->values.begin() + index);
        std::copy(right_sibling->values.begin(), right_sibling->values.end(),
                  std::back_inserter(this->values));
        std::copy(right_sibling->children.begin(), right_sibling->children.end(),
                  std::back_inserter(this->children));
        updateParent(this->children, this->weak_from_this());
        right_sibling->clearNode();
        parent_ptr->children.erase(parent_ptr->children.begin() + index + 1);
    }else{
        throw std::runtime_error("mergeWithRight called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::mergeWithLeft(std::shared_ptr<BTreeNode<T>>left_sibling, size_t index){
    if (auto parent_ptr = this->parent.lock()){
        this->values.push_back(parent_ptr->values.at(index - 1));
        parent_ptr->values.erase(parent_ptr->values.begin() + index - 1);
        this->values.insert(this->values.begin(), left_sibling->values.begin(), left_sibling->values.end());
        this->children.insert(this->children.begin(), left_sibling->children.begin(), left_sibling->children.end());
        updateParent(this->children, this->weak_from_this());
        left_sibling->clearNode();
        parent_ptr->children.erase(parent_ptr->children.begin() + index - 1);
    }else{
        throw std::runtime_error("mergeWithLeft called on node with empty or expired parent");
    }
}
template <typename T>
void BTreeNode<T>::clearNode(){
    this->values.clear();
    this->children.clear();
    
}
#endif
