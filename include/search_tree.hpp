#pragma once
#include <functional>
#include <stack>
#include <set>
#include <iterator>
#include <fstream>
#include <string>

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
    using rbnode_ptr = RBNode*;   

    key_type  key_ {};
    detail::Colors  color_  = detail::Colors::Red;
    rbnode_ptr parent_ = nullptr, left_ = nullptr, right_ = nullptr;

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

} // namespace detail

template<typename KeyT = int, class Cmp = std::less<KeyT>>  
class SearchTree
{
    using node_type      = detail::RBNode<KeyT>;
    using node_ptr       = node_type*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using size_type      = typename std::size_t;

    node_ptr null_init() const
    {
        node_ptr Null = new node_type{{}, detail::Colors::Black};
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

// sub func
private:
    void put_min_max_in_null()
    {
        Null_->left_  = min_;
        Null_->right_ = max_;
    }

//----------------------------------------=| Ctors start |=---------------------------------------------
public:
    SearchTree() = default;

    SearchTree(const key_type& key)
    :root_ {new node_type{key, detail::Colors::Black, Null_, Null_, Null_}}, max_ {root_}, min_ {root_}, size_ {1}
    {
        put_min_max_in_null();
    }

    SearchTree(std::initializer_list<key_type> list)
    {
        for (auto x: list)
            insert(x);
    }

    template<std::input_iterator It>
    SearchTree(It begin, It end)
    {   
        for (auto itr = begin; itr != end; ++itr)
            insert(*itr);
    }
//----------------------------------------=| Ctors end |=-----------------------------------------------

//----------------------------------------=| Size`s funcs start |=--------------------------------------
    size_type size() const {return size_;}

    bool empty() const {return (size_ == 0);}
//----------------------------------------=| Size`s funcs end |=----------------------------------------

//----------------------------------------=| Big five start |=------------------------------------------
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

        min_ = detail::find_min(root_, Null_);
        max_ = detail::find_max(root_, Null_);
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

    virtual ~SearchTree()
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
//----------------------------------------=| Big five end |=--------------------------------------------

//----------------------------------------=| Algorithm funcs start |=-----------------------------------
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
    void left_rotate(node_ptr x)
    {
        // declare y as right son of x
        node_ptr y = x->right_;
        // new right son of x is yl 
        x->right_ = y->left_;

        // if yl exist (not nullptr)
        // replace parent of yl from y to x
        if (y->left_ != Null_)      
            y->left_->parent_ = x;

        // parent of y will be parent of x
        y->parent_ = x->parent_;

        // if x were root,then y is new root
        if (x == root_)
            root_ = y;
        // if x were left son of parent,
        // then y is new left son
        else if (x == x->parent_->left_)
            x->parent_->left_ = y;
        // else x were right son of parent,
        // then y is new right son
        else
            x->parent_->right_ = y;

        // x is y left son now
        y->left_ = x;
        // parent of x is y now
        x->parent_ = y;
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
    void right_rotate(node_ptr x)
    {
        node_ptr y = x->left_;
        x->left_ = y->right_;

        if (y->right_ != Null_)
            y->right_->parent_ = x;

        y->parent_ = x->parent_;
        
        if (x == root_)
            root_ = y;
        else if (x == x->parent_->left_)
            x->parent_->left_  = y;
        else
            x->parent_->right_ = y;

        y->right_  = x;
        x->parent_ = y;
    }
//----------------------------------------=| Algorithm funcs end |=-------------------------------------

//----------------------------------------=| Iterators start |=-----------------------------------------
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
                node_ = detail::find_max(node_->left, Null_);
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

        Iterator operator--(int)
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
                node_ = detail::find_max(node_->left, Null_);
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

        ConstIterator operator--(int)
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

//----------------------------------------=| Iterators end |=-------------------------------------------

//----------------------------------------=| Find start |=----------------------------------------------
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
//----------------------------------------=| Find end |=------------------------------------------------

//----------------------------------------=| Insert end |=----------------------------------------------
    std::pair<Iterator, bool> insert(const key_type& key)
    {
        // if tree is empty
        if (empty())
        {
            // increament size
            size_++;
            // create root
            root_ = new node_type{key, detail::Colors::Black, Null_, Null_, Null_};
            // upadte min and max
            min_ = root_;
            max_ = root_;
            put_min_max_in_null();
            // return pair with itr on root and true
            return std::pair{Iterator{root_, Null_}, true};
        }
        // declare two nodes
        node_ptr x = root_;
        node_ptr y = Null_;
        
        // search place to insert
        // y will be parent
        while (x != Null_)
        {
            // save pointer on x before turn
            y = x;
            // if key less x.key turn left
            if (cmp(key, x->key_))
                x = x->left_;
            // else turn right
            else
                x = x->right_;
        }

        // if this key already in tree, then return default iterator and false
        if (cmp(y->key_, key) == cmp(key, y->key_))
            return std::pair{end(), false};

        // increament size
        size_++; 
        // create new node with key equal to input key, with red color,
        // with parent and without sons
        node_ptr z = new node_type{key, detail::Colors::Red, y, Null_, Null_};

        // insert new node in right place
        if (cmp(z->key_, y->key_))
            y->left_ = z;
        else
            y->right_ = z;
        // fix red-black properties
        rb_insert_fix(z);
        // return pair with iterator on new node and true
        return std::pair{Iterator{z, Null_}, true};
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
        // fix min and max pointers
        fix_min_max(node);
        // if parent color is Black of all invariants holds
        while (node->parent_->color_ == detail::Colors::Red)
            // if parent is right son of grandparent
            if (node->parent_ == node->parent_->parent_->left_)
            {
                // uncle declare
                node_ptr uncle = node->parent_->parent_->right_;
                // Case 1
                if (uncle->color_ == detail::Colors::Red)
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
                    node->parent_->color_          = detail::Colors::Black;
                    uncle->color_                  = detail::Colors::Black;
                    node->parent_->parent_->color_ = detail::Colors::Red;
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
                    node->parent_->color_          = detail::Colors::Black;
                    node->parent_->parent_->color_ = detail::Colors::Red;
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
                if (uncle->color_ == detail::Colors::Red)
                {
                    node->parent_->color_          = detail::Colors::Black;
                    uncle->color_                  = detail::Colors::Black;
                    node->parent_->parent_->color_ = detail::Colors::Red;
                    node = node->parent_->parent_;
                }
                else
                {
                    if (node == node->parent_->left_)
                    {
                        node = node->parent_;
                        right_rotate(node);
                    }
                    node->parent_->color_ = detail::Colors::Black;
                    node->parent_->parent_->color_ = detail::Colors::Red;
                    left_rotate(node->parent_->parent_);
                }
            }
        // fix invariont that is "root is black"
        root_->color_ = detail::Colors::Black;
    }
//----------------------------------------=| Insert end |=----------------------------------------------

//----------------------------------------=| Erase start |=---------------------------------------------
public:
    // replace subtree with root u with subtree with root v
    void transplant(node_ptr u, node_ptr v)
    {
        if (u == root_)
            root_ = v;
        else if (u->is_left_son())
            u->parent_->left_ = v;
        else
            u->parent_->right_ = v;
        u->parent_ = v->parent_;
    }

    // delete z from tree with saving all invariants
    void erase_by_ptr(node_ptr z)
    {
        // declare two pointer, y - replacment for z in else case
        // x - root of subtree, where we need fix invarinats 
        node_ptr y = z, x = Null_;
        auto y_original_color = y->color_;
        // if z has only right subtree
        /*\_____________________________
        |*     |                  |     |
        |*     |                  |     |
        |*     z    --------->    zr    |
        |*      \                /  \   |
        |*       \                      |
        |*        zr                    |
        |*       /  \                   |
        |* _____________________________|
        \*/
        if (z->left_ == Null_)
        {
            // save root os subtree that will be replace
            x = z->right_;
            // replace right subtree of z with z
            transplant(z, z->right_);
        }
        // if z has only left subtree
        /*\_____________________________
        |*     |                  |     |
        |*     |                  |     |
        |*     z    --------->    zl    |
        |*    /                  /  \   |
        |*   /                          |
        |*  zl                          |
        |* /  \                         |
        |* _____________________________|
        \*/
        else if (z->right_ == Null_)
        {
            x = z->left_;
            transplant(z, z->left_);
        }
        /*\____________________________________________________
        |*      z                z                     y       |
        |*     / \              /                     / \      | 
        |*    /   \            /    y                /   \     |
        |*   zl   zr          zl     \              zl   zr    |
        |*  / \   / \        /  \     \                  / \   |
        |*       ~       1             zr      2        ~      |
        |*      /      ---->          /  \   ---->     /       |
        |*     y                     /                yr       | 
        |*      \                   yr               /  \      |
        |*       \                 /  \                        |
        |*        yr                                           |
        |*       /  \                                          | 
        |* ____________________________________________________|
        \*/
        else
        {
            // find y in right subtree of x
            // y most left of z->right than y->left_ == Null_
            y = detail::find_min(z->right_, Null_);
            // save original color of y node
            // (look at the end of method to see cause)
            y_original_color = y->color_;
            // save root of subtree that we maybe need to fix
            x = y->right_;
            
            //first arrow on picture
            // if y right son of z
            if (y->parent_ == z)
                // only set parent of x as y
                x->parent_ = y;
            else
            {

                // replace y with right subtree of y
                // after this x->parent_ will have correct value
                transplant(y, y->right_);
                // connect right son of z and y
                y->right_ = z->right_;
                y->right_->parent_ = y;
            }
            // second arrow on picture
            // replace z with y
            transplant(z, y);
            // connect left son of z with y
            y->left_ = z->left_;
            y->left_->parent_ = y;
            // to save invariant everywhere except in subtree with root x
            y->color_ = z->color_;
        }

        // clear memory allocated on z node
        delete z;

        // in first two cases ("if" and "else if")
        // if z had the Red color, then we cant broke any invarinats
        // in third case, if y was Red we cant broke any invarints too
        if (y_original_color == detail::Colors::Black)
            rb_delete_fixup(x);
    }

    void rb_delete_fixup(node_ptr x)
    {
        while (true) {}
    }
//----------------------------------------=| Erase end |=-----------------------------------------------

//----------------------------------------=| Graph dump start |=----------------------------------------
#ifdef DEBUG
public:  
    void debug_graph_dump(const std::string& filename) const
    {   
        std::fstream file {filename + ".dot", std::ofstream::out | std::ofstream::trunc};

        file << "digraph G {" << std::endl;
        file << "\trankdir=\"TB\"" << std::endl;
        file << "\tnode[shape=record, penwidth=3.0, style=filled, color=black, fontcolor=white];" << std::endl;
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
        file << "\tTree [fillcolor=purple, label = \"{ SearchTree\\ndescriptor| size: " << size_ << "| <root> root:\\n " << root_
        << "| min:\\n " << min_  << "\\n min key: " << min_->key_
        << "| max:\\n " << max_ << "\\n max key: " << max_->key_ <<
        "| <null> Null:\\n " << Null_ << "}\"];" << std::endl;
    }

    void null_dump(std::fstream& file) const
    {
        file << "Null_" << "[fillcolor=navy, label = \"{Null node | ptr:\\n " << Null_ << "| {min:\\n " << Null_->left_ <<
        "| max:\\n " << Null_->right_ << "}}\"];" << std::endl;
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
            if (node->color_ == detail::Colors::Red)    
                file << "red, fontcolor=black";
            else
                file << "black, color=red";
            
            file << ", label = \"{<_node_>ptr:\\n " << node << "| parent:\\n " << node->parent_ << "| key: " << node->key_
                << "| {<left>left:\\n " << node->left_ << "| <right>right:\\n " << node->right_ << "}}\"];" << std::endl;
        }

        file << "edge[penwidth=3, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.get();
            if (node->left_ != Null_)
                file << "Node_" << node << ":left:s -> Node_" << node->left_ << ":_node_:n;" << std::endl;
            if (node->right_ != Null_)
                file << "Node_" << node << ":right:s -> Node_" << node->right_ << ":_node_:n;" << std::endl;
        }

        file << "Tree:root:e -> Node_" << root_ << ":_node_:n;" << std::endl;
        file << "Tree:null:w -> Null_:n;" << std::endl;
    }
#endif
//----------------------------------------=| Graph dump end |=------------------------------------------
}; // class SearchTree

} // namespace Container