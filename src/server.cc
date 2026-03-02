#include "server.h"

#include <string>
#include <iostream>
#include <sstream>

Server::Server() {
    dataStore.push_back(new Segment("test1"));
}

void Server::run() {
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