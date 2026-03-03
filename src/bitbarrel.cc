#include "bitbarrel.h"

#include <iostream>
#include <chrono>
#include <ctime>

uint64_t get_current_timestamp() {
    auto now = std::chrono::steady_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()
    ).count();

    return ts;
}

BitBarrel::BitBarrel() {
    uint64_t segment_id = get_current_timestamp();
    std::string file_path = "segment1";

    Segment* current_segment = new Segment(segment_id, file_path);
    dataStore.push_back(current_segment);
    active_segment = current_segment; 
}

void BitBarrel::set(std::string key, std::string value) {
}

std::string BitBarrel::get(std::string key) {
    return "oi, here's your get\n";
}