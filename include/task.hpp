#pragma once
#include <iostream>
#include <vector>
#include "boost_set.hpp"

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