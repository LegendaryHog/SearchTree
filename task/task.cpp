#include "task.hpp"

using namespace Task;

std::vector<Input> Task::scan_input()
{
    char query_type {};
    int key = 0;
    Action act;

    std::vector<Input> requests;
    while (std::cin >> query_type >> key)
    {
        std::cerr << "in while\n";
        switch (query_type)
        {
            case 'k':
                act = Action::InsertKey;
                break;
            case 'm':
                act = Action::NumMinimum;
                break;
            case 'n':
                act = Action::CountLess;
                break;
            default:
                break;
        }
        std::cerr << "push\n";
        requests.push_back(Input{act, key});
    }
    std::cerr << "after while\n";
    return requests;
}