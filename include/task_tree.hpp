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

    void copy_left(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        left_ = new_node;
        new_node->parent_ = this;

        new_node->key_ = other->key_;
        new_node->color_ = other->color_;
        new_node->left_ = Null;
        new_node->right_ = Null;
    }

    void copy_right(node_ptr new_node, node_ptr other, node_ptr Null)
    {
        right_ = new_node;
        new_node->parent_ = this;

        new_node->key_ = other->key_;
        new_node->color_ = other->color_;
        new_node->left_ = Null;
        new_node->right_ = Null;
    }

    void init_data(node_ptr other, node_ptr Null)
    {
        key_   = other->key_;
        color_ = other->color_;

        parent_ = Null;
        left_   = Null;
        right_  = Null;
    }

    void dump(std::fstream& file) const
    {
        file << "Node_" << this << "[fillcolor=";
        if (color_ == Colors::Red)    
            file << "red, fontcolor=black";
        else
            file << "black, color=red";
            
        file << ", label = \"{<_node_>ptr:\\n " << this << "| parent:\\n " << parent_ << "| key: " << key_
        << "| size: " << size_ << "| {<left>left:\\n " << left_
        << "| <right>right:\\n " << right_ << "}}\"];" << std::endl;
    }

private:
    void recalc_size()
    {
        size_ = left_->size_ + right_->size_ + 1;
    }
public:
    void action_after_left_rotate(node_ptr Null)
    {
        right_->recalc_size();
        recalc_size();
    }

    void action_after_right_rotate(node_ptr Null) 
    {
        left_->recalc_size();
        recalc_size();
    }

    void action_before_insert(node_ptr Null)
    {
        for (auto node = parent_; node != Null; node = node->parent_)
            node->size_++;
    }
};
} // namespace detail

template<typename KeyT = int, class Cmp = std::less<KeyT>>
class BoostSet : public RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>
{
    using base = RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>;
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
};
} // namespace Container