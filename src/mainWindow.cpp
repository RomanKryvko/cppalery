#include "mainWindow.h"

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
    if (selection == 0) {
        ceiling = selection;
        return;
    }
    int correctedCeiling = selection - height / 2;
    if (correctedCeiling > 0) {
        ceiling = correctedCeiling;
    }
    else {
        ceiling = selection - 1;
    }
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
        ceiling = 0;
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

void MainWindow::printDirectoryContents() {
    resetSetup();
    int dirSize = this->directory.size();
    int selection = this->directory.selection;
    int maxDisplayedIdx = ceiling + height - 2;
    int floor = (dirSize > maxDisplayedIdx) ? maxDisplayedIdx : dirSize;

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
        wattron(this->window, COLOR_PAIR(TermColors::EmptyColor));
        mvwprintw(this->window, 1, 1, "Empty");
        wattroff(this->window, COLOR_PAIR(TermColors::EmptyColor));
        wrefresh(this->window);
        return;
    }

    for (int i = ceiling, j = 1; i < floor; i++, j++) {
        const std::string entryStr = this->directory.contents[i].path().filename();
        const char *entryCStr = entryStr.c_str();

        if (i == selection) {
            if (this->directory.contents[selection].is_directory()) {
                wattron(this->window, COLOR_PAIR(TermColors::SelectedDirColor));
                mvwprintw(this->window, j, 1, entryCStr);
                wattroff(this->window, COLOR_PAIR(TermColors::SelectedDirColor));
            }
            else {
                wattron(this->window, COLOR_PAIR(TermColors::SelectedColor));
                mvwprintw(this->window, j, 1, entryCStr);
                wattroff(this->window, COLOR_PAIR(TermColors::SelectedColor));
            }
        }
        else if (this->directory.contents[i].is_directory()) {
            wattron(this->window, COLOR_PAIR(TermColors::DirColor));
            mvwprintw(this->window, j, 1, entryCStr);
            wattroff(this->window, COLOR_PAIR(TermColors::DirColor));
        }
        else if (this->directory.isAnImage(i)) {
            wattron(this->window, COLOR_PAIR(TermColors::ImageColor));
            mvwprintw(this->window, j, 1, entryCStr);
            wattroff(this->window, COLOR_PAIR(TermColors::ImageColor));
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

void MainWindow::sortContentsByName(bool ascending) {
    directory.nameAsc = ascending;
    directory.formatDir();
}

std::string MainWindow::chooseNextFoundEntry() {
    int currentIdx = directory.chosenFoundEntryIdx;
    std::string res = "";
    if (currentIdx >= 0) {
        if (currentIdx == directory.foundEntries.size() - 1) {
            directory.chosenFoundEntryIdx = 0;
            res = "Search hit BOTTOM, starting from TOP";
        }
        else {
            directory.chosenFoundEntryIdx++;
            res = directory.contents[directory.foundEntries[directory.chosenFoundEntryIdx]].path().filename().string();
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
    std::string res = str + ": " + std::to_string(directory.foundEntries.size()) + " matches";
    return res;
}