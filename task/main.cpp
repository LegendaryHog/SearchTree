#include "task.hpp"

using namespace Task;

int main()
{
    std::cerr << "scan start\n";
    auto requests = scan_input();
    std::cerr << "scan end\n";
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