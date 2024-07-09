#include "directory.h"

void Directory::setupDirectory(const std::string &workPath) {
    this->workPath = fs::canonical(workPath);
    this->directoryName = this->workPath;
    this->refreshDirectoryContents();
    this->formatDir();
    this->chosenFoundEntryIdx = -1;
    this->foundEntries.clear();
}

bool Directory::inString(std::string haystack, std::string needle) {
    std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
    std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

    return haystack.find(needle) != std::string::npos;
}

Directory::Directory() {
    this->selection = 0;
    this->hideDots = true;
    this->nameAsc = true;
    this->chosenFoundEntryIdx = -1;
}

Directory::Directory(const std::string &workPath) {
    this->selection = 0;
    this->hideDots = true;
    this->nameAsc = true;
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
    std::vector<int> res;
    char firstChar = std::tolower(str[0]);
    int high = this->dirSize - 1;
    int low = 0;
    this->nameAsc = true;
    this->formatDir();
    while (low <= high) {
        int mid = low + (high - low) / 2;
        std::string entryStr = this->contents[mid].path().filename().string();
        char currentChar = std::tolower(entryStr[0]);
        if (currentChar == firstChar) {
            for (int i = low; i <= high; i++) {
                entryStr = this->contents[i].path().filename().string();
                if (inString(entryStr, str)) {
                    res.push_back(i);
                }
            }
            break;
        }
        if (currentChar > firstChar) {
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }
   this->foundEntries = res;
   if (res.empty()) {
       this->chosenFoundEntryIdx = -1;
   }
   else {
       this->chosenFoundEntryIdx = 0;
   }
}
