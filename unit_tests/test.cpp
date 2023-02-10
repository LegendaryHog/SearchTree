#include <gtest/gtest.h>
#define DEBUG
#include <search_tree.hpp>

using namespace Container;

TEST(Tree, Ctors)
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


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
