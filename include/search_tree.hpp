#pragma once
#include <functional>
#include <set>
#include <iterator>

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

    key_type  key_ {};
    Colors  color_  = Colors::Red;
    RBNode* parent_ = nullptr, left_ = nullptr, right_ = nullptr;
};

} // namespace anonymous

template<typename KeyT = int, class Cmp = std::less<KeyT>>  
class SearchTree
{
    using node_type = RBNode<KeyT>;
    using node_ptr  = node_type*
    using key_type  = KeyT;
    using size_type = std::size_t;

    node_ptr root_ = nullptr;
    node_ptr max_  = nullptr;
    node_ptr min_  = nullptr;

    size_type size_ = 0;
    const node_ptr Null_ = new node_type{{}, Colors::Black}; // all of nullptr is replaced on null_, for minimalize checking
    Cmp cmp {};

public:
    SearchTree() = default;

    SearchTree(const key_type& key)
    :root_ {new node_type{key, Colors::Black, Null_, Null_, Null_}}, max_ {root_}, min_ {root_}, size_ {1}
    {}

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
        if (y_node->left != Null_)      
            y_node->left_->parent_ = x_node;

        // parent of y will be parent of x
        y_node->parent_ = x_node->parent_;

        // if x were root,then y is new root
        if (x_node == root_)
            root_ = y_node;
        // if x were left son of parent,
        // then y is new left son
        else if (x_node == x_node->parent_->left_)
            x_node->parent_->left = y_node;
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
    |*__________________________________________________|
    \*/
    void right_rotate(node_ptr x_node)
    {
        node_ptr y_node = x_node->left_;
        x_node->left = y_node->right_;

        if (y_node->right != Null_)
            y_node->right_->parent_ = x_node;

        y_node->parent = x_node->parent_;
        
        if (x_node == root_)
            root_ = y_node;
        else if (x_node == x_node->parent_->left_)
            x_node->parent_->left_  = y_node;
        else
            x_node->parent_->right_ = y_node;

        y_node->right_ = x_node;
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
        node_type* node_;

    public:
        Iterator(node_key* node = nullptr): node_ {node}
        {}

        reference operator*() const
        {
            return node_->key_;
        }

        pointer operator->() const
        {
            return &(node_->key_);
        }
    }; // class Iterator

    class ConstIterator
    {

    }; // class ConstIterator

private:
    void fix_min_max(node_ptr node)
    {
        if (cmp(max_->key_, node->key_))
            max_ = node;
        if (cmp(node->key_, min_->key))
            min_ = node;
    }

public:
    bool empty() const {return (size_ == 0);}

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
            // return pair with itr on root and true
            return std::pair{Iterator{root_}, true};
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
            if (cmp(key, x_node->key))
                x_node = x_node->left_;
            // else turn right
            else
                x_node = x_node->right_;
        }

        // if this key already in tree, then return default iterator and false
        if (cmp(y_node->key_, key) == cmp(key, y_node->key_))
            return std::pair{Iterator{}, false};

        // increament size
        size_++; 
        // create new node with key equal to input key, with red color, with parent and without sons
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
        return std::pair{Iterator{z_node}, true};
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
                    |*                      ______
                    |*       gr_par(Black) |rotate|       gr_par(Black)
                    |*          /   \      | left |         /    \
                    |*         /   delta   |around|        /    delta
                    |*    par(Red)         |parent|     node(Red)   
                    |*       / \         ----------->   /  \ 
                    |*   alpha  \                      /    gamma
                    |*         node(Red)            par(Red)
                    |*          /  \                 /  \
                    |*       beta  gamma          alpha beta
                    |* ___________________________________________________
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
                    |*                                 ______
                    |*         gr_par(Black -> Red)   |rotate|         par(Black)        
                    |*            /    \              |right |           /   \
                    |*           /    delta           |around|          /     \
                    |*       par(Red -> Black)        |gr_par|    node(Red)  gr_par(Red)  
                    |*        /  \                  ------------>   /  \        /  \
                    |*       /    gamma                          alpha beta  gamma delta
                    |*   node(Red)
                    |*     /  \
                    |*  alpha beta
                    |* ___________________________________________________________________
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
                    left_rotate(node->parent_->parent);
                }
            }
        // fix invariont that is "root is black"
        root_->color_ = Colors::Black;
    }
}; // class SearchTree

} // namespace Container