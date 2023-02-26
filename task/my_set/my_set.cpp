#include "set.hpp"
#include <iostream>

int main()
{
    Container::Set set {};
    char req_type {};
    int num = 0;
    
    while (std::cin >> req_type >> num)
        switch (req_type)
        {
            case 'k':
                set.insert(num);
                break;
            case 'm':
                std::cout << *std::next(set.begin(), num - 1) << " ";
                break;
            case 'n':
                std::cout << std::distance(set.begin(), set.lower_bound(num)) << " ";
                break;
            default:
                break;
        }
    std::cout << std::endl;
    return 0;
}