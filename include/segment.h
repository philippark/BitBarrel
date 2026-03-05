#pragma once

#include "status.h"

#include <string>
#include <fstream>
#include <cstdint>

class Segment {
private:
    std::string file_path;
    std::fstream file;
    uint64_t segment_id;
    uint64_t segment_size = 0;

    struct Header {
        uint32_t timestamp;
        uint32_t key_size;
        uint32_t value_size;
    };

public:
    Segment(uint64_t segment_id, std::string file_path);

    uint64_t get_id() {return segment_id;};
    
    // writes entry to log, returns offset position for value
    Result<uint32_t> set(std::string key, std::string value);
    
    // retrieves value from file at a given offset
    Result<std::string> get(uint64_t value_pos, uint32_t value_size);
};
