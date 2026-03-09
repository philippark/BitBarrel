#include "bitbarrel.h"
#include "timestamp.h"
#include "scan_dir.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <fstream>

void BitBarrel::load_key_dir_from_segment(Segment *segment) {
    auto entries = segment->get_all_entries();
    
    for (const auto& entry : entries) {
        KeyDirEntry kde{segment->get_id(), entry.value_size, 
                        entry.value_pos, entry.timestamp};
        key_dir[entry.key] = kde;
    }
}

Segment* BitBarrel::create_segment() {
    uint32_t id = get_current_timestamp();
    std::string file_path = dir_name + "/" + std::to_string(id);
    return new Segment(id, file_path);
}

BitBarrel::BitBarrel(const std::string& dir_name) {
    // Create directory if it doesn't exist
    // TODO: maybe move this somewhere else, and also handle errors
    if (std::filesystem::create_directory(dir_name)) {
        std::cout << "created directory\n";
    } else {
        std::cout << "directory already exists or failed to create\n";
    }

    this->dir_name = dir_name;

    auto files = scan_dir(dir_name);
    std::sort(files.begin(), files.end());

    // rebuild key dir and in-mem list of segments
    for (auto& file : files) {
        uint32_t segment_id = static_cast<uint32_t>(std::stoi(file));
        Segment *segment = new Segment(segment_id, dir_name + "/" + file);
        data_store.push_back(segment);
        load_key_dir_from_segment(segment);
    }

    if (data_store.empty()) {
        // Create initial segment
        Segment *segment = create_segment(); 
        data_store.push_back(segment);
    } 

    active_segment = data_store.back();
}

Status BitBarrel::set(std::string key, std::string value) {
    std::cout << "active segment size: " << active_segment->get_size() << "\n";
    if (active_segment->is_full(key.size() + value.size())) {
        active_segment = create_segment();
        data_store.push_back(active_segment); 
    }

    Result<uint32_t> res = active_segment->set(key, value); 
    if (!res.isOk()) {
        return Status::Error;
    }
    uint32_t value_pos = res.value.value();

    // update key directory
    uint32_t timestamp = get_current_timestamp();
    KeyDirEntry kde {active_segment->get_id(),
        static_cast<uint32_t>(value.size()), value_pos, timestamp};
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