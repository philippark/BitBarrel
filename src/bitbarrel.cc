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

Status BitBarrel::set(std::string key, std::string value) {
    Result<uint32_t> res = active_segment->set(key, value); 
    if (!res.isOk()) {
        return Status::Error;
    }
    uint32_t value_pos = res.value.value();

    // update key directory
    uint32_t timestamp = get_current_timestamp();
    KeyDirEntry kde {active_segment->get_id(),
        static_cast<uint32_t>(sizeof(value)), value_pos, timestamp};
    key_dir[key] = kde;

    return Status::Ok;
}

Result<std::string> BitBarrel::get(std::string key) {
    if (key_dir.find(key) == key_dir.end()) {
        return Result<std::string>{Status::NotFound};
    }

    KeyDirEntry kde = key_dir[key];

    for (auto it = data_store.rbegin(); it != data_store.rend(); ++it) {
        if ((*it)->get_id() == kde.segment_id) {
            Result<std::string> res = (*it)->get(kde.value_pos, kde.value_size);
            if (!res.isOk()) {
                return Result<std::string>{Status::Error};
            }
            return Result<std::string>{Status::Ok, res.value.value()};
        }
    }

    return Result<std::string>{Status::NotFound};
}