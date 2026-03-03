#include "segment.h"

#include <string>
#include <iostream>
#include <sstream>
#include <list>

std::list<Segment*> dataStore;

int main()
{
    dataStore.push_back(new Segment("segment1"));
    
    while (true) {
        std::string input;
        getline(std::cin, input);

        std::string op;
        std::istringstream iss(input);
        iss >> op;

        if (op == "set") {
            std::string key, value;
            iss >> key >> value;
            dataStore.back()->set(key, value);
        } else if (op == "get") {
            std::string key;
            iss >> key;
            std::cout << dataStore.back()->get(key) << "\n";
            
        }
    }
}
