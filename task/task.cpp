#include "task.hpp"

using namespace Task;

std::vector<Input> Task::scan_input()
{
    char req_type {};
    int key = 0;
    Action act;

    std::vector<Input> requests;
    while (std::cin >> req_type >> key)
    {
        switch (req_type)
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
        requests.push_back(Input{act, key});
    }
    return requests;
}