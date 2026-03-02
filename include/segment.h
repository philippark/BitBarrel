#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <unordered_map>
#include <string>
#include <fstream>
#include <cstdint>

class Segment {
private:
    std::unordered_map<std::string, int64_t> keydir;
    std::ofstream wfile;
    std::ifstream rfile;
    uint64_t segment_size = 0;

public:
    Segment(std::string file_name);
    void set(std::string key, std::string value);
    std::string get(std::string key);
};

#endif