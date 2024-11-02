#include "mainWindow.h"
#include "colors.h"
#include "directory.h"
#include "window.h"

MainWindow::MainWindow() {}

MainWindow::MainWindow(int height, int width, const std::shared_ptr<Directory> &directory) : Window(height, width) {
    this->window = newwin(height, width, 1, 1);
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

    if (directorySize <= height - 2) {
        ceiling = 0;
        return;
    }

    // Prevent blank space from appearing when focusing on entries at the bottom of directory
    if (selection + height / 2 >= directorySize) {
        ceiling = directorySize - height + 2;
        return;
    }

    int correctedCeiling = selection - height / 2;
    ceiling = std::max(correctedCeiling, 0);
}

void MainWindow::resetSetup() {
    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 5, " CPPalery ");
}

void MainWindow::jumpToEntry(int idx) {
    directory->selection = idx;
    focusScrolling();
}


void MainWindow::printColoredString(const char* str, int y, int x, ColorPair pair) {
    wattron(window, COLOR_PAIR(pair));
    mvwprintw(window, y, x, str);
    wattroff(window, COLOR_PAIR(pair));
}

void MainWindow::printDirectoryContents() {
    resetSetup();
    int dirSize = directory->size();
    int selection = directory->selection;
    int floor = std::min(ceiling + height - 2, dirSize);

    std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");
    mvwprintw(window, height - 1, width - strlen(filesStr.c_str()) - 2, filesStr.c_str());

    mvwprintw(window, 0, 20, directory->directoryName.c_str());

    if (selection == 0) {
        mvwprintw(window, height - 1, 1, "TOP");
    }
    if (selection == dirSize - 1) {
        mvwprintw(window, height - 1, 1, "BOT");
    }

    if (dirSize == 0) {
        printColoredString("Empty", 1, 1, ColorPair::Empty);
        wrefresh(window);
        return;
    }

    for (int i = ceiling, j = 1; i < floor; i++, j++) {
        std::string entryStr = directory->contents[i].path().filename();
        if (showPreview && directory->isSelectionAnImage() && entryStr.length() > width / 2 - 1) {
            entryStr = entryStr.substr(0, width / 2 - 2).append("~");
        }
        else if (entryStr.length() > width - 2) {
            entryStr = entryStr.substr(0, width - 3).append("~");
        }
        const char *entryCStr = entryStr.c_str();

        if (i == selection) {
            if (directory->contents[selection].is_directory()) {
                printColoredString(entryCStr, j, 1, ColorPair::SelectedDirectory);
                continue;
            }
            else {
                printColoredString(entryCStr, j, 1, ColorPair::Selected);
                continue;
            }
        }
        else if (std::find(directory->foundEntries.begin(), directory->foundEntries.end(), i) != directory->foundEntries.end()) {
            printColoredString(entryCStr, j, 1, ColorPair::SearchHighlight);
            continue;
        }
        else if (directory->contents[i].is_directory()) {
            printColoredString(entryCStr, j, 1, ColorPair::Dir);
            continue;
        }
        else if (directory->isAnImage(i)) {
            printColoredString(entryCStr, j, 1, ColorPair::Image);
            continue;
        }
        else {
            mvwprintw(window, j, 1, entryCStr);
        }
    }

    wrefresh(window);
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
