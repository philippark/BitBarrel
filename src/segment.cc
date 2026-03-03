#include "segment.h"

Segment::Segment(uint64_t segment_id, std::string file_path) {
    this->segment_id = segment_id;
    this->file_path = file_path;
    file = std::fstream(file_path, std::fstream::in | 
        std::fstream::out | std::fstream::binary | std::fstream::trunc);
}

uint64_t Segment::set(std::string value) {
    size_t size = value.size();
    file.write(value.c_str(), size); 

    segment_size += size;

    return segment_size - size;
}

std::string Segment::get(uint64_t offset, size_t size) {
    file.seekg(offset, std::ios::beg);
    if (!file) throw std::runtime_error("Failed to seek");

    std::string value(size, '\0');
    file.read(value.data(), size);

    return value;
}
