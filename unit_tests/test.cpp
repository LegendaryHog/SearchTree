#include <gtest/gtest.h>
#define DEBUG
#include "search_tree.hpp"
#include "boost_set.hpp"
#include "set.hpp"

using namespace Container;

TEST(Tree, default_ctor)
{
    Set tree0 {};
}

TEST(Tree, insert)
{
    Set tree {};
    for (auto i = 0; i < 40; i++)
    {
        if (i % 2)
            tree.insert(i);
        else
            tree.insert(i + 20);
    }

    Set tree1 {12, 1, 4, 5, -4, 8, 7, -9};
    Set tree2 {12, 1, 4, 5, -4, 8, 7, -9};
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
    Set tree0 {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    std::array<int, 14> arr {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    Set tree1 (arr.cbegin(), arr.cend());
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
    static_assert(std::bidirectional_iterator<Set<>::Iterator>);
    static_assert(std::bidirectional_iterator<Set<>::ConstIterator>);

    std::array<int, 8> arr {-2, 1, 2, 4, 5, 6, 7, 9};

    Set tree1 {1, 2, 4, 7, 9, -2, 5, 6};
    std::size_t i = 0;
    for (auto x: tree1)
        EXPECT_EQ(x, arr[i++]);

    const Set ctree {1, 2, 4, 7, 9, -2, 5, 6};
    i = 0;
    for (auto x: ctree)
        EXPECT_EQ(x, arr[i++]);

    Set tree {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_EQ(11, *std::prev(tree.end()));
    EXPECT_EQ(11, *std::prev(tree.cend()));

    auto itr  = tree.begin();
    auto citr = tree.cbegin();
    EXPECT_EQ(*itr++, 0);
    EXPECT_EQ(*citr++, 0);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*++itr, 2);
    EXPECT_EQ(*++citr, 2);
    EXPECT_EQ(*itr--, 2);
    EXPECT_EQ(*citr--, 2);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*--itr, 0);
    EXPECT_EQ(*--citr, 0);

    EXPECT_EQ(std::next(tree.begin(), 5), std::prev(tree.end(), 7));
    EXPECT_EQ(std::distance(tree.begin(), tree.end()), tree.size());
}

TEST(Tree, big_five)
{
    Set tree1 {1, 2, 3, 4, -10, 5, 8, 9, 11};
    Set tree2 {tree1};

    EXPECT_EQ(tree1.size(), tree2.size());
    
    for (auto itr1 = tree1.cbegin(), itr2 = tree2.cbegin(),
    end1 = tree1.cend(), end2 = tree2.cend(); itr1 != end1 && itr2 != end2; ++itr1, ++itr2)
        EXPECT_EQ(*itr1, *itr2);
    
    Set tree3 {};
    EXPECT_EQ(tree3.size(), 0);

    auto tree4 {std::move(tree1)};
    EXPECT_EQ(tree4.size(), tree2.size());

    for (auto itr4 = tree4.cbegin(), itr2 = tree2.cbegin(),
    end4 = tree4.cend(), end2 = tree2.cend(); itr4 != end4 && itr2 != end2; ++itr4, ++itr2)
        EXPECT_EQ(*itr4, *itr2);

    Set tree5 {};
    EXPECT_EQ(tree5.size(), 0);

    tree5 = std::move(tree2);

    for (auto itr4 = tree4.cbegin(), itr5 = tree5.cbegin(),
    end4 = tree4.cend(), end5 = tree5.cend(); itr4 != end4 && itr5 != end5; ++itr4, ++itr5)
        EXPECT_EQ(*itr4, *itr5);
}

TEST(Tree, erase)
{
    Set tree {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    auto er1 = tree.erase(1);
    EXPECT_EQ(*er1, 2);
    EXPECT_EQ(*tree.erase(er1), 3);

    auto citr = tree.cbegin();
    EXPECT_EQ(*tree.erase(citr), 3);

    Set tree1 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto itr1 = std::next(tree1.begin(), 6);
    auto itr2 = std::next(tree1.begin(), 12);

    EXPECT_EQ(*tree1.erase(itr1, itr2), 12);

    Set tree2 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto citr1 = std::next(tree2.cbegin(), 6);
    auto citr2 = std::next(tree2.cbegin(), 12);

    EXPECT_EQ(*tree2.erase(citr1, citr2), 12);
}

TEST(Tree, bounds)
{
    Set tree {-5, -4, -3, 6, 8, 9, 10, 11, 15, 17};

    EXPECT_EQ(*tree.upper_bound(-4), -3);
    EXPECT_EQ(*tree.upper_bound(6), 8);
    EXPECT_EQ(*tree.upper_bound(7), 8);
    EXPECT_EQ(*tree.upper_bound(13), 15);
    EXPECT_EQ(*tree.upper_bound(15), 17);

    EXPECT_EQ(*tree.lower_bound(-5), -5);
    EXPECT_EQ(*tree.lower_bound(0), 6);
    EXPECT_EQ(*tree.lower_bound(-4), -4);
    EXPECT_EQ(*tree.lower_bound(6), 6);
    EXPECT_EQ(*tree.lower_bound(7), 8);
}

TEST(BoostSet, big_five_and_insert)
{
    BoostSet set {1, 2, 3, 4, 5, 6, 7, 8, 12, 34, -7, -9, 15, 3};
    set.insert(9);
    
    // copy ctor
    BoostSet set_cpy {set};
    EXPECT_EQ(set.size(), set_cpy.size());
    for (auto itr = set.begin(), itr_cpy = set_cpy.begin(), end = set.end(), end_cpy = set_cpy.end();
        itr != end && itr_cpy != end_cpy; ++itr, ++itr_cpy)
        EXPECT_EQ(*itr, *itr_cpy);

    BoostSet set_cpy1 {1, 2, 3};
    set_cpy1 = set;
    EXPECT_EQ(set.size(), set_cpy1.size());
    for (auto itr = set.begin(), itr_cpy1 = set_cpy1.begin(), end = set.end(), end_cpy1 = set_cpy1.end();
        itr != end && itr_cpy1 != end_cpy1; ++itr, ++itr_cpy1)
        EXPECT_EQ(*itr, *itr_cpy1);


    // move ctor
    BoostSet set_move {std::move(set_cpy)};
    EXPECT_EQ(set.size(), set_move.size());
    for (auto itr = set.begin(), itr_move = set_move.begin(), end = set.end(), end_move = set_move.end();
        itr != end && itr_move != end_move; ++itr, ++itr_move)
        EXPECT_EQ(*itr, *itr_move);

    // move assign
    BoostSet set_move1 {1, 2, 3};
    set_move1 = std::move(set_move);
    EXPECT_EQ(set.size(), set_move1.size());
    for (auto itr = set.begin(), itr_move1 = set_move1.begin(), end = set.end(), end_move1 = set_move1.end();
        itr != end && itr_move1 != end_move1; ++itr, ++itr_move1)
        EXPECT_EQ(*itr, *itr_move1);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}    
