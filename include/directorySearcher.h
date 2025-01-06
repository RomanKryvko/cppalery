#ifndef DIRECTORY_SEARCHER_H
#define DIRECTORY_SEARCHER_H

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

class DirectorySearcher {
private:
    std::string searchTerm;
    std::vector<int> foundEntries;
    int chosenEntryIdx = -1;

    static bool inString(std::string haystack, std::string needle) {
        std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
        std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

        return haystack.find(needle) != std::string::npos;
    }

public:
    DirectorySearcher();
    int chooseNext();
    int choosePrevious();
    int getChosenIdx() const;
    const std::vector<int>& getFoundEntries() const;
    void setSearchTerm(const std::string& str);
    int findString(const std::vector<const std::filesystem::directory_entry*>& directoryEntries);
    void clearSearchResults();
};

#endif
