#include "directory.h"
#include <filesystem>
#include <vector>

void Directory::setupDirectory(const std::string &workPath) {
    this->workPath = fs::canonical(workPath);
    setDirectoryName();
    this->refreshDirectoryContents();
    this->formatDir();
    this->chosenFoundEntryIdx = -1;
    this->foundEntries.clear();
}

void Directory::setDirectoryName() {
    if (relativePath && workPath != homePath.parent_path() && workPath != homePath.parent_path().parent_path()) {
        directoryName = std::string("~" + workPath.string().erase(0, homePath.string().length()));
    }
    else {
        directoryName = workPath;
    }
}

bool Directory::inString(std::string haystack, std::string needle) {
    std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
    std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

    return haystack.find(needle) != std::string::npos;
}

Directory::Directory() {}

Directory::Directory(const std::string &workPath, bool relativePath) {
    this->selection = 0;
    this->hideDots = true;
    this->nameAsc = true;
    this->chosenFoundEntryIdx = -1;
    this->relativePath = relativePath;
    this->homePath = fs::canonical(getenv("HOME"));
    this->setupDirectory(workPath);
}

int Directory::size() {
    return this->dirSize;
}

bool Directory::isSelectionAnImage() {
    if (this->dirSize > 0) {
        return count(imgExtensions.begin(), imgExtensions.end(), this->contents[selection].path().extension());
    }
    return false;
}

bool Directory::isAnImage(int idx) {
    return count(imgExtensions.begin(), imgExtensions.end(), this->contents[idx].path().extension());
}

int Directory::findIdxOfEntry(const fs::path &path) {
    for(int i = 0; i < this->size(); i++) {
        if (this->contents[i].path() == path) {
            return i;
        }
    }
    return -1;
}

bool Directory::goUpDirectory() {
    fs::path directoryOfEntry = this->workPath;
    if (this->workPath.has_parent_path()) {
        if (this->dirSize > 0) {
            this->childDirectoryOfEntry = this->contents[selection].path();
        }
        this->setupDirectory(fs::canonical(this->workPath / ".."));
        int foundParentIdx = this->findIdxOfEntry(directoryOfEntry);
        if (foundParentIdx > 0) {
            this->selection = foundParentIdx;
        }
        else {
            this->selection = 0;
        }
        return true;
    }
    return false;
}

bool Directory::goIntoDirectory() {
    if (this->size() > 0) {
        fs::directory_entry currentEntry = this->contents[this->selection];
        if (currentEntry.is_directory()) {
            this->setupDirectory(fs::canonical(currentEntry.path()));
            if(!this->childDirectoryOfEntry.empty()) {
                int foundChildIdx = this->findIdxOfEntry(this->childDirectoryOfEntry);
                if (foundChildIdx > 0) {
                    this->selection = foundChildIdx;
                    return true;
                }
            }
            this->selection = 0;
            return true;
        }
    }
    return false;
}

void Directory::refreshDirectoryContents() {
    std::vector<fs::directory_entry> res;
    for (const auto &entry : fs::directory_iterator(this->workPath)) {
        res.push_back(entry);
    }
    this->contents = res;
    this->dirSize = res.size();
    if (this->selection > this->dirSize - 1) {
        this->selection = this->dirSize - 1;
    }
}

void Directory::formatDir() {
    if (this->nameAsc) {
        std::sort(this->contents.begin(), this->contents.end(), caseInsensitiveCompare);
    }
    if (!this->nameAsc) {
        std::sort(this->contents.begin(), this->contents.end(), caseInsensitiveCompareDesc);
    }
    if (this->hideDots) {
        for (auto it = this->contents.begin(); it != this->contents.end();) {
            std::string fileName = it->path().filename().string();
            if (fileName[0] == '.') {
                it = this->contents.erase(it);
            } else {
                ++it;
            }
        }
        this->dirSize = contents.size();
        if (this->selection > this->dirSize - 1) {
            this->selection = this->dirSize - 1;
        }
    }
}

std::vector<fs::path> Directory::getAllImages() {
    std::vector<fs::path> images;
    for (const auto &entry : fs::directory_iterator(workPath)) {
        if (count(imgExtensions.begin(), imgExtensions.end(), entry.path().extension())) {
            images.push_back(entry.path());
        }
    }
    return images;
}

void Directory::findAllEntriesInDirectory(const std::string &str) {
    foundEntries.clear();
    for (int i = 0; i < size(); i++) {
        if (inString(this->contents[i].path().filename().string(), str)) {
            foundEntries.push_back(i);
        }
    }
    chosenFoundEntryIdx = (foundEntries.size() > 0) ? 0 : -1;
}
