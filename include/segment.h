#pragma once

#include "status.h"

#include <string>
#include <fstream>
#include <cstdint>
#include <vector>

// static constexpr uint64_t MAX_SEGMENT_SIZE = 1ULL * 1024 * 1024 * 1024; // 1GB
static constexpr uint64_t MAX_SEGMENT_SIZE = 50;

class Segment {
private:
    std::string file_path;
    std::ofstream file;
    uint32_t segment_id;

    struct Header {
        uint32_t timestamp;
        uint32_t key_size;
        uint32_t value_size;
    };

public:
    struct Entry {
        std::string key;
        uint32_t value_size;
        uint32_t value_pos;
        uint32_t timestamp;
    };

    Segment(uint32_t segment_id, std::string file_path);

    uint32_t get_id() const {return segment_id;};
    
    // writes entry to log, returns offset position for value
    Result<uint32_t> set(std::string key, std::string value);
    
    // retrieves value from file at a given offset
    Result<std::string> get(uint32_t value_pos, uint32_t value_size);
    
    // retrieves all entries from the segment
    std::vector<Entry> get_all_entries() const;

    uint32_t get_size();

    bool can_write(uint64_t incoming_entry_size) {
        return (this->get_size() + sizeof(Header) + incoming_entry_size) <= MAX_SEGMENT_SIZE;
    }

    void remove_permanently();
};
