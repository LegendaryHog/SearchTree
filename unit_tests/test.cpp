#include <gtest/gtest.h>
#define DEBUG
#include <search_tree.hpp>

using namespace Container;

TEST(Tree, default_ctor_and_one_key_ctor)
{
    SearchTree tree0 {};
    SearchTree tree1 {15};
}

TEST(Tree, insert)
{
    SearchTree tree {};
    for (auto i = 0; i < 40; i++)
    {
        if (i % 2)
            tree.insert(i);
        else
            tree.insert(i + 20);
        tree.debug_graph_dump("dump" + std::to_string(i));
    }
}

TEST(Tree, init_list_and_cont_ctor)
{
    SearchTree tree0 {1, 4, -2, 8, 9, 20, 10, 34, 17};
    std::array<int, 9> arr {1, 4, -2, 8, 9, 20, 10, 34, 17};
    SearchTree tree1 (arr.cbegin(), arr.cend());
}

TEST(Tree, Iterators)
{
    static_assert(std::bidirectional_iterator<SearchTree<>::Iterator>);
    static_assert(std::bidirectional_iterator<SearchTree<>::ConstIterator>);

    std::array<int, 8> arr {-2, 1, 2, 4, 5, 6, 7, 9};

    SearchTree tree {1, 2, 4, 7, 9, -2, 5, 6};
    std::size_t i = 0;
    for (auto x: tree)
        EXPECT_EQ(x, arr[i++]);

    const SearchTree ctree {1, 2, 4, 7, 9, -2, 5, 6};
    i = 0;
    for (auto x: tree)
        EXPECT_EQ(x, arr[i++]);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
