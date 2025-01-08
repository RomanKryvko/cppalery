#ifndef DIRECTORY_CONTROLLER_H
#define DIRECTORY_CONTROLLER_H

#include "directory.h"
#include "directorySearcher.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class DirectoryController {
private:
    Directory directory;
    DirectorySearcher directorySearcher;
    std::vector<const fs::directory_entry*> shownEntries;
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
        return count(imgExtensions.begin(), imgExtensions.end(), path.extension());
    }

    DirectoryController(const fs::path workpath, bool hideDots, bool sortAscending, bool useRelativePath);
    DirectoryController();
    DirectoryController(const DirectoryController& other);
    DirectoryController& operator=(const DirectoryController& other);

    bool goIntoDirectory(int idx);
    void goUpDirectory();

    void toggleDots();
    void toggleRelativePath();
    void sortByAscending();
    void sortByDescending();

    int findAllEntriesInDirectory(const std::string searchTerm);
    int getNextFoundEntry();
    int getPreviousFoundEntry();

    const std::vector<int>& getFoundEntries() const;
    int getNumberOfEntries() const;
    const std::string& getDirectoryName() const;
    const fs::path& getWorkpath() const;
    const fs::path& getPathAt(int idx) const;
    const fs::directory_entry& getEntryAt(int idx) const;
    const std::vector<const fs::directory_entry*>& getAllEntries() const;
    int findIdxOfEntry(const fs::path &path) const;

    bool inFoundEntries(int idx) const;

    bool isAnImage(int idx) const;
    std::vector<fs::path> getAllImages() const;

    void clearSearchResults();
};

#endif
