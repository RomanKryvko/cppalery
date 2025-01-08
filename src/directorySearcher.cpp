#include "directorySearcher.h"

DirectorySearcher::DirectorySearcher() = default;

DirectorySearcher::DirectorySearcher(const std::shared_ptr<MessagePrinter>& messagePrinter) :
    messagePrinter(messagePrinter)
{}

DirectorySearcher::DirectorySearcher(const DirectorySearcher& other) {
    copyFromOther(other);
}

DirectorySearcher& DirectorySearcher::operator=(const DirectorySearcher& other) {
    if (this == &other) 
        return *this;

    copyFromOther(other);
    return *this;
}

void DirectorySearcher::copyFromOther(const DirectorySearcher& other) {
    messagePrinter = other.messagePrinter;
    searchTerm = other.searchTerm;
    foundEntries = other.foundEntries;
    chosenEntryIdx = other.chosenEntryIdx;
}

int DirectorySearcher::chooseNext() {
    if (!foundEntries.empty()) {
        if (chosenEntryIdx == foundEntries.size() - 1) {
            //TODO: this actually doesn't do anything at the moment,
            // as the message is overwritten while looping through foundEntries in DirectoryController.
            // This needs to be fixed
            messagePrinter->setMessage("Search hit BOTTOM, continuing at TOP");
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
            //TODO: this actually doesn't do anything at the moment,
            // as the message is overwritten while looping through foundEntries in DirectoryController.
            // This needs to be fixed
            messagePrinter->setMessage("Search hit TOP, continuing at BOTTOM");
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
