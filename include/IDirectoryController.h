#pragma once

#include <filesystem>

class IDirectoryController {
public:
    virtual const std::filesystem::path& getWorkpath() const = 0;
    virtual const std::filesystem::path& getPathAt(int idx) const = 0;
    virtual int findIdxOfEntry(const std::filesystem::path& entry) const = 0;
    virtual int getNumberOfEntries() const = 0;
};
