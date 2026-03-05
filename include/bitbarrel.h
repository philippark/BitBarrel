#pragma once

#include "segment.h"
#include "status.h"

#include <list>
#include <unordered_map>
#include <string>
#include <cstdint>

class BitBarrel {
private:
    struct KeyDirEntry {
        uint64_t segment_id;
        uint32_t value_size;
        uint32_t value_pos;
        uint32_t timestamp;
    };

    std::list<Segment*> data_store;
    Segment* active_segment;
    std::unordered_map<std::string, KeyDirEntry> key_dir;

public:
    BitBarrel();
    Status set(std::string key, std::string value);
    Result<std::string> get(std::string key);

};
