#pragma once
#include "boost_node.hpp"
#include "search_tree.hpp"

namespace Container
{
template<typename KeyT = int, class Cmp = std::less<KeyT>>
class BoostSet final : public detail::RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>
{
    using base = detail::RBSearchTree<KeyT, Cmp, detail::RBSubTreeRoot<KeyT>>;
    using typename base::node_ptr;
    using typename base::node_type;
    using typename base::key_type;
    using typename base::size_type;
    using typename base::Colors;
    using typename base::Iterator;
    using typename base::ConstIterator;
    
    using base::Null_;
    using base::root_;
    using base::size_;

public:
    BoostSet(): base::RBSearchTree() {}
    
    template<std::input_iterator InpIt>
    BoostSet(InpIt first, InpIt last): base::RBSearchTree(first, last) {}

    BoostSet(std::initializer_list<key_type> initlist): base::RBSearchTree(initlist) {}

    using base::size;
    using base::empty;
    using base::maximum;
    using base::minimum;
    using base::find;
    using base::insert;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::lower_bound;
    using base::upper_bound;
    using base::debug_graph_dump;

    const key_type& operator[](size_type index) const noexcept
    {
        node_ptr current = root_;
        while (current->left_->size_ != index)
            if (current->left_ != Null_ && index < current->left_->size_)
                current = current->left_;
            else
            {
                index  -= current->left_->size_ + 1;
                current = current->right_;
            }
        return current->key_;
    }
};
} // namespace Container