#include "directoryPager.h"

DirectoryPager& DirectoryPager::operator=(const DirectoryPager& other) {
    Pager::operator=(other);
    directoryHistory = other.directoryHistory;
    return *this;
}

DirectoryPager::DirectoryPager(int height, int numberOfElements) :
    Pager(height, numberOfElements)
{ }

DirectoryPager::DirectoryPager(const DirectoryPager& other) {
    directoryHistory = other.directoryHistory;
}

void DirectoryPager::onDirectoryChanged(const IDirectoryController* directory) {
    setNumberOfElements(directory->getNumberOfEntries());

    // save path for future lookup when going up a directory
    //TODO: figure out a way to save idx for directory we came from
    directoryHistory[directory->getWorkpath().parent_path()] = directory->getWorkpath();

    int newIdx = 0;
    if (directoryHistory.count(directory->getWorkpath())) {
        newIdx = std::max(newIdx, directory->
                          findIdxOfEntry(directoryHistory.at(directory->getWorkpath())));
    }

    jumpToIdx(newIdx);
}

void DirectoryPager::onDirectoryEntriesChanged(const IDirectoryController* directory) {
    setNumberOfElements(directory->getNumberOfEntries());
    focusScrolling();
}

void DirectoryPager::onSearchChange(int idx) {
    jumpToIdx(idx);
}
