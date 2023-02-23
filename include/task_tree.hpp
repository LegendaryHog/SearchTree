#include "search_tree.hpp"

namespace Container
{
namespace detail
{
template<typename KeyT>
struct RBSubTreeRoot
{
    using key_type  = KeyT;
    using node_ptr  = RBSubTreeRoot<KeyT>*;
    using size_type = typename std::size_t; 

    key_type  key_ {};
    Colors color_ = Colors::Red;
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;
    size_type size_ = 0;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
};
} // namespace detail

template<typename KeyT = int, class Cmp = std::less<KeyT>>
class BoostSet : public RBSearchTree<KeyT, detail::RBSubTreeRoot<KeyT>, Cmp>
{
    using base = RBSearchTree<KeyT, detail::RBSubTreeRoot<KeyT>, Cmp>;
    using typename base::node_ptr;
    using typename base::node_type;
    using typename base::key_type;
    using typename base::Colors;
    using typename base::Iterator;
    using typename base::ConstIterator;
    
    using base::Null_;
    using base::root_;
    using base::size_;

public:
    BoostSet(): base::RBSearchTree() {}
    
    template<std::input_iterator InpIt>
    BoostSet(InpIt first, InpIt last) {base::insert(first, last);}

    BoostSet(std::initializer_list<key_type> initlist): BoostSet{initlist.begin(), initlist.end()} {}

protected:
    void insert_left(node_ptr this_current, node_ptr other_left) override
    {
        this_current->left_ = new node_type{other_left->key_, other_left->color_, this_current, Null_, Null_, other_left->size_};
    }

    void insert_right(node_ptr this_current, node_ptr other_right) override
    {
        this_current->right_ = new node_type{other_right->key_, other_right->color_, this_current, Null_, Null_, other_right->size_};
    }

    void insert_root(node_ptr other_root) override
    {
        root_ = new node_type{other_root->key_, other_root->color_, Null_, Null_, Null_, other_root->size_};
    }

    void action_after_left_rotate(node_ptr x, node_ptr y) override
    {
        x->size_ = x->left_->size_ + x->right_->size_ + 1;
        y->size_ = y->left_->size_ + y->right_->size_ + 1;
    }

    void action_after_right_rotate(node_ptr x, node_ptr y) override
    {
        x->size_ = x->left_->size_ + x->right_->size_;
        y->size_ = y->left_->size_ + y->right_->size_;
    }
private:
    void increment_parents_size(node_ptr parent) const
    {
        for (;parent != Null_; parent = parent->parent_)
            parent->size_++;
    }

protected:
    void action_before_insert(node_ptr new_node) override
    {
        new_node->size_++;
        increment_parents_size(new_node->parent_);   
    }

    void dump_node(std::fstream& file, node_ptr node) const override
    {
        file << "Node_" << node << "[fillcolor=";
        if (node->color_ == Colors::Red)    
            file << "red, fontcolor=black";
        else
            file << "black, color=red";
            
        file << ", label = \"{<_node_>ptr:\\n " << node << "| parent:\\n " << node->parent_ << "| key: " << node->key_
        << "| size: " << node->size_ << "| {<left>left:\\n " << node->left_
        << "| <right>right:\\n " << node->right_ << "}}\"];" << std::endl;
    }
};
} // namespace Container