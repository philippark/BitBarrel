#include "segment.h"
#include "timestamp.h"

Segment::Segment(uint64_t segment_id, std::string file_path) {
    this->segment_id = segment_id;
    this->file_path = file_path;
    file = std::fstream(file_path, std::fstream::in | 
        std::fstream::out | std::fstream::binary | std::fstream::trunc);
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

    return Result<uint32_t>{Status::Ok, value_pos};
}

Result<std::string> Segment::get(uint64_t value_pos, uint32_t value_size) {
    file.seekg(value_pos, std::ios::beg);
    if (!file) throw std::runtime_error("Failed to seek");

    std::string value(value_size, '\0');
    file.read(value.data(), value_size);

    return Result<std::string>{Status::Ok, value};
}