#include "segment.h"
#include "timestamp.h"

Segment::Segment(uint32_t segment_id, std::string file_path) {
    this->segment_id = segment_id;
    this->file_path = file_path;
    file = std::fstream(file_path, std::fstream::in | 
        std::fstream::out | std::fstream::binary);
    
    if (!file.is_open()) {
        // File doesn't exist, create it
        file.open(file_path, std::fstream::out | std::fstream::binary);
        file.close();
        file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::binary);
    }
}

Result<uint32_t> Segment::set(std::string key, std::string value) {
    uint32_t key_size = static_cast<uint32_t>(key.size());
    uint32_t value_size = static_cast<uint32_t>(value.size());

    Header header;
    header.timestamp = get_current_timestamp();
    header.key_size = key_size; 
    header.value_size = value_size; 
   
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(key.c_str(), key_size);
    file.write(value.c_str(), value_size);

    segment_size += sizeof(header) + key_size + value_size;
    uint32_t value_pos = segment_size - value_size;

    file.flush();

    return Result<uint32_t>{Status::Ok, value_pos};
}

Result<std::string> Segment::get(uint64_t value_pos, uint32_t value_size) {
    file.seekg(value_pos, std::ios::beg);
    if (!file) throw std::runtime_error("Failed to seek");

    std::string value(value_size, '\0');
    file.read(value.data(), value_size);

    return Result<std::string>{Status::Ok, value};
}

std::vector<Segment::Entry> Segment::getAllEntries() {
    std::vector<Entry> entries;
    file.seekg(0, std::ios::beg);
    
    uint64_t pos = 0;
    while (!file.eof()) {
        Header header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (file.eof() || !file.gcount()) break;
        
        std::string key(header.key_size, '\0');
        file.read(key.data(), header.key_size);
        
        uint32_t value_pos = pos + sizeof(header) + header.key_size;
        
        // Skip the value
        file.seekg(header.value_size, std::ios::cur);
        
        entries.push_back({key, header.value_size, value_pos, header.timestamp});
        pos += sizeof(header) + header.key_size + header.value_size;
    }
    
    return entries;
}