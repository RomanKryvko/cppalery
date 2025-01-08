#pragma once

#include <vector>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

const std::vector<std::string> imgExtensions = {".jpg", ".webp", ".png", ".gif", ".jpeg"};

class Directory {
private:
    std::vector<fs::directory_entry> contents;
    int dirSize;
    fs::path workPath;

public:
    Directory();
    Directory(const Directory& other);
    Directory& operator=(const Directory& other);

    Directory(const std::string &workPath);

    void setWorkpath(const std::string &workPath);

    int size() const;

    const std::vector<fs::directory_entry>& getEntries() const;

    const fs::path& getWorkpath() const;

    bool empty() const;

    void refreshDirectoryContents();
};
