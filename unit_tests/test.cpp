#include <gtest/gtest.h>
#define DEBUG
#include "search_tree.hpp"

using namespace Container;

TEST(Tree, default_ctor)
{
    RBSearchTree tree0 {};
}

TEST(Tree, insert)
{
    RBSearchTree tree {};
    for (auto i = 0; i < 40; i++)
    {
        if (i % 2)
            tree.insert(i);
        else
            tree.insert(i + 20);
    }

    RBSearchTree tree1 {12, 1, 4, 5, -4, 8, 7, -9};
    RBSearchTree tree2 {12, 1, 4, 5, -4, 8, 7, -9};
    std::array<int, 10> arr1 {2, 7, 12, 100, -12, 56, 10, 9, 11, 5};
    std::array<int, 15> res {-12, -9, -4, 1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 56, 100};

    for (auto x: arr1)
        tree1.insert(x);
    
    tree2.insert(arr1.cbegin(), arr1.cend());

    auto i = 0;
    for (auto x: tree1)
        EXPECT_EQ(x, res[i++]);

    i = 0;
    for (auto x: tree2)
        EXPECT_EQ(x, res[i++]);
}

TEST(Tree, init_list_and_cont_ctor)
{
    RBSearchTree tree0 {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    std::array<int, 14> arr {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    RBSearchTree tree1 (arr.cbegin(), arr.cend());
    std::array<int, 10> res {-20, -4, -2, 1, 8, 9, 10, 17, 21, 34};

    auto i = 0;
    for (auto x: tree0)
        EXPECT_EQ(x, res[i++]);

    i = 0;
    for (auto x: tree1)
        EXPECT_EQ(x, res[i++]);

}

TEST(Tree, Iterators)
{
    static_assert(std::bidirectional_iterator<RBSearchTree<>::Iterator>);
    static_assert(std::bidirectional_iterator<RBSearchTree<>::ConstIterator>);

    std::array<int, 8> arr {-2, 1, 2, 4, 5, 6, 7, 9};

    RBSearchTree tree {1, 2, 4, 7, 9, -2, 5, 6};
    std::size_t i = 0;
    for (auto x: tree)
        EXPECT_EQ(x, arr[i++]);

    const RBSearchTree ctree {1, 2, 4, 7, 9, -2, 5, 6};
    i = 0;
    for (auto x: tree)
        EXPECT_EQ(x, arr[i++]);
}


TEST(Tree, big_five)
{
    RBSearchTree tree1 {1, 2, 3, 4, -10, 5, 8, 9, 11};
    auto tree2 {tree1};

    for (auto x: tree1)
        std::cout << x << " ";
    std::cout << std::endl;

    for (auto x: tree2)
        std::cout << x << " ";
    std::cout << std::endl;

    EXPECT_EQ(tree1.size(), tree2.size());
    
    for (auto itr1 = tree1.cbegin(), itr2 = tree2.cbegin(),
    end1 = tree1.cend(), end2 = tree2.cend(); itr1 != end1 && itr2 != end2; ++itr1, ++itr2)
        EXPECT_EQ(*itr1, *itr2);
    
    RBSearchTree tree3 {};
    EXPECT_EQ(tree3.size(), 0);

    auto tree4 {std::move(tree1)};
    EXPECT_EQ(tree4.size(), tree2.size());

    for (auto itr4 = tree4.cbegin(), itr2 = tree2.cbegin(),
    end4 = tree4.cend(), end2 = tree2.cend(); itr4 != end4 && itr2 != end2; ++itr4, ++itr2)
        EXPECT_EQ(*itr4, *itr2);

    RBSearchTree tree5 {};
    EXPECT_EQ(tree5.size(), 0);

    tree5 = std::move(tree2);

    for (auto itr4 = tree4.cbegin(), itr5 = tree5.cbegin(),
    end4 = tree4.cend(), end5 = tree5.cend(); itr4 != end4 && itr5 != end5; ++itr4, ++itr5)
        EXPECT_EQ(*itr4, *itr5);
}

TEST(Tree, erase)
{
    RBSearchTree tree {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    auto er1 = tree.erase(1);
    EXPECT_EQ(*er1, 2);
    EXPECT_EQ(*tree.erase(er1), 3);

    auto citr = tree.cbegin();
    EXPECT_EQ(*tree.erase(citr), 3);

    RBSearchTree tree1 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto itr1 = std::next(tree1.begin(), 6);
    auto itr2 = std::next(tree1.begin(), 12);

    EXPECT_EQ(*tree1.erase(itr1, itr2), 12);

    RBSearchTree tree2 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto citr1 = std::next(tree2.cbegin(), 6);
    auto citr2 = std::next(tree2.cbegin(), 12);

    EXPECT_EQ(*tree2.erase(citr1, citr2), 12);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
