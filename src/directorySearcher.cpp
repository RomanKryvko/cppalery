#include "directorySearcher.h"

DirectorySearcher::DirectorySearcher() = default;

int DirectorySearcher::chooseNext() {
    if (!foundEntries.empty()) {
        if (chosenEntryIdx == foundEntries.size() - 1) {
            //TODO: implement sending message on wrappping
            chosenEntryIdx = 0;
            return foundEntries[chosenEntryIdx];
        }
        chosenEntryIdx++;
        return foundEntries[chosenEntryIdx];
    }
    return -1;
}

int DirectorySearcher::choosePrevious() {
    if (!foundEntries.empty()) {
        if (!chosenEntryIdx) {
            //TODO: implement sending message on wrappping
            chosenEntryIdx = foundEntries.size() - 1;
            return foundEntries[chosenEntryIdx];
        }
        chosenEntryIdx--;
        return foundEntries[chosenEntryIdx];
    }
    return -1;
}

void DirectorySearcher::setSearchTerm(const std::string& str) {
    searchTerm = str;
}

int DirectorySearcher::findString(const std::vector<const std::filesystem::directory_entry*>& directoryEntries) {
    clearSearchResults();

    if (!searchTerm.empty()) {
        for (int i = 0; i < directoryEntries.size(); i++) {
            if (inString(directoryEntries[i]->path().filename().string(), searchTerm))
                foundEntries.push_back(i);
        }

        if (!foundEntries.empty())
            chosenEntryIdx = 0;

        return foundEntries.size();
    }

    return -1;
}

void DirectorySearcher::clearSearchResults() {
    foundEntries.clear();
    chosenEntryIdx = -1;
}

int DirectorySearcher::getChosenIdx() const {
    return chosenEntryIdx;
}


const std::vector<int>& DirectorySearcher::getFoundEntries() const {
    return foundEntries;
}
