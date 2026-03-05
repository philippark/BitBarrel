#pragma once
#include <cstdint>
#include <chrono>

inline uint32_t get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    uint32_t timestamp = static_cast<uint32_t>(
        std::chrono::system_clock::to_time_t(now)
    );
    return timestamp;
}