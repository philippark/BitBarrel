#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <string>
#include <fstream>
#include <cstdint>

class Segment {
private:
    std::string file_path;
    std::fstream file;
    int segment_id;
    uint64_t segment_size = 0;

public:
    Segment(int segment_id, std::string file_path);
    
    // writes a value to file, returns the offset it's stored at
    uint64_t set(std::string value);
    
    // retrieves value from file at a given offset
    std::string get(uint64_t offset, size_t size);
};

#endif