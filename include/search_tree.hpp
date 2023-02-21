#pragma once
#include <functional>
#include <fstream>
#include <string>
#include "node.hpp"
#include "search_tree_iterator.hpp"
#include <unordered_map>

namespace Container
{

template<typename KeyT = int, typename Node = detail::RBNode<KeyT>, class Cmp = std::less<KeyT>>  
class RBSearchTree
{
    using node_type      = Node;
    using node_ptr       = Node*;
    using const_node_ptr = const node_type*;
    using key_type       = KeyT;
    using reference      = key_type&;
    using size_type      = typename std::size_t;
    using Colors         = detail::Colors;

    node_ptr null_init()
    {
        auto Null = new node_type{key_type{}, Colors::Black};
        Null->left_   = Null;
        Null->right_  = Null;
        Null->parent_ = Null;
        return Null;
    }

    node_ptr Null_ = null_init(); // all of nullptr is replaced on Null_, for minimalize checking

    node_ptr root_ = Null_;
    node_ptr max_  = Null_;
    node_ptr min_  = Null_;

    size_type size_ = 0;

    Cmp cmp {};
    bool key_less(const key_type& key1, const key_type& key2) const {return cmp(key1, key2);}
    bool key_equal(const key_type& key1, const key_type& key2) const
    {
        return !cmp(key1, key2) && !cmp(key2, key1);
    }

// sub func
private:
    void put_min_max_in_null()
    {
        Null_->left_  = min_;
        Null_->right_ = max_;
    }

//----------------------------------------=| Ctors start |=---------------------------------------------
public:
    RBSearchTree() = default;

    template<std::input_iterator InpIt>
    RBSearchTree(InpIt first, InpIt last)
    {   
        RBSearchTree tmp {};
        tmp.insert(first, last);
        std::swap(*this, tmp);
    }

    RBSearchTree(std::initializer_list<key_type> initlist)
    :RBSearchTree(initlist.begin(), initlist.end())
    {}
//----------------------------------------=| Ctors end |=-----------------------------------------------

//----------------------------------------=| Size`s funcs start |=--------------------------------------
    size_type size() const {return size_;}

    bool empty() const {return (size_ == 0);}
//----------------------------------------=| Size`s funcs end |=----------------------------------------

//----------------------------------------=| Big five start |=------------------------------------------
private:
    void swap(RBSearchTree& rhs) noexcept
    {
        std::swap(Null_, rhs.Null_);
        std::swap(root_, rhs.root_);
        std::swap(max_, rhs.max_);
        std::swap(min_, rhs.min_);
        std::swap(size_, rhs.size_);
    }

public:
    RBSearchTree(RBSearchTree&& other)
    {
        swap(other);
    }
    RBSearchTree& operator=(RBSearchTree&& rhs)
    {
        swap(rhs);
        return *this;
    }

private:
    void insert_left(node_ptr this_current, node_ptr other_left)
    {
        this_current->left_ = new node_type{other_left->key_, other_left->color_, this_current, Null_, Null_};
    }

    void insert_right(node_ptr this_current, node_ptr other_right)
    {
        this_current->right_ = new node_type{other_right->key_, other_right->color_, this_current, Null_, Null_};
    }

    void insert_root(node_ptr other_root)
    {
        root_ = new node_type{other_root->key_, other_root->color_, Null_, Null_, Null_};
    }

public:
    RBSearchTree(const RBSearchTree& other): size_ {other.size_}
    {
        if (empty())
            return;

        RBSearchTree tmp {std::move(*this)};

        tmp.insert_root(other.root_);
        auto tmp_current   = tmp.root_;
        auto other_current = other.root_;

        while (other_current != other.Null_)
        {
            if (other_current->left_ != other.Null_ && tmp_current->left_ == tmp.Null_)
            {
                tmp.insert_left(tmp_current, other_current->left_);
                other_current = other_current->left_;
                tmp_current   = tmp_current->left_;
            }
            else if (other_current->right_ != other.Null_ && tmp_current->right_ == tmp.Null_)
            {
                tmp.insert_right(tmp_current, other_current->right_);
                other_current = other_current->right_;
                tmp_current   = tmp_current->right_;
            }
            else
            {
                other_current = other_current->parent_;
                tmp_current   = tmp_current->parent_;
            }
        }

        std::swap(*this, tmp);
        
        min_ = detail::find_min(root_, Null_);
        max_ = detail::find_max(root_, Null_);
        put_min_max_in_null();  
    }

    RBSearchTree& operator=(const RBSearchTree& rhs)
    {
        auto rhs_cpy {rhs};
        std::swap(*this, rhs);
        return *this;
    }

    virtual ~RBSearchTree()
    {
        if (empty())
        {
            delete Null_;
            return;
        }

        auto current = root_;
        while (current != Null_)
        {
            if (current->left_ != Null_)
                current = current->left_;
            else if (current->right_ != Null_)
                current = current->right_;
            else
            {
                auto parent = current->parent_;

                if (current == root_)
                    break;
                else if (current->is_left_son())
                    parent->left_ = Null_;
                else
                    parent->right_ = Null_;

                delete current;
                current = parent;
            }
        }
        delete root_;
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
    void left_rotate(node_ptr x) noexcept
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
    void right_rotate(node_ptr x) noexcept
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
    using Iterator      = detail::SearchTreeIterator<key_type, node_type>;
    using ConstIterator = detail::SearchTreeIterator<const key_type, node_type>;

    Iterator begin() {return Iterator{min_, Null_};}
    Iterator end()   {return Iterator{Null_};}

    ConstIterator begin() const {return ConstIterator{min_, Null_};}
    ConstIterator end()   const {return ConstIterator{Null_, Null_};}

    ConstIterator cbegin() const {return ConstIterator{min_, Null_};}
    ConstIterator cend()   const {return ConstIterator{Null_, Null_};}

//----------------------------------------=| Iterators end |=-------------------------------------------

//----------------------------------------=| Find start |=----------------------------------------------
public:
    Iterator find(const key_type& key)
    {
        node_ptr node = root_;
        while (node != Null_)
            if (key_less(key, node->key_))
                node = node->left_;
            else if (key_less(node->key_, key))
                node = node->right_;
            else
                return Iterator{node, Null_};
        return end();
    }
//----------------------------------------=| Find end |=------------------------------------------------

    node_ptr find_parent(const key_type& key) const noexcept
    {
        auto x = root_;
        auto y = Null_;

        while (x != Null_)
        {
            // save pointer on x before turn
            y = x;
            // if key less x.key turn left
            if (key_less(key, x->key_))
                x = x->left_;
            // else turn right
            else if (key_equal(key, x->key_))
                return x;
            else
                x = x->right_;
        }
        return y;
    }

//----------------------------------------=| Insert start |=--------------------------------------------
    void insert_by_ptr(node_ptr node) noexcept
    {
        assert(node != nullptr);
        assert(node->color_ == Colors::Red);
        assert(node->left_ == Null_);
        assert(node->right_ == Null_);

        // if tree is empty
        if (node->parent_ == Null_)
        {
            // root is node
            root_ = node;
            root_->color_ = Colors::Black;
            // upadte min and max
            min_ = root_;
            max_ = root_;
            put_min_max_in_null();
            return;
        }

        // increment size
        size_++;
        
        // insert new node in right place
        if (key_less(node->key_, node->parent_->key_))
            node->parent_->left_ = node;
        else
            node->parent_->right_ = node;
        // fix red-black properties
        rb_insert_fix(node);
    }

    std::pair<Iterator, bool> insert(const key_type& key)
    {
        key_type key_cpy {key};
        return insert(std::move(key_cpy));
    }

    std::pair<Iterator, bool> insert(key_type&& key) noexcept
    {
        auto parent = find_parent(key);

        // if key is alredy in tree
        if (parent != Null_ && key_equal(parent->key_, key))
            return std::pair{Iterator{parent, Null_}, false};

        node_ptr new_node = new node_type{std::move(key), Colors::Red, parent, Null_, Null_};
        insert_by_ptr(new_node);
        return std::pair{Iterator{new_node, Null_}, true};
    }

public:
    template<std::input_iterator InpIt>
    void insert(InpIt first, InpIt last)
    {
        for (auto itr = first; itr != last; ++itr)
            insert(*itr);
    }

    void insert(std::initializer_list<key_type> initlist)
    {
        insert(initlist.begin(), initlist.end());
    }

private:
    void insert_fix_min_max(node_ptr node) noexcept
    {
        if (key_less(max_->key_, node->key_))
            max_ = node;
        if (key_less(node->key_, min_->key_))
            min_ = node;
        put_min_max_in_null();
    }

    void rb_insert_fix(node_ptr node) noexcept
    {
        // fix min and max pointers
        insert_fix_min_max(node);
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
//----------------------------------------=| Insert end |=----------------------------------------------

//----------------------------------------=| Erase start |=---------------------------------------------
public:
    // replace subtree with root u with subtree with root v
    void transplant(node_ptr u, node_ptr v) noexcept
    {
        if (u == root_)
            root_ = v;
        else if (u->is_left_son())
            u->parent_->left_ = v;
        else
            u->parent_->right_ = v;
        v->parent_ = u->parent_;
    }

    // delete z from tree with saving all invariants
    void erase_from_tree(node_ptr z)
    {
        // decrement size
        size_--;
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

        // in first two cases ("if" and "else if")
        // if z had the Red color, then we cant broke any invarinats
        // in third case, if y was Red we cant broke any invarints too
        //if (y_original_color == Colors::Black)
        //    rb_delete_fixup(x);
        delete_fix_min_max(z);
    }

private:
    void delete_fix_min_max(node_ptr node)
    {
        if (node == min_)
            min_ = detail::find_min(root_, Null_);
        else if (node == max_)
            max_ = detail::find_max(root_, Null_);
        put_min_max_in_null();
    }

    void rb_delete_fixup(node_ptr x) noexcept
    {
        while (x != root_ && x->color_ == Colors::Black)
            if (x->is_left_son())
            {
                node_ptr w = x->parent_->right_;
                if (w->color_ == Colors::Red)
                {
                    w->color_ = Colors::Black;
                    x->parent_->color_ = Colors::Red;
                    left_rotate(x->parent_);
                    w = x->parent_->right_;
                }
                if (w->left_->color_ == Colors::Black && w->right_->color_ == Colors::Black)
                {
                    w->color_ = Colors::Red;
                    x = x->parent_;
                }
                else
                {
                    if (w->right_->color_ == Colors::Black)
                    {
                        w->left_->color_ = Colors::Black;
                        w->color_ = Colors::Red;
                        right_rotate(w);
                        w = x->parent_->right_;
                    }
                    w->color_ = x->parent_->color_;
                    x->parent_->color_ = Colors::Black;
                    w->right_->color_ = Colors::Black;
                    left_rotate(x->parent_);
                    x = root_;
                }
            }
            else
            {
                node_ptr w = x->parent_->left_;
                if (w->color_ == Colors::Red)
                {
                    w->color_ = Colors::Black;
                    x->parent_->color_ = Colors::Red;
                    right_rotate(x->parent_);
                    w = x->parent_->left_;
                }
                if (w->right_->color_ == Colors::Black && w->left_->color_ == Colors::Black)
                {
                    w->color_ = Colors::Red;
                    x = x->parent_;
                }
                else
                {
                    if (w->left_->color_ == Colors::Black)
                    {
                        w->right_->color_ = Colors::Black;
                        w->color_ = Colors::Red;
                        left_rotate(w);
                        w = x->parent_->left_;
                    }
                    w->color_ = x->parent_->color_;
                    x->parent_->color_ = Colors::Black;
                    w->left_->color_ = Colors::Black;
                    right_rotate(x->parent_);
                    x = root_;
                }
            }
        x->color_ = Colors::Black;
    }


public:
    Iterator erase(Iterator itr)
    {
        if (itr == end())
            return end();
        auto itr_next = std::next(itr);
        auto node = itr.base();
        erase_from_tree(node);
        delete node;
        return itr_next;
    }

    Iterator erase(ConstIterator itr)
    {
        return erase(Iterator{itr.base(), Null_});
    }

    Iterator erase(const key_type& key)
    {
        return erase(find(key));
    }

    Iterator erase(Iterator first, Iterator last)
    {
        Iterator ret {};
        while (first != last)
            ret = erase(first++);
        return ret;
    }

    Iterator erase(ConstIterator first, ConstIterator last)
    {
        Iterator ret {};
        while (first != last)
            ret = erase(first++);
        return ret;
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
        file << "\tTree [fillcolor=purple, label = \"{ RBSearchTree\\ndescriptor| size: " << size() << "| <root> root:\\n " << root_
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
            node_ptr node = itr.base();
            file << "Node_" << node << "[fillcolor=";
            if (node->color_ == Colors::Red)    
                file << "red, fontcolor=black";
            else
                file << "black, color=red";
            
            file << ", label = \"{<_node_>ptr:\\n " << node << "| parent:\\n " << node->parent_ << "| key: " << node->key_
                << "| {<left>left:\\n " << node->left_ << "| <right>right:\\n " << node->right_ << "}}\"];" << std::endl;
        }

        file << "edge[penwidth=3, color=black];" << std::endl;
        for (auto itr = cbegin(), end = cend(); itr != end; ++itr)
        {
            node_ptr node = itr.base();
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
}; // class RBSearchTree

} // namespace Container