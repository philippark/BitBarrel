#include "bitbarrel.h"
#include "timestamp.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>

BitBarrel::BitBarrel() {
    // Use timestamp as a monotonic counter for the id 
    uint64_t segment_id = get_current_timestamp();
    
    // Create data directory if it doesn't exist
    std::filesystem::create_directory("data");
    std::string file_path = "data/" + std::to_string(segment_id);

    Segment* current_segment = new Segment(segment_id, file_path);
    data_store.push_back(current_segment);
    active_segment = current_segment; 
}

void BitBarrel::set(std::string key, std::string value) {
    uint32_t value_pos = active_segment->set(key, value); 
    uint32_t timestamp = get_current_timestamp();

    KeyDirEntry kde {active_segment->get_id(),
        static_cast<uint32_t>(sizeof(value)), value_pos, timestamp};
    key_dir[key] = kde;
}

std::string BitBarrel::get(std::string key) {
    if (key_dir.find(key) == key_dir.end()) {
        return "key not found"; // TODO: pack this in an error or status struct
    }

    KeyDirEntry kde = key_dir[key];

    for (auto it = data_store.rbegin(); it != data_store.rend(); ++it) {
        if ((*it)->get_id() == kde.segment_id) {
            return (*it)->get(kde.value_pos, kde.value_size);
        }
    }

    return "none found bucko";
}