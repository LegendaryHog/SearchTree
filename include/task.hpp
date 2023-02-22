#pragma once
#include <iostream>
#include <vector>
#include "search_tree.hpp"

namespace Task
{

enum class Action
{
    InsertKey,
    NumMinimum,
    CountLess,
};

struct Input
{
    Action act_;
    int  key_;
};

std::vector<Input> scan_input();
}