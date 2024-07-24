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

        fs::path workPath;

        void formatDir();

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

        void setDirectoryName();

        int findIdxOfEntry(const fs::path &path);

    public:
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

        void setupDirectory(const std::string &workPath);

        int size() const;

        bool empty() const;

        fs::path getSelectedFilePathString() const;

        int getSelection() const;

        fs::path getPath() const;

        void toggleRelativePath();

        void toggleDots();

        bool isSelectionAnImage();

        bool isAnImage(int idx);

        bool goUpDirectory();

        bool goIntoDirectory();

        void sortContentsByName(bool ascending);

        void refreshDirectoryContents();

        std::vector<fs::path> getAllImages();

        void clearSearchResults();

        int findAllEntriesInDirectory(const std::string &str);

        std::string chooseNextFoundEntry(bool orderaAsc);
};

#endif
