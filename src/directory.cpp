#include "directory.h"
#include <filesystem>
#include <vector>

void Directory::setupDirectory(const std::string &workPath) {
    this->workPath = fs::canonical(workPath);
    setDirectoryName();
    refreshDirectoryContents();
    formatDir();
    clearSearchResults();
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
    selection = 0;
    hideDots = true;
    nameAsc = true;
    chosenFoundEntryIdx = -1;
    this->relativePath = relativePath;
    homePath = fs::canonical(getenv("HOME"));
    setupDirectory(workPath);
}

int Directory::size() const {
    return dirSize;
}

bool Directory::empty() const {
    return dirSize < 0;
}

fs::path Directory::getSelectedFilePathString() const {
    return contents[selection].path().string();
}

int Directory::getSelection() const {
    return selection;
}

fs::path Directory::getPath() const {
    return workPath;
}

void Directory::toggleRelativePath() {
    relativePath = !relativePath;
    setDirectoryName();
}

void Directory::toggleDots() {
    hideDots = !hideDots;
    if (!hideDots) {
        refreshDirectoryContents();
    }
    formatDir();
}

bool Directory::isSelectionAnImage() {
    if (dirSize > 0) {
        return isAnImage(selection);
    }
    return false;
}

bool Directory::isAnImage(int idx) {
    return count(imgExtensions.begin(), imgExtensions.end(), contents[idx].path().extension());
}

int Directory::findIdxOfEntry(const fs::path &path) {
    for(int i = 0; i < size(); i++) {
        if (contents[i].path() == path) {
            return i;
        }
    }
    return -1;
}

void Directory::sortContentsByName(bool ascending) {
    nameAsc = ascending;
    formatDir();
}

bool Directory::goUpDirectory() {
    fs::path directoryOfEntry = workPath;
    if (workPath.has_parent_path()) {
        if (dirSize > 0) {
            childDirectoryOfEntry = contents[selection].path();
        }
        setupDirectory(fs::canonical(workPath / ".."));
        int foundParentIdx = findIdxOfEntry(directoryOfEntry);
        if (foundParentIdx > 0) {
            selection = foundParentIdx;
        }
        else {
            selection = 0;
        }
        return true;
    }
    return false;
}

bool Directory::goIntoDirectory() {
    if (size() > 0) {
        fs::directory_entry currentEntry = contents[selection];
        if (currentEntry.is_directory()) {
            setupDirectory(fs::canonical(currentEntry.path()));
            if(!childDirectoryOfEntry.empty()) {
                int foundChildIdx = findIdxOfEntry(childDirectoryOfEntry);
                if (foundChildIdx > 0) {
                    selection = foundChildIdx;
                    return true;
                }
            }
            selection = 0;
            return true;
        }
    }
    return false;
}

void Directory::refreshDirectoryContents() {
    std::vector<fs::directory_entry> res;
    for (const auto &entry : fs::directory_iterator(workPath)) {
        res.push_back(entry);
    }
    contents = res;
    dirSize = res.size();
    if (selection > dirSize - 1) {
        selection = dirSize - 1;
    }
}

void Directory::formatDir() {
    if (nameAsc) {
        std::sort(contents.begin(), contents.end(), caseInsensitiveCompare);
    }
    if (!nameAsc) {
        std::sort(contents.begin(), contents.end(), caseInsensitiveCompareDesc);
    }
    if (hideDots) {
        for (auto it = contents.begin(); it != contents.end();) {
            std::string fileName = it->path().filename().string();
            if (fileName[0] == '.') {
                it = contents.erase(it);
            } else {
                ++it;
            }
        }
        dirSize = contents.size();
        if (selection > dirSize - 1) {
            selection = dirSize - 1;
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

int Directory::findAllEntriesInDirectory(const std::string &str) {
    clearSearchResults();
    for (int i = 0; i < size(); i++) {
        if (inString(contents[i].path().filename().string(), str)) {
            foundEntries.push_back(i);
        }
    }
    chosenFoundEntryIdx = (!foundEntries.empty()) ? 0 : -1;
    return foundEntries.size();
}

void Directory::clearSearchResults() {
    foundEntries.clear();
    chosenFoundEntryIdx = -1;
}
