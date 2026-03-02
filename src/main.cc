#include "segment.h"
#include "server.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>

int main()
{
    Server server;
    server.run();
    
    /*
    Segment segment("test");    

    std::vector<std::tuple<std::string, std::string>> test {
        std::make_tuple("key1", "value1"),
        std::make_tuple("key2", "value2"),
    };

    for (auto t : test) {
        std::string key = std::get<0>(t);
        std::string val = std::get<1>(t);
        segment.set(key, val);
    }

    for (auto t : test) {
        std::string key = std::get<0>(t);
        std::cout << segment.get(key) << "\n";
    }
    */

};