#include "directoryController.h"
#include "directorySearcher.h"

DirectoryController::DirectoryController() = default;

DirectoryController::DirectoryController(const fs::path workpath, bool hideDots, bool sortAscending, bool useRelativePath, const std::shared_ptr<IDirectoryObserver>& observer, const std::shared_ptr<MessagePrinter>& messagePrinter) :
    directory(workpath),
    hideDots(hideDots),
    sortInAscendingOrder(sortAscending),
    useRelativePath(useRelativePath),
    observer(observer),
    messagePrinter(messagePrinter),
    directorySearcher(messagePrinter)
{
    homePath = fs::canonical(getenv("HOME"));
    doFullDirectorySetup();
}

DirectoryController::DirectoryController(const DirectoryController& other) {
    copyFromOther(other);
}

DirectoryController& DirectoryController::operator=(const DirectoryController& other) {
    if (this == &other)
        return *this;
    copyFromOther(other);
    return *this;
}

void DirectoryController::copyFromOther(const DirectoryController& other) {
    directory = other.directory;
    directorySearcher = other.directorySearcher;
    observer = other.observer;
    messagePrinter = other.messagePrinter;
    backSetter = other.backSetter;
    shownEntries = other.shownEntries;
    homePath = other.homePath;
    directoryName = other.directoryName;
    hideDots = other.hideDots;
    sortInAscendingOrder = other.sortInAscendingOrder;
    useRelativePath = other.useRelativePath;
}

void DirectoryController::doFullDirectorySetup() {
    setDirectoryName();
    refreshShownEntries();
    formatShownEntries();
    clearSearchResults();
    observer->onDirectoryChanged(this);
}

void DirectoryController::setDirectoryName() {
    if (useRelativePath && directory.getWorkpath() != homePath.parent_path() && directory.getWorkpath() != homePath.parent_path().parent_path()) {
        directoryName = std::string("~" + directory.getWorkpath().string().erase(0, homePath.string().length()));
    }
    else {
        directoryName = directory.getWorkpath();
    }
}

void DirectoryController::refreshShownEntries() {
    shownEntries.resize(directory.size());

    for (int i = 0; i < directory.size(); i++) {
        shownEntries[i] = &directory.getEntries()[i];
    }

    observer->onDirectoryEntriesChanged(this);
}

void DirectoryController::formatShownEntries() {
    sortShownEntries();
    if (hideDots) {
        hideDotEntries();
    }
}

void DirectoryController::sortShownEntries() {
    if (sortInAscendingOrder) {
        std::sort(shownEntries.begin(), shownEntries.end(), caseInsensitiveCompare);
    }
    else {
        std::sort(shownEntries.begin(), shownEntries.end(), caseInsensitiveCompareDesc);
    }
}

void DirectoryController::hideDotEntries() {
    for (auto it = shownEntries.begin(); it != shownEntries.end();) {
        if ((**it).path().filename().string()[0] == '.') {
            it = shownEntries.erase(it);
        } else {
            ++it;
        }
    }

    observer->onDirectoryEntriesChanged(this);
}

void DirectoryController::sortByAscending() {
    sortInAscendingOrder = true;
    sortShownEntries();
}

void DirectoryController::sortByDescending() {
    sortInAscendingOrder = false;
    sortShownEntries();
}

void DirectoryController::goIntoDirectory(int idx) {
    if (shownEntries.size() && shownEntries[idx]->is_directory()) {
        directory = Directory(fs::canonical(shownEntries[idx]->path()));
        doFullDirectorySetup();
        return;
    }

    backSetter->setBackground(shownEntries[idx]->path());
}

int DirectoryController::findIdxOfEntry(const fs::path& path) const {
    for(int i = 0; i < shownEntries.size(); i++) {
        if (shownEntries[i]->path() == path) {
            return i;
        }
    }
    return -1;
}

void DirectoryController::goUpDirectory() {
    const fs::path* workpath = &directory.getWorkpath();

    if (workpath->has_parent_path()) {
        directory = Directory(fs::canonical(*workpath / ".."));
        doFullDirectorySetup();
    }
}

void DirectoryController::setBackgroundSetter(const std::shared_ptr<BackgroundSetter>& backgroundSetter) {
    backSetter = backgroundSetter;
}

void DirectoryController::toggleDots() {
    hideDots = !hideDots;
    if (hideDots) {
        hideDotEntries();
    }
    else {
        refreshShownEntries();
        sortShownEntries();
    }
}

void DirectoryController::toggleRelativePath() {
    useRelativePath = !useRelativePath;
    setDirectoryName();
}

const std::vector<int>& DirectoryController::getFoundEntries() const {
    return directorySearcher.getFoundEntries();
}

int DirectoryController::findAllEntriesInDirectory(const std::string searchTerm) {
    directorySearcher.setSearchTerm(searchTerm);
    return directorySearcher.findString(shownEntries);
}

void DirectoryController::chooseNextFoundEntry() {
    int idx = directorySearcher.chooseNext();
    if (idx >= 0) {
        messagePrinter->setMessage(getPathAt(idx).filename());
        observer->onSearchChange(idx);
    }
}

void DirectoryController::choosePreviousFoundEntry() {
    int idx = directorySearcher.choosePrevious();
    if (idx >= 0) {
        messagePrinter->setMessage(getPathAt(idx).filename());
        observer->onSearchChange(idx);
    }
}

int DirectoryController::getNumberOfEntries() const {
    return shownEntries.size();
}

const std::string& DirectoryController::getDirectoryName() const {
    return directoryName;
}

const fs::path& DirectoryController::getWorkpath() const {
    return directory.getWorkpath();
}

const fs::path& DirectoryController::getPathAt(int idx) const {
    return shownEntries[idx]->path();
}

const fs::directory_entry& DirectoryController::getEntryAt(int idx) const {
    return *shownEntries[idx];
}

const std::vector<const fs::directory_entry*>& DirectoryController::getAllEntries() const {
    return shownEntries;
}

std::vector<fs::path> DirectoryController::getAllImages() const {
    std::vector<fs::path> images;
    for (const auto &entry : shownEntries) {
        if (isAnImage(entry->path())) {
            images.push_back(entry->path());
        }
    }
    return images;
}

bool DirectoryController::isAnImage(int idx) const {
    return isAnImage(shownEntries[idx]->path());
}

bool DirectoryController::inFoundEntries(int idx) const {
    return std::find(getFoundEntries().begin(), getFoundEntries().end(), idx) != getFoundEntries().end();
}

void DirectoryController::clearSearchResults() {
    directorySearcher.clearSearchResults();
}
