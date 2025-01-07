#include "mainWindow.h"

MainWindow::MainWindow() {}

MainWindow::MainWindow(int height, int width) : Window(height, width) {
    this->window = newwin(height, width, 1, 1);
}

MainWindow::MainWindow(const MainWindow& other) {
    showPreview = other.showPreview;
}

MainWindow& MainWindow::operator=(const MainWindow& other) {
    Window::operator=(other);
    showPreview = other.showPreview;

    return *this;
}

void MainWindow::printColoredString(const char* str, int y, int x, ColorPair pair) {
    wattron(window, COLOR_PAIR(pair));
    mvwprintw(window, y, x, "%s", str);
    wattroff(window, COLOR_PAIR(pair));
}

void MainWindow::resetSetup() {
    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 5, " CPPalery ");
}

void MainWindow::printDirectoryContents(const Pager& pager, const std::shared_ptr<DirectoryController>& dirController) {
    resetSetup();
    int floor = std::min(pager.getMinDisplayedIdx() + height - 2, pager.getNumberOfElements());

    std::string filesStr = std::string(" Entries total: ").append(std::to_string(pager.getNumberOfElements())).append(" ");
    mvwprintw(window, height - 1, width - filesStr.length() - 2, "%s", filesStr.c_str());

    mvwprintw(window, 0, 20, "%s", dirController->getDirectoryName().c_str());

    if (pager.getSelection() == 0) {
        mvwprintw(window, height - 1, 1, "TOP");
    }
    if (pager.getSelection() == pager.getNumberOfElements() - 1) {
        mvwprintw(window, height - 1, 1, "BOT");
    }

    if (!dirController->getNumberOfEntries()) {
        printColoredString("Empty", 1, 1, ColorPair::Empty);
        wrefresh(window);
        return;
    }

    for (int i = pager.getMinDisplayedIdx(), j = 1; i < floor; i++, j++) {
        const bool isSelectionAnImage = dirController->isAnImage(i);
        std::string entryStr = dirController->getPathAt(i).filename();

        if (showPreview && isSelectionAnImage && entryStr.length() > width / 2 - 1) {
            entryStr = truncateString(entryStr, width / 2 - 2);
        }
        else if (entryStr.length() > width - 2) {
            entryStr = truncateString(entryStr, width - 3);
        }

        const char *entryCStr = entryStr.c_str();

        if (i == pager.getSelection()) {
            if (dirController->getEntryAt(pager.getSelection()).is_directory()) {
                printColoredString(entryCStr, j, 1, ColorPair::SelectedDirectory);
                continue;
            }
            else {
                printColoredString(entryCStr, j, 1, ColorPair::Selected);
                continue;
            }
        }
        else if (dirController->inFoundEntries(i)) {
            printColoredString(entryCStr, j, 1, ColorPair::SearchHighlight);
            continue;
        }
        else if (dirController->getEntryAt(i).is_directory()) {
            printColoredString(entryCStr, j, 1, ColorPair::Directory);
            continue;
        }
        else if (isSelectionAnImage) {
            printColoredString(entryCStr, j, 1, ColorPair::Image);
            continue;
        }
        else {
            mvwprintw(window, j, 1, "%s", entryCStr);
        }
    }

    wrefresh(window);
}
