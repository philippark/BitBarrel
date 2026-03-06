#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> scan_dir(const std::filesystem::path& dir_path) {
    std::vector<std::string> entries;

    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        // entry.path() returns a fs::path object
        // std::cout << "* " << entry.path().filename().string();

        // Check if it's a directory or a regular file
        if (std::filesystem::is_directory(entry.status())) {
            continue;
        } 
        entries.push_back(entry.path().filename().string());
    }
    return entries;
}