#pragma once
#include <iterator>
#include "node.hpp"

namespace Container
{
namespace detail
{

template<typename KeyT, typename Node>
class SearchTreeIterator
{
public:
    using iterator_category = typename std::bidirectional_iterator_tag;
    using difference_type   = typename std::ptrdiff_t;
    using value_type        = KeyT;
    using pointer           = KeyT*;
    using reference         = KeyT&;
    using node_ptr          = Node*;
    using const_node_ptr    = const Node*;
private:
    node_ptr node_;
    const_node_ptr Null_;

public:
    SearchTreeIterator(node_ptr node = nullptr, const_node_ptr Null = nullptr)
    :node_ {node}, Null_ {Null}
    {}

    reference operator*() const
    {
        return node_->key_;
    }

    pointer operator->() const
    {
        return &(node_->key_);
    }

    SearchTreeIterator& operator++()
    {
        if (node_->right_ != Null_)
            node_ = detail::find_min(node_->right_, Null_);
        else
        {
            auto parent = node_->parent_;

            while (parent != Null_ && node_->is_right_son())
            {
                node_ = parent;
                parent = parent->parent_;
            }
            node_ = parent;
        }
        return *this;
    }

    SearchTreeIterator operator++(int)
    {
        auto cpy {*this};
        ++(*this);
        return cpy;
    }

    SearchTreeIterator& operator--()
    {
        if (node_ == Null_)
            node_ = Null_->right_;
        else if (node_->left_ != Null_)
            node_ = detail::find_max(node_->left_, Null_);
        else
        {    
            auto parent = node_->parent_;

            while (parent != Null_ && node_->is_left_son())
            {
                node_ = parent;
                parent = parent->parent_;
            }
            node_ = parent;
        }
        return *this;
    }

    SearchTreeIterator operator--(int)
    {
        auto cpy {*this};
        --(*this);
        return cpy;
    }

    node_ptr base() const {return node_;}
}; // class SearchTreeIterator

template<typename KeyT, typename Node>
bool operator==(const SearchTreeIterator<KeyT, Node>& lhs, const SearchTreeIterator<KeyT, Node>& rhs)
{
    return (lhs.base() == rhs.base());
}

template<typename KeyT, typename Node>
bool operator!=(const SearchTreeIterator<KeyT, Node>& lhs, const SearchTreeIterator<KeyT, Node>& rhs)
{
    return (lhs.base() != rhs.base());
}

} // namespace detail

} // namespace Container