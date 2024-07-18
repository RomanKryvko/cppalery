#include "mainWindow.h"
#include <algorithm>
#include <ncurses.h>
#include <vector>

MainWindow::MainWindow() {}

MainWindow::MainWindow(const std::string &workPath, int height, int width) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, 1, 1);
    this->directory = Directory(workPath);
    ceiling = 0;
}

// A workaround for a constructor
void MainWindow::initialize(const std::string &workPath, int height, int width) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, 1, 1);
    this->directory = Directory(workPath);
    ceiling = 0;
}

Directory MainWindow::getDirectory() {
    return this->directory;
}

int MainWindow::getDirSize() {
    return this->directory.size();
}

int MainWindow::getDirPosition() {
    return this->directory.selection;
}

void MainWindow::scrollUp() {
    int selection = directory.selection;
    if (selection > 0) {
        directory.selection--;
    }
    if (ceiling > 0 && selection < ceiling + SCROLL_OFFSET) {
        ceiling--;
    }
}

void MainWindow::scrollDown() {
    int selection = directory.selection;
    int dirSize = directory.size();
    int maxDisplayedIdx = ceiling + height - 2;
    int floor = (dirSize > maxDisplayedIdx) ? maxDisplayedIdx : dirSize;

    if (selection < dirSize - 1) {
        directory.selection++;
    }
    if (ceiling < dirSize - 1 && selection >= floor - SCROLL_OFFSET && floor != dirSize) {
        ceiling++;
    }
}

void MainWindow::focusScrolling() {
    int selection = directory.selection;
    int dirSize = directory.size();
    // The first entry is selected OR all contents fit within a single frame
    // OR selection is among the first entries, i.e. no scroll required
    if (selection == 0 || (dirSize > 0 && dirSize < height - 2) || (selection < height / 2)) {
        ceiling = 0;
        return;
    }

    // The selected entry is in range between centre and bottom
    if (selection + height / 2 >= dirSize) {
        ceiling = dirSize - height + 2;
        return;
    }

    // The selected entry is in the middle of contents
    int correctedCeiling = selection - height / 2;
    ceiling = (correctedCeiling > 0) ? correctedCeiling : selection - 1;
}

void MainWindow::resetSetup() {
    werase(this->window);
    box(this->window, 0, 0);
    mvwprintw(this->window, 0, 5, " CPPalery ");
}

void MainWindow::jumpToEntry(int idx) {
    directory.selection = idx;
    focusScrolling();
}

bool MainWindow::goUpDirectory() {
    if (directory.goUpDirectory()) {
        focusScrolling();
        return true;
    }
    return false;
}

bool MainWindow::goIntoDirectory() {
    if (directory.goIntoDirectory()) {
        focusScrolling();
        return true;
    }
    return false;
}

bool MainWindow::isSelectionAnImage() {
    return directory.isSelectionAnImage();
}

std::string MainWindow::getCurrentFilePath() {
    return this->directory.contents[directory.selection].path().string();
}

void MainWindow::printColoredString(const char* str, int y, int x, TermColors color) {
    wattron(this->window, COLOR_PAIR(color));
    mvwprintw(this->window, y, x, str);
    wattroff(this->window, COLOR_PAIR(color));
}

void MainWindow::printDirectoryContents() {
    resetSetup();
    int dirSize = this->directory.size();
    int selection = this->directory.selection;
    int floor = std::min(ceiling + height - 2, dirSize);

    std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");
    mvwprintw(this->window, this->height - 1, this->width - strlen(filesStr.c_str()) - 2, filesStr.c_str());

    mvwprintw(this->window, 0, 20, directory.directoryName.c_str());

    if (selection == 0) {
        mvwprintw(this->window, height - 1, 1, "TOP");
    }
    if (selection == dirSize - 1) {
        mvwprintw(this->window, height - 1, 1, "BOT");
    }

    if (dirSize == 0) {
        printColoredString("Empty", 1, 1, TermColors::EmptyColor);
        wrefresh(this->window);
        return;
    }

    for (int i = ceiling, j = 1; i < floor; i++, j++) {
        std::string entryStr = this->directory.contents[i].path().filename();
        if (isSelectionAnImage() && entryStr.length() > width / 2 - 1) {
            entryStr = entryStr.substr(0, width / 2 - 2).append("~");
        }
        else if (entryStr.length() > width - 2) {
            entryStr = entryStr.substr(0, width - 3).append("~");
        }
        const char *entryCStr = entryStr.c_str();

        if (i == selection) {
            if (this->directory.contents[selection].is_directory()) {
                printColoredString(entryCStr, j, 1, TermColors::SelectedDirColor);
                continue;
            }
            else {
                printColoredString(entryCStr, j, 1, TermColors::SelectedColor);
                continue;
            }
        }
        else if (std::find(directory.foundEntries.begin(), directory.foundEntries.end(), i) != directory.foundEntries.end()) {
            printColoredString(entryCStr, j, 1, TermColors::SearchHighlightColor);
            continue;
        }
        else if (this->directory.contents[i].is_directory()) {
            printColoredString(entryCStr, j, 1, TermColors::DirColor);
            continue;
        }
        else if (this->directory.isAnImage(i)) {
            printColoredString(entryCStr, j, 1, TermColors::ImageColor);
            continue;
        }
        else {
            mvwprintw(this->window, j, 1, entryCStr);
        }
    }

    wrefresh(this->window);
}

void MainWindow::toggleDots() {
    directory.hideDots = !directory.hideDots;
    if (!directory.hideDots) {
        directory.refreshDirectoryContents();
    }
    directory.formatDir();
    focusScrolling();
}

void MainWindow::toggleRelativePath() {
    directory.relativePath = !directory.relativePath;
    directory.setDirectoryName();
}

void MainWindow::sortContentsByName(bool ascending) {
    directory.nameAsc = ascending;
    directory.formatDir();
}

std::vector<fs::path> MainWindow::getAllImages() {
    return directory.getAllImages();
}

std::string MainWindow::chooseNextFoundEntry(bool orderAsc) {
    int currentIdx = directory.chosenFoundEntryIdx;
    std::string res = "";
    if (currentIdx >= 0) {
        if (orderAsc) {
            if (currentIdx == directory.foundEntries.size() - 1) {
                directory.chosenFoundEntryIdx = 0;
                res = "Search hit BOTTOM, starting from TOP";
            }
            else {
                directory.chosenFoundEntryIdx++;
                res = directory.contents[directory.foundEntries[directory.chosenFoundEntryIdx]].path().filename().string();
            }
        }
        else {
            if (currentIdx == 0) {
                directory.chosenFoundEntryIdx = directory.foundEntries.size() - 1;
                res = "Search hit TOP, starting from BOTTOM";
            }
            else {
                directory.chosenFoundEntryIdx--;
                res = directory.contents[directory.foundEntries[directory.chosenFoundEntryIdx]].path().filename().string();
            }
        }
        jumpToEntry(directory.foundEntries[directory.chosenFoundEntryIdx]);
    }
    return res;
}

std::string MainWindow::findEntryInDirectory(const std::string &str) {
    directory.findAllEntriesInDirectory(str);
    if (directory.foundEntries.empty()) {
        std::string res = "Pattern not found: ";
        res.append(str);
        return res;
    }
    int firstFoundEntry = directory.foundEntries[0];
    jumpToEntry(firstFoundEntry);
    int numberFound = directory.foundEntries.size();
    std::string res;
    if (numberFound == 1) {
        res = str + ": " + std::to_string(numberFound) + " match";
    }
    else {
        res = str + ": " + std::to_string(numberFound) + " matches";
    }
    return res;
}
