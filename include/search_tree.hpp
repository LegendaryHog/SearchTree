#pragma once
#include <functional>
#include <stack>
#include <set>
#include <iterator>
#include <fstream>
#include <string>

namespace Container
{

namespace 
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
    using node_ptr = RBNode*;   

    key_type  key_ {};
    Colors  color_  = Colors::Red;
    node_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;

    bool is_left_son()  const noexcept {return this == parent_->left_;}
    bool is_right_son() const noexcept {return this == parent_->right_;}
};

// for sorted search tree
template<typename KeyT>
RBNode<KeyT>* find_min(RBNode<KeyT>* root, const RBNode<KeyT>* Null)
{
    RBNode<KeyT>* node = root;
    for (node = root; node->left_ != Null; node = node->left_) {}
    return node;
}

// for sorted search tree
template<typename KeyT>
RBNode<KeyT>* find_max(RBNode<KeyT>* root, const RBNode<KeyT>* Null)
{
    RBNode<KeyT>* node = root;
    for (node = root; node->right_ != Null; node = node->right_) {}
    return node;
}

} // namespace anonymous

template<typename KeyT = int, class Cmp = std::less<KeyT>>  
class SearchTree
{
    using node_type      = RBNode<KeyT>;
    using node_ptr       = node_type*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using size_type      = typename std::size_t;

    node_ptr null_init() const
    {
        node_ptr Null = new node_type{{}, Colors::Black};
        Null->left_  = Null;
        Null->right_ = Null;
        return Null;
    }

    node_ptr Null_ = null_init(); // all of nullptr is replaced on null_, for minimalize checking

    node_ptr root_ = Null_;
    node_ptr max_  = Null_;
    node_ptr min_  = Null_;

    size_type size_ = 0;
    Cmp cmp {};

public:
    SearchTree() = default;

private:
    void put_min_max_in_null()
    {
        Null_->left_  = min_;
        Null_->right_ = max_;
    }
public:
    SearchTree(const key_type& key)
    :root_ {new node_type{key, Colors::Black, Null_, Null_, Null_}}, max_ {root_}, min_ {root_}, size_ {1}
    {
        put_min_max_in_null();
    }

    bool empty() const {return (size_ == 0);}

    SearchTree(const SearchTree& other): size_ {other.size_}
    {
        if (empty())
            return;

        root_ = new node_type{other.root_->key_, other.root_->color_, Null_, Null_, Null_};
        auto this_current = root_;
        auto other_current = other.root_;

        while (other_current != other.Null_)
        {
            if (other_current->left_ != other.Null_ && this_current->left_ == Null_)
            {
                this_current->left_ = new node_type{other_current->left_->key_, other_current->left_->color_, this_current, Null_, Null_};
                other_current = other_current->left_;
                this_current = this_current->left_;
            }
            else if (other_current->right_ != other.Null_ && this_current->right == Null_)
            {
                this_current->right_ = new node_type{other_current->left_->key_, other_current->left_->color_, this_current, Null_, Null_};
                other_current = other_current->right_;
                this_current = this_current->right_;
            }
            else
            {
                other_current = other_current->parent_;
                this_current = this_current->parent_;
            }
        }

        min_ = find_min(root_, Null_);
        max_ = find_max(root_, Null_);
        put_min_max_in_null();
    }

// recursive copy ctor
/*
    SearchTree(const SearchTree& other): size_ {other.size_}
    {
        root_ = recursive_copy(other.root_, Null_, other.Null_);
        min_  = find_min();
        max_  = find_max(); 
    }

private:
    node_ptr recursive_copy(node_ptr other_node, node_ptr this_parent, node_ptr other_Null_) const
    {
        if (other_node == other_Null_)
            return Null_;

        node_ptr this_node = new node_type{other_node->key_, other_node->color_, this_parent};

        this_node->left_  = recursive_copy(other_node->left_, this_node, other_Null_);
        this_node->right_ = recursive_copy(other_node->right_, this_node, other_Null_);

        return this_node;
    }
*/
private:
    void swap(SearchTree& other) noexcept
    {
        std::swap(Null_, other.Null_);
        std::swap(root_, other.root_);
        std::swap(min_, other.min_);
        std::swap(max_, other.max_);
        std::swap(size_, other.size_);
    }

public:
    SearchTree(SearchTree&& other) noexcept
    {
        swap(other);
    }

    SearchTree& operator=(const SearchTree& rhs)
    {
        auto rhs_cpy {rhs};
        swap(rhs);
        return *this;
    }

    SearchTree& operator=(SearchTree&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    ~SearchTree()
    {
        node_ptr current = root_;

        while (current != Null_)
        {
            if (current->left_ != Null_)
                current = current->left_;
            else if (current->right_ != Null_)
                current = current->right_;
            else
            {
                if (current->parent_ == Null_)
                {
                    delete root_;
                    break;
                }
                if (current->is_left_son())
                {
                    current = current->parent_;
                    delete current->left_;
                    current->left_ = Null_;
                }
                else
                {
                    current = current->parent_;
                    delete current->right_;
                    current->right_ = Null_;
                }
                size_--;
            }
        }
        delete Null_;
    }

private:
    /*\_________________________________________________
    |*                                                  |
    |*        x                             y           |
    |*       / \                           / \          |
    |*      /   \     left_rotate(x)      /   \         |
    |*     l     y    ------------->     x     yr       |
    |*          / \                     / \             |
    |*         /   \                   /   \            |
    |*       yl     yr                l     yl          |
    |*__________________________________________________|
    \*/
    void left_rotate(node_ptr x_node)
    {
        // declare y as right son of x
        node_ptr y_node = x_node->right_;
        // new right son of x is yl 
        x_node->right_ = y_node->left_;

        // if yl exist (not nullptr)
        // replace parent of yl from y to x
        if (y_node->left_ != Null_)      
            y_node->left_->parent_ = x_node;

        // parent of y will be parent of x
        y_node->parent_ = x_node->parent_;

        // if x were root,then y is new root
        if (x_node == root_)
            root_ = y_node;
        // if x were left son of parent,
        // then y is new left son
        else if (x_node == x_node->parent_->left_)
            x_node->parent_->left_ = y_node;
        // else x were right son of parent,
        // then y is new right son
        else
            x_node->parent_->right_ = y_node;

        // x is y left son now
        y_node->left_ = x_node;
        // parent of x is y now
        x_node->parent_ = y_node;
    }

    /*\_________________________________________________
    |*                                                  |
    |*        x                             y           |
    |*       / \                           / \          |
    |*      /   \     right_rotate(x)     /   \         |
    |*     y     r    -------------->    yl    x        |
    |*    / \                                 / \       |
    |*   /   \                               /   \      |
    |* yl     yr                           yr      r    |
    |* _________________________________________________|
    \*/
    void right_rotate(node_ptr x_node)
    {
        node_ptr y_node = x_node->left_;
        x_node->left_ = y_node->right_;

        if (y_node->right_ != Null_)
            y_node->right_->parent_ = x_node;

        y_node->parent_ = x_node->parent_;
        
        if (x_node == root_)
            root_ = y_node;
        else if (x_node == x_node->parent_->left_)
            x_node->parent_->left_  = y_node;
        else
            x_node->parent_->right_ = y_node;

        y_node->right_  = x_node;
        x_node->parent_ = y_node;
    }

public:
    class Iterator
    {
    public:
        using iterator_category = typename std::bidirectional_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = key_type;
        using pointer           = key_type*;
        using reference         = key_type&;
    private:
        node_ptr node_;
        const_node_ptr Null_;

    public:
        Iterator(node_ptr node = nullptr, const_node_ptr Null = nullptr)
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

        Iterator& operator++()
        {
            if (node_->right_ != Null_)
                node_ = find_min(node_->right_, Null_);
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

        Iterator operator++(int)
        {
            auto cpy {*this};
            ++(*this);
            return cpy;
        }

        Iterator& operator--()
        {
            if (node_ == Null_)
                node_ = Null_->right_;
            else if (node_->left_ != Null_)
                node_ = find_max(node_->left, Null_);
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

        Iterator& operator--(int)
        {
            auto cpy {*this};
            --(*this);
            return cpy;
        }

        bool operator==(const Iterator& rhs) const
        {
            return node_ == rhs.node_;
        }
        
        bool operator!=(const Iterator& rhs) const
        {
            return !(*this == rhs);
        }

        node_ptr get() const {return node_;}
    }; // class Iterator

    class ConstIterator
    {
    public:
        using iterator_category = typename std::bidirectional_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = key_type;
        using const_pointer     = const key_type*;
        using const_reference   = const key_type&;
    private:
        node_ptr node_;
        const_node_ptr Null_;

    public:
        ConstIterator(node_ptr node = nullptr, const_node_ptr Null = nullptr)
        :node_ {node}, Null_ {Null}
        {}

        const_reference operator*() const
        {
            return node_->key_;
        }

        const_pointer operator->() const
        {
            return &(node_->key_);
        }

        ConstIterator& operator++()
        {
            if (node_->right_ != Null_)
                node_ = find_min(node_->right_, Null_);
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

        ConstIterator operator++(int)
        {
            auto cpy {*this};
            ++(*this);
            return cpy;
        }

        ConstIterator& operator--()
        {
            if (node_ == Null_)
                node_ = Null_->right_;
            else if (node_->left_ != Null_)
                node_ = find_max(node_->left, Null_);
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

        ConstIterator& operator--(int)
        {
            auto cpy {*this};
            --(*this);
            return cpy;
        }

        bool operator==(const ConstIterator& rhs) const
        {
            return node_ == rhs.node_;
        }
        
        bool operator!=(const ConstIterator& rhs) const
        {
            return !(*this == rhs);
        }

        node_ptr get() const {return node_;}
    }; // class ConstIterator

    Iterator begin() & {return Iterator{min_, Null_};}
    Iterator end()   & {return Iterator{Null_};}

    ConstIterator begin() const& {return ConstIterator{min_, Null_};}
    ConstIterator end()   const& {return ConstIterator{Null_, Null_};}

    ConstIterator cbegin() const& {return ConstIterator{min_, Null_};}
    ConstIterator cend()   const& {return ConstIterator{Null_, Null_};}

public:
    Iterator find(const key_type& key)
    {
        node_ptr node = root_;
        while (node != Null_)
            if (cmp(key, node->key_))
                node = node->left_;
            else if (cmp(node->key_, key))
                node = node->right_;
            else
                return Iterator{node, Null_};
    }

    std::pair<Iterator, bool> insert(const key_type& key)
    {
        // if tree is empty
        if (empty())
        {
            // increament size
            size_++;
            // create root
            root_ = new node_type{key, Colors::Black, Null_, Null_, Null_};
            // upadte min and max
            min_ = root_;
            max_ = root_;
            put_min_max_in_null();
            // return pair with itr on root and true
            return std::pair{Iterator{root_, Null_}, true};
        }
        // declare two nodes
        node_ptr x_node = root_;
        node_ptr y_node = Null_;
        
        // search place to insert
        // y will be parent
        while (x_node != Null_)
        {
            // save pointer on x before turn
            y_node = x_node;
            // if key less x.key turn left
            if (cmp(key, x_node->key_))
                x_node = x_node->left_;
            // else turn right
            else
                x_node = x_node->right_;
        }

        // if this key already in tree, then return default iterator and false
        if (cmp(y_node->key_, key) == cmp(key, y_node->key_))
            return std::pair{end(), false};

        // increament size
        size_++; 
        // create new node with key equal to input key, with red color,
        // with parent and without sons
        node_ptr z_node = new node_type{key, Colors::Red, y_node, Null_, Null_};

        // insert new node in right place
        if (cmp(z_node->key_, y_node->key_))
            y_node->left_ = z_node;
        else
            y_node->right_ = z_node;

        // fix min and max pointers
        fix_min_max(z_node);
        // fix red-black properties
        rb_insert_fix(z_node);
        // return pair with iterator on new node and true
        return std::pair{Iterator{z_node, Null_}, true};
    }

private:
    void fix_min_max(node_ptr node)
    {
        if (cmp(max_->key_, node->key_))
            max_ = node;
        if (cmp(node->key_, min_->key_))
            min_ = node;
        put_min_max_in_null();
    }


    void rb_insert_fix(node_ptr node)
    {
        // if parent color is Black of all invariants holds
        while (node->parent_->color_ == Colors::Red)
            // if parent is right son of grandparent
            if (node->parent_ == node->parent_->parent_->left_)
            {
                // uncle declare
                node_ptr uncle = node->parent_->parent_->right_;
                // Case 1
                if (uncle->color_ == Colors::Red)
                {
                    /*\___________________________________________________
                    |*                                                    |
                    |*      (Black)                          (Red)        |
                    |*      gr_par                           gr_par       |
                    |*       / \                              / \         |
                    |*      /   \      after iteartion       /   \        |
                    |*   (Red) (Red)  --------------->  (Black) (Black)   |
                    |*   parent uncle                   parent   uncle    |             
                    |*    / \   / \                       / \     / \     |
                    |*   /                               /                |
                    |* (Red)                           (Red)              |
                    |* node                            node               |
                    |*____________________________________________________|
                    \*/
                    // Comment to picture: dont matter which son of parent is node (right or left)
                    node->parent_->color_          = Colors::Black;
                    uncle->color_                  = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    // new node for new iteration of cycle
                    node = node->parent_->parent_;
                    /*\
                    |* we fix invariant that told us "son of red node is black node"
                    |* but we broke invariant that told us "for all nodes simple ways from node to leaf,
                    |* that are descendants of this node, are equal", to fix that we start new iteration with new node = gr_par
                    |* but we can change color of root from balck to red, we need to fix this invarinat in the end
                    \*/
                }
                // Case 2 and Case 3
                else
                {
                    // Case 2
                    /*\__________________________________________________
                    |*                      ______                       |
                    |*       gr_par(Black) |rotate|       gr_par(Black)  |
                    |*          /   \      | left |         /    \       |
                    |*         /   delta   |around|        /    delta    |
                    |*    par(Red)         |parent|     node(Red)        |
                    |*       / \         ----------->   /  \             |
                    |*   alpha  \                      /    gamma        |
                    |*         node(Red)            par(Red)             |
                    |*          /  \                 /  \                |
                    |*       beta  gamma          alpha beta             |
                    |* __________________________________________________|
                    \*/
                    if (node == node->parent_->right_)
                    {
                        // node is pointer on parent
                        node = node->parent_;
                        // after left rotate node is par from pictre
                        left_rotate(node);
                    }
                    // Case 3
                    /*\___________________________________________________________________
                    |*                                 ______                             |
                    |*         gr_par(Black -> Red)   |rotate|         par(Black)         |
                    |*            /    \              |right |           /   \            |
                    |*           /    delta           |around|          /     \           |
                    |*       par(Red -> Black)        |gr_par|    node(Red)  gr_par(Red)  |
                    |*        /  \                  ------------>   /  \        /  \      |
                    |*       /    gamma                          alpha beta  gamma delta  |
                    |*   node(Red)                                                        |
                    |*     /  \                                                           |
                    |*  alpha beta                                                        |
                    |* ___________________________________________________________________|
                    \*/
                    node->parent_->color_          = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    right_rotate(node->parent_->parent_);
                    /*
                     * After this case tree will be finally fixed and
                     * cycle finished because parent color is Black
                     */
                }
            }
            // symmetrical about the replacement "right" on "left" in "if" part
            else
            {
                node_ptr uncle = node->parent_->parent_->left_;
                if (uncle->color_ == Colors::Red)
                {
                    node->parent_->color_          = Colors::Black;
                    uncle->color_                  = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    node = node->parent_->parent_;
                }
                else
                {
                    if (node == node->parent_->left_)
                    {
                        node = node->parent_;
                        right_rotate(node);
                    }
                    node->parent_->color_ = Colors::Black;
                    node->parent_->parent_->color_ = Colors::Red;
                    left_rotate(node->parent_->parent_);
                }
            }
        // fix invariont that is "root is black"
        root_->color_ = Colors::Black;
    }

#ifdef DEBUG
public:  
    void debug_graph_dump(const std::string& filename) const
    {   
        std::fstream file {filename + ".dot", std::ofstream::out | std::ofstream::trunc};

        file << "digraph G {" << std::endl;
        file << "\trankdir=\"TB\"" << std::endl;
        file << "\tnode[shape=record, penwidth=3.0, style=filled, color=black];" << std::endl;
        descriptor_dump(file);
        tree_dump(file);
        file << "}" << std::endl;
        file.close();

        std::system(("dot -T svg " + filename + ".dot -o " + filename + ".svg").c_str());
        std::system(("rm " + filename + ".dot").c_str());
    }

private:
    void descriptor_dump(std::fstream& file) const
    {
        file << "\tTree [fillcolor=deeppink,label = \"{ size: " << size_ << "| <_root_> root: " << root_
        << "| <_min_> min: " << min_  << " min key: " << min_->key_
        << "| max: " << max_ << " max key: " << max_->key_ <<
        "| Null: " << Null_ << "}\"];" << std::endl;
    }

    void null_dump(std::fstream& file) const
    {
        file << "Null_" << "[fillcolor=cyan, label = \"{Null node | <_node_>ptr: " << Null_ << "| {<left>min: " << Null_->left_ <<
        "| <right>max: " << Null_->right_ << "}}\"];" << std::endl;
    }

    void tree_dump(std::fstream& file) const
    {
        if (empty())
            return;

        null_dump(file);

        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.get();
            file << "Node_" << node << "[fillcolor=";
            if (node->color_ == Colors::Red)    
                file << "red";
            else
                file << "green";
            
            file << ", label = \"{<_node_>ptr: " << node << "| parent: " << node->parent_ << "| key: " << node->key_
                << "| {<left>left: " << node->left_ << "| <right>right: " << node->right_ << "}}\"];" << std::endl;
        }

        file << "edge[penwidth=2, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.get();
            if (node->left_ != Null_)
                file << "Node_" << node << ":left:s -> Node_" << node->left_ << ":_node_:n;" << std::endl;
            if (node->right_ != Null_)
                file << "Node_" << node << ":right:s -> Node_" << node->right_ << ":_node_:n;" << std::endl;
        }
    }
#endif
}; // class SearchTree

} // namespace Container