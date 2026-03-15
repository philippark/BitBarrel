#include "bitbarrel.h"
#include "timestamp.h"
#include "scan_dir.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <fstream>

void BitBarrel::load_key_dir_from_segment(const Segment& segment) {
    auto entries = segment.get_all_entries();
    
    for (const auto& entry : entries) {
        // ignore if earlier entry
        if (key_dir.find(entry.key) != key_dir.end()) {
            if (key_dir[entry.key].timestamp > entry.timestamp) {
                continue;
            }
        }
        
        KeyDirEntry kde{segment.get_id(), entry.value_size, 
                        entry.value_pos, entry.timestamp};
        key_dir[entry.key] = kde;
    }
}

std::unique_ptr<Segment> BitBarrel::create_segment() {
    uint32_t id = get_current_timestamp();
    std::string file_path = dir_name + "/" + std::to_string(id);
    return std::make_unique<Segment>(id, file_path);
}

BitBarrel::BitBarrel(const std::string& dir_name) {
    // Create directory if it doesn't exist
    std::filesystem::create_directory(dir_name);

    this->dir_name = dir_name;

    auto files = scan_dir(dir_name);
    std::vector<uint64_t> segment_ids;

    // rebuild key dir and in-mem list of segments
    for (auto& file : files) {
        uint32_t segment_id = static_cast<uint32_t>(std::stoi(file));
        segment_ids.push_back(segment_id);
    }

    std::sort(segment_ids.begin(), segment_ids.end());

    for (auto segment_id : segment_ids) {
        auto segment = std::make_unique<Segment>(segment_id, dir_name + "/" + std::to_string(segment_id));
        load_key_dir_from_segment(*segment);
        data_store.push_back(std::move(segment));
    }
    
    if (data_store.empty()) {
        // Create initial segment
        auto segment = create_segment();
        data_store.push_back(std::move(segment));
    } 

    active_segment = data_store.back().get();
}

Status BitBarrel::set(std::string key, std::string value) {
    std::cout << "active segment size: " << active_segment->get_size() << "\n";
    if (active_segment->is_full(key.size() + value.size())) {
        auto new_segment = create_segment();
        data_store.push_back(std::move(new_segment));
        active_segment = data_store.back().get();
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