#ifndef DIRECTORY_PAGER_H
#define DIRECTOTY_PAGER_H

#include "pager.h"
#include <map>
#include <filesystem>
#include "IDirectoryObserver.h"

namespace fs = std::filesystem;

class DirectoryPager : public Pager , public IDirectoryObserver {
public:
    using PathMap = std::map<fs::path, fs::path>;
    DirectoryPager(int height, int numberOfElements);
    DirectoryPager(const DirectoryPager& other);
    DirectoryPager& operator=(const DirectoryPager& other);

    virtual void onDirectoryChanged(const IDirectoryController* directory) override;
    virtual void onDirectoryEntriesChanged(const IDirectoryController* directory) override;

private:
    PathMap directoryHistory;
};

#endif
