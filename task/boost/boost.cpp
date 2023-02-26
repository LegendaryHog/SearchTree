#include "boost_set.hpp"
#include <iostream>

int main()
{
    Container::BoostSet set {};
    char req_type {};
    int num = 0;
    
    while (std::cin >> req_type >> num)
        switch (req_type)
        {
            case 'k':
                set.insert(num);
                break;
            case 'm':
                std::cout << set[num - 1] << " ";
                break;
            case 'n':
                std::cout << (set ->* num) << " ";
                break;
            default:
                break;
        }
    std::cout << std::endl;

    return 0;
}