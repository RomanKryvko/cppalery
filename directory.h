#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "cppalery.h"

class Directory {
    private:
        int dirSize;
        fs::path directoryOfEntry;

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
        int selection;
        std::vector<fs::directory_entry> contents;
        std::string directoryName;
        std::vector<int> foundEntries;
        int chosenFoundEntryIdx;

        Directory(const std::string &workPath);

        Directory();

        int size();

        bool isSelectionAnImage();

        bool goUpDirectory();

        bool goIntoDirectory();

        void refreshDirectoryContents();

        void formatDir();

        int findIdxOfEntry(const fs::path &path);

        void findAllEntriesInDirectory(const std::string &str);
};

#endif