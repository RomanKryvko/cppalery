#pragma once

#include <filesystem>
#include <vector>

class IDirectoryController {
public:
    virtual const std::filesystem::path& getWorkpath() const = 0;
    virtual const std::filesystem::path& getPathAt(int idx) const = 0;
    virtual int findIdxOfEntry(const std::filesystem::path& entry) const = 0;
    virtual int getNumberOfEntries() const = 0;

    virtual std::vector<std::filesystem::path> getAllImages() const = 0;
    virtual bool isAnImage(int idx) const = 0;
};
