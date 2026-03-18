#pragma once

#include "segment.h"
#include "status.h"

#include <list>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <atomic>
#include <thread>

class BitBarrel {
public:
    BitBarrel(const std::string& dir_name);
    ~BitBarrel();
    Status set(std::string key, std::string value);
    Result<std::string> get(std::string key);

    void compact();

private:
    struct KeyDirEntry {
        uint32_t segment_id;
        uint32_t value_size;
        uint32_t value_pos;
        uint32_t timestamp;
    };

    std::list<std::unique_ptr<Segment>> data_store;
    Segment* active_segment;
    std::unordered_map<std::string, KeyDirEntry> key_dir;
    std::string dir_name;

    std::shared_mutex rw_lock; // read-write lock
    std::atomic<bool> compaction_stop {false};
    std::thread compaction_thread;

    /*Helpers*/
    // loads key directory entries from a segment
    void load_key_dir_from_segment(const Segment& segment);

    // Creates and returns a new segment
    std::unique_ptr<Segment> create_segment();

    void compaction_worker();
};
