#include "cppalery.h"
#include "directory.h"

void Directory::setupDirectory(const std::string &workPath) {
    this->workPath = fs::canonical(workPath);
    this->directoryName = this->workPath;
    this->refreshDirectoryContents();
    this->formatDir();
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
    return count(imgExtensions.begin(), imgExtensions.end(), this->contents[selection].path().extension());
}

int Directory::findIdxOfEntry(const fs::path &path) {
    for(int i = 0; i < this->size() - 1; i++) {
        if (this->contents[i].path() == path) {
            return i;
        }
    }
    return -1;
}

bool Directory::goUpDirectory() {
    if (this->workPath.has_parent_path()) {
        this->setupDirectory(fs::canonical(this->workPath / ".."));
        int foundParentIdx = this->findIdxOfEntry(this->directoryOfEntry);
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
    fs::directory_entry currentEntry = this->contents[this->selection];
    if (currentEntry.is_directory()) {
        this->directoryOfEntry = currentEntry.path();
        this->setupDirectory(fs::canonical(currentEntry.path()));
        this->selection = 0;
        return true;
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