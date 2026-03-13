#include "segment.h"
#include "timestamp.h"

#include <cstring>
#include <filesystem>

Segment::Segment(uint32_t segment_id, std::string file_path) {
    this->file_path = file_path;
    this->segment_id = segment_id;

    file.open(file_path, std::ios::out | std::ios::binary | std::ios::app);
    if (!file) throw std::runtime_error("Cannot open file: " + file_path);
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

    std::streampos value_pos = file.tellp();
    if (value_pos == std::streampos(-1)) {
        throw std::runtime_error("tellp() failed\n");
    }

    file.write(value.c_str(), value_size);

    if (file.bad()) {
        throw std::runtime_error(
        std::string("Unrecoverable write error: ") + std::strerror(errno));
    }
    if (file.fail()) {
        throw std::runtime_error("Write failed (partial write or logic error)");
    }

    file.flush();
    if (!file) {
        throw std::runtime_error(
        std::string("Flush failed: ") + std::strerror(errno));
    }

    return Result<uint32_t>{Status::Ok, 
        static_cast<uint32_t>(static_cast<std::streamoff>(value_pos))};
}

Result<std::string> Segment::get(uint32_t value_pos, uint32_t value_size) {
    std::ifstream read_file(file_path, std::ios::in | std::ios::binary);
    if (!read_file) throw std::runtime_error("Cannot open read file: " + file_path);
    
    read_file.seekg(value_pos, std::ios::beg);
    if (!read_file) throw std::runtime_error("Failed to seek");

    std::string value(value_size, '\0');
    read_file.read(value.data(), value_size);

    return Result<std::string>{Status::Ok, value};
}

std::vector<Segment::Entry> Segment::get_all_entries() const {
    std::vector<Entry> entries;

    std::ifstream read_file(file_path, std::ios::in | std::ios::binary);
    if (!read_file) throw std::runtime_error("Cannot open read file: " + file_path);
    
    read_file.seekg(0, std::ios::beg);
    
    while (true) {
        Header header;
        if (!read_file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            break;
        }
        
        std::string key(header.key_size, '\0');
        if (!read_file.read(key.data(), header.key_size)) {
            break;
        }
        
        std::streampos value_offset = read_file.tellg();
        uint32_t value_pos = static_cast<std::streamoff>(value_offset);
        // Skip the value
        read_file.seekg(header.value_size, std::ios::cur);
        
        entries.push_back({key, header.value_size, value_pos, header.timestamp});
    }
    
    return entries;
}

uint32_t Segment::get_size() {
    std::error_code ec;
    auto size = std::filesystem::file_size(file_path, ec);
    if (ec) {
        throw std::runtime_error("get_size failed: " + ec.message());
    }
    return size;
}
