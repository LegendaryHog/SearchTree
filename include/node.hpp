#pragma once

namespace Container
{
namespace detail
{

enum class Colors
{
    Red,
    Black,
};

template<typename KeyT = int>
struct RBNode
{
    using key_type = KeyT;
    using node_ptr = RBNode<KeyT>*; 

    key_type  key_ {};
    Colors color_ = Colors::Red;
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
};

// for sorted search tree
template<typename Node>
Node* find_min(Node* root, const Node* Null) noexcept
{
    auto node = root;
    for (node = root; node->left_ != Null; node = node->left_) {}
    return node;
}

// for sorted search tree
template<typename Node>
Node* find_max(Node* root, const Node* Null) noexcept
{
    auto node = root;
    for (node = root; node->right_ != Null; node = node->right_) {}
    return node;
}

} // namespace detail
} // namespace Container