#pragma once
#include "search_tree.hpp"

namespace Container
{
template<typename KeyT = int, class Cmp = std::less<KeyT>>
class Set final : public detail::RBSearchTree<KeyT, Cmp>
{
    using base = detail::RBSearchTree<KeyT, Cmp>;
    using typename base::node_ptr;
    using typename base::node_type;
    using typename base::key_type;
    using typename base::Colors;
public:
    using typename base::Iterator;
    using typename base::ConstIterator;

private:
    using base::Null_;
    using base::root_;
    using base::size_;

public:
    Set(): base::RBSearchTree() {}
    
    template<std::input_iterator InpIt>
    Set(InpIt first, InpIt last): base::RBSearchTree(first, last) {}

    Set(std::initializer_list<key_type> initlist): base::RBSearchTree(initlist) {}

    using base::size;
    using base::empty;
    using base::maximum;
    using base::minimum;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::find;
    using base::insert;
    using base::erase;
    using base::lower_bound;
    using base::upper_bound;
    using base::debug_graph_dump;
};

} // namespace Container