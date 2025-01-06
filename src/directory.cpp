#include "directory.h"

void Directory::setWorkpath(const std::string &workPath) {
    this->workPath = fs::canonical(workPath);
    refreshDirectoryContents();
}

Directory::Directory() = default;

Directory::Directory(const std::string &workPath) {
    setWorkpath(workPath);
}

Directory::Directory(const Directory& other) {
    contents = other.contents;
    dirSize = other.dirSize;
    workPath = other.workPath;
}

Directory& Directory::operator=(const Directory& other) {
    if (this == &other)
        return *this;

    contents = other.contents;
    dirSize = other.dirSize;
    workPath = other.workPath;

    return *this;
}

int Directory::size() const {
    return dirSize;
}

const std::vector<fs::directory_entry>& Directory::getEntries() const {
    return contents;
}

const fs::path& Directory::getWorkpath() const {
    return workPath;
}

bool Directory::empty() const {
    return dirSize <= 0;
}

void Directory::refreshDirectoryContents() {
    std::vector<fs::directory_entry> res;
    for (const auto &entry : fs::directory_iterator(workPath)) {
        res.push_back(entry);
    }
    contents = res;
    dirSize = res.size();
}
