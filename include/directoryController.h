#pragma once

#include "IDirectoryObserver.h"
#include "backgroundSetter.h"
#include "directory.h"
#include "directorySearcher.h"
#include "IDirectoryController.h"
#include "messagePrinter.h"
#include <filesystem>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

class DirectoryController : public IDirectoryController {
private:
    Directory directory;
    DirectorySearcher directorySearcher;
    std::vector<const fs::directory_entry*> shownEntries;
    std::shared_ptr<IDirectoryObserver> observer;
    std::shared_ptr<MessagePrinter> messagePrinter;
    std::shared_ptr<BackgroundSetter> backSetter;
    fs::path homePath;
    std::string directoryName;
    bool hideDots;
    bool sortInAscendingOrder;
    bool useRelativePath;

    void refreshShownEntries();
    void formatShownEntries();
    void sortShownEntries();
    void hideDotEntries();
    void setDirectoryName();
    void doFullDirectorySetup();
    void copyFromOther(const DirectoryController& other);

    static bool caseInsensitiveCompare(const fs::directory_entry* a, const fs::directory_entry* b) {
        std::string aName = a->path().filename().string();
        std::string bName = b->path().filename().string();

        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

        return aName < bName;
    }

    static bool caseInsensitiveCompareDesc(const fs::directory_entry* a, const fs::directory_entry* b) {
        std::string aName = a->path().filename().string();
        std::string bName = b->path().filename().string();

        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

        return aName > bName;
    }

public:
    static bool isAnImage(const fs::path& path) {
        return std::count(IMG_EXTENSIONS.begin(), IMG_EXTENSIONS.end(), path.extension());
    }

    DirectoryController(const fs::path workpath, bool hideDots, bool sortAscending, bool useRelativePath, const std::shared_ptr<IDirectoryObserver>& observer, const std::shared_ptr<MessagePrinter>& messagePrinter);
    DirectoryController();
    DirectoryController(const DirectoryController& other);
    DirectoryController& operator=(const DirectoryController& other);

    void setBackgroundSetter(const std::shared_ptr<BackgroundSetter>& backgroundSetter);

    void goIntoDirectory(int idx);
    void goUpDirectory();

    void toggleDots();
    void toggleRelativePath();
    void sortByAscending();
    void sortByDescending();

    int findAllEntriesInDirectory(const std::string searchTerm);
    void chooseNextFoundEntry();
    void choosePreviousFoundEntry();

    virtual int findIdxOfEntry(const fs::path &path) const override;
    virtual int getNumberOfEntries() const override;
    virtual const fs::path& getWorkpath() const override;
    virtual const fs::path& getPathAt(int idx) const override;

    virtual bool isAnImage(int idx) const override;
    virtual std::vector<fs::path> getAllImages() const override;

    const std::vector<int>& getFoundEntries() const;
    const std::string& getDirectoryName() const;
    const fs::directory_entry& getEntryAt(int idx) const;
    const std::vector<const fs::directory_entry*>& getAllEntries() const;

    bool inFoundEntries(int idx) const;

    void clearSearchResults();
};
