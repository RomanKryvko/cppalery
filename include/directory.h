#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

const std::vector<std::string> imgExtensions = {".jpg", ".webp", ".png", ".gif", ".jpeg"};

class Directory {
    private:
        int dirSize;

        fs::path childDirectoryOfEntry;

        fs::path homePath;

        void setupDirectory(const std::string &workPath);

        static bool caseInsensitiveCompare(const fs::directory_entry& a, const fs::directory_entry& b) {
            std::string aName = a.path().filename().string();
            std::string bName = b.path().filename().string();

            std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
            std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

            return aName < bName;
        }

        static bool caseInsensitiveCompareDesc(const fs::directory_entry& a, const fs::directory_entry& b) {
            std::string aName = a.path().filename().string();
            std::string bName = b.path().filename().string();

            std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
            std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

            return aName > bName;
        }

        bool inString(std::string haystack, std::string needle);

    public:
        fs::path workPath;
        bool hideDots;
        bool nameAsc;
        bool relativePath;
        int selection;
        std::vector<fs::directory_entry> contents;
        std::string directoryName;
        std::vector<int> foundEntries;
        int chosenFoundEntryIdx;

        Directory(const std::string &workPath, bool relativePath = true);

        Directory();

        int size();

        bool isSelectionAnImage();

        bool isAnImage(int idx);

        bool goUpDirectory();

        bool goIntoDirectory();

        void refreshDirectoryContents();

        void formatDir();

        void setDirectoryName();

        std::vector<fs::path> getAllImages();

        int findIdxOfEntry(const fs::path &path);

        void findAllEntriesInDirectory(const std::string &str);
};

#endif
