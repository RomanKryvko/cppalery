#pragma once

#include "messagePrinter.h"
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

class DirectorySearcher {
private:
    std::shared_ptr<MessagePrinter> messagePrinter;
    std::string searchTerm;
    std::vector<int> foundEntries;
    int chosenEntryIdx = -1;

    void copyFromOther(const DirectorySearcher& other);

    static bool inString(std::string haystack, std::string needle) {
        std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
        std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

        return haystack.find(needle) != std::string::npos;
    }

public:
    DirectorySearcher();
    DirectorySearcher(const std::shared_ptr<MessagePrinter>& messagePrinter);
    DirectorySearcher(const DirectorySearcher& other);
    DirectorySearcher& operator=(const DirectorySearcher& other);

    int chooseNext();
    int choosePrevious();
    int getChosenIdx() const;
    const std::vector<int>& getFoundEntries() const;
    void setSearchTerm(const std::string& str);
    int findString(const std::vector<const std::filesystem::directory_entry*>& directoryEntries);
    void clearSearchResults();
};
