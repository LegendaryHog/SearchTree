#include "task.hpp"

using namespace Task;

int main()
{
    auto requests = scan_input();
    Container::RBSearchTree set {};
    for (auto req: requests)
        switch (req.act_)
        {
            case Action::InsertKey:
                set.insert(req.key_);
                break;
            case Action::CountLess:
                std::cout << std::distance(set.begin(), set.lower_bound(req.key_)) << " ";
                break;
            case Action::NumMinimum:
                std::cout << *std::next(set.begin(), req.key_ - 1) << " ";
                break;
            default:
                std::cout << "error" << std::endl;
                return 0;
        }
    std::cout << std::endl;
    return 0;
}