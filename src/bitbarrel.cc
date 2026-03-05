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
    dataStore.push_back(current_segment);
    active_segment = current_segment; 
}

void BitBarrel::set(std::string key, std::string value) {
    uint64_t offset = active_segment->set(key, value); 
    keydir[key] = std::make_tuple(active_segment->get_id(), offset, value.size());
}

std::string BitBarrel::get(std::string key) {
    auto segment_info = keydir[key];
    uint64_t segment_id = std::get<0>(segment_info);
    uint64_t offset = std::get<1>(segment_info);
    size_t size = std::get<2>(segment_info);

    for (auto it = dataStore.rbegin(); it != dataStore.rend(); ++it) {
        if ((*it)->get_id() == segment_id) {
            return (*it)->get(offset, size);
        }
    }

    return "none found bucko";
}