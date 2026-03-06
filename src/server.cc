#include "bitbarrel.h"

#include <string>
#include <iostream>
#include <sstream>


int main()
{
    BitBarrel bit_barrel("data");

    while (true) {
        std::string input;
        getline(std::cin, input);

        std::string op;
        std::istringstream iss(input);
        iss >> op;

        if (op == "set") {
            std::string key, value;
            iss >> key >> value;
            bit_barrel.set(key, value);
        } else if (op == "get") {
            std::string key;
            iss >> key;
            
            Result<std::string> res = bit_barrel.get(key);

            if (res.isOk()) {
                std::cout << res.value.value() << "\n";
            }
        }
    }
}
