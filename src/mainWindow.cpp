#include "mainWindow.h"
#include "directory.h"
#include "window.h"
#include <algorithm>
#include <ncurses.h>
#include <vector>

MainWindow::MainWindow() {}

MainWindow::MainWindow(int height, int width, Directory* directory) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, 1, 1);
    this->directory = directory;
    ceiling = 0;
}

MainWindow::MainWindow(const MainWindow& other) {
    ceiling = other.ceiling;
    showPreview = other.showPreview;
    directory = other.directory;
}

MainWindow& MainWindow::operator=(const MainWindow& other) {
    Window::operator=(other);
    ceiling = other.ceiling;
    showPreview = other.showPreview;
    directory = other.directory;

    return *this;
}

void MainWindow::scrollUp() {
    if (directory->selection > 0) {
        directory->selection--;
    }
    if (ceiling > 0 && directory->selection < ceiling + SCROLL_OFFSET) {
        ceiling--;
    }
}

void MainWindow::scrollDown() {
    int selection = directory->selection;
    int directorySize = directory->size();
    int maxDisplayedIdx = ceiling + height - 2;
    int floor = (directorySize > maxDisplayedIdx) ? maxDisplayedIdx : directorySize;

    if (selection < directorySize - 1) {
        directory->selection++;
    }
    if (ceiling < directorySize - 1 && selection >= floor - SCROLL_OFFSET && floor != directorySize) {
        ceiling++;
    }
}

void MainWindow::focusScrolling() {
    int selection = directory->selection;
    int directorySize = directory->size();
    // The first entry is selected OR all contents fit within a single frame
    // OR selection is among the first entries, i.e. no scroll required
    if (selection == 0 || (directorySize > 0 && directorySize  < height - 2) || (selection < height / 2)) {
        ceiling = 0;
        return;
    }

    // The selected entry is in range between centre and bottom
    if (selection + height / 2 >= directorySize) {
        ceiling = directorySize - height + 2;
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
    directory->selection = idx;
    focusScrolling();
}


void MainWindow::printColoredString(const char* str, int y, int x, TermColors color) {
    wattron(this->window, COLOR_PAIR(color));
    mvwprintw(this->window, y, x, str);
    wattroff(this->window, COLOR_PAIR(color));
}

void MainWindow::printDirectoryContents() {
    resetSetup();
    int dirSize = this->directory->size();
    int selection = this->directory->selection;
    int floor = std::min(ceiling + height - 2, dirSize);

    std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");
    mvwprintw(this->window, this->height - 1, this->width - strlen(filesStr.c_str()) - 2, filesStr.c_str());

    mvwprintw(this->window, 0, 20, directory->directoryName.c_str());

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
        std::string entryStr = this->directory->contents[i].path().filename();
        if (showPreview && this->directory->isSelectionAnImage() && entryStr.length() > width / 2 - 1) {
            entryStr = entryStr.substr(0, width / 2 - 2).append("~");
        }
        else if (entryStr.length() > width - 2) {
            entryStr = entryStr.substr(0, width - 3).append("~");
        }
        const char *entryCStr = entryStr.c_str();

        if (i == selection) {
            if (this->directory->contents[selection].is_directory()) {
                printColoredString(entryCStr, j, 1, TermColors::SelectedDirColor);
                continue;
            }
            else {
                printColoredString(entryCStr, j, 1, TermColors::SelectedColor);
                continue;
            }
        }
        else if (std::find(directory->foundEntries.begin(), directory->foundEntries.end(), i) != directory->foundEntries.end()) {
            printColoredString(entryCStr, j, 1, TermColors::SearchHighlightColor);
            continue;
        }
        else if (this->directory->contents[i].is_directory()) {
            printColoredString(entryCStr, j, 1, TermColors::DirColor);
            continue;
        }
        else if (this->directory->isAnImage(i)) {
            printColoredString(entryCStr, j, 1, TermColors::ImageColor);
            continue;
        }
        else {
            mvwprintw(this->window, j, 1, entryCStr);
        }
    }

    wrefresh(this->window);
}

std::string MainWindow::chooseNextFoundEntry(bool orderAsc) {
    int currentIdx = directory->chosenFoundEntryIdx;
    std::string res = "";
    if (currentIdx >= 0) {
        if (orderAsc) {
            if (currentIdx == directory->foundEntries.size() - 1) {
                directory->chosenFoundEntryIdx = 0;
                res = "Search hit BOTTOM, starting from TOP";
            }
            else {
                directory->chosenFoundEntryIdx++;
                res = directory->contents[directory->foundEntries[directory->chosenFoundEntryIdx]].path().filename().string();
            }
        }
        else {
            if (currentIdx == 0) {
                directory->chosenFoundEntryIdx = directory->foundEntries.size() - 1;
                res = "Search hit TOP, starting from BOTTOM";
            }
            else {
                directory->chosenFoundEntryIdx--;
                res = directory->contents[directory->foundEntries[directory->chosenFoundEntryIdx]].path().filename().string();
            }
        }
        jumpToEntry(directory->foundEntries[directory->chosenFoundEntryIdx]);
    }
    return res;
}
