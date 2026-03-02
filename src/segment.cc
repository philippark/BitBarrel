#include "segment.h"

Segment::Segment(std::string file_name) {
    wfile = std::ofstream(file_name, std::ofstream::binary);
    rfile = std::ifstream(file_name, std::ifstream::binary);
}

void Segment::set(std::string key, std::string value) {
    size_t size = value.size();
    wfile.write(reinterpret_cast<char *>(&size), sizeof(size));
    wfile.write(value.c_str(), size); 
    wfile.flush();

    keydir[key] = segment_size;
    segment_size += sizeof(size) + size;
}

std::string Segment::get(std::string key) {
    int64_t offset = keydir[key];

    rfile.seekg(offset, std::ios::beg);
    if (!rfile) throw std::runtime_error("Failed to seek");

    size_t length = 0;
    rfile.read(reinterpret_cast<char*>(&length), sizeof(length));
    if (!rfile) throw std::runtime_error("Failed to read length");

    std::string value(length, '\0');
    rfile.read(value.data(), length);

    return value;
}
