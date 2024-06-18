#include "cppalery.h"
#include "directory.h"
#include "window.h"

class MainWindow : public Window {
    private:
        Directory directory;
        int ceiling;

    public:
        MainWindow() {}

        MainWindow(const std::string &workPath, int height, int width) {
            this->height = height;
            this->width = width;
            this->window = newwin(this->height, this->width, 1, 1);
            this->directory = Directory(workPath);
            ceiling = 0;
        }

        // A workaround for a constructor
        void initialize(const std::string &workPath, int height, int width) {
            this->height = height;
            this->width = width;
            this->window = newwin(this->height, this->width, 1, 1);
            this->directory = Directory(workPath);
            ceiling = 0;
        }

        Directory getDirectory() {
            return this->directory;
        }

        void scrollUp() {
            int selection = directory.selection;
            if (selection > 0) {
                directory.selection--;
            }
            if (ceiling > 0 && selection < ceiling + SCROLL_OFFSET) {
                ceiling--;
            }
        }

        void scrollDown() {
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

        void focusScrolling() {
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

        virtual void resetSetup() {
            werase(this->window);
            box(this->window, 0, 0);
            mvwprintw(this->window, 0, 5, " CPPalery ");
            wrefresh(this->window);
        }

        void resize(int newHeight, int newWidth) {
            this->height = newHeight;
            this->width = newWidth;
            wresize(window, newHeight, newWidth);
            resetSetup();
        }

        bool goUpDirectory() {
            if (directory.goUpDirectory()) {
                focusScrolling();
                return true;
            }
            return false;
        }

        bool goIntoDirectory() {
            if (directory.goIntoDirectory()) {
                ceiling = 0;
                return true;
            }
            return false;
        }

        bool isSelectionAnImage() {
            return directory.isSelectionAnImage();
        }

        std::string getCurrentFilePath() {
            return this->directory.contents[directory.selection].path().string();
        }

        void printDirectoryContents() {
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

            for (int i = ceiling, j = 1; i < floor; i++, j++) {
                const std::string entryStr = this->directory.contents[i].path().filename();
                const char *entryCStr = entryStr.c_str();

                if (i == selection) {
                    wattron(this->window, COLOR_PAIR(TermColors::SelectedColor));
                    mvwprintw(this->window, j, 1, entryCStr);
                    wattroff(this->window, COLOR_PAIR(TermColors::SelectedColor));
                }
                else if (this->directory.contents[i].is_directory()) {
                    wattron(this->window, COLOR_PAIR(TermColors::DirColor));
                    mvwprintw(this->window, j, 1, entryCStr);
                    wattroff(this->window, COLOR_PAIR(TermColors::DirColor));
                }
                else {
                    mvwprintw(this->window, j, 1, entryCStr);
                }
            }

            wrefresh(this->window);
        }

        void toggleDots() {
            directory.hideDots = !directory.hideDots;
            if (!directory.hideDots) {
                directory.refreshDirectoryContents();
            }
            directory.formatDir();
            focusScrolling();
        }

        void sortContentsByName(bool ascending = true) {
            directory.nameAsc = ascending;
            directory.formatDir();
        }

        void chooseNextFoundEntry() {
            int currentIdx = directory.chosenFoundEntryIdx;
            if (currentIdx >= 0) {
                if (currentIdx == directory.foundEntries.size() - 1) {
                    //TODO: output "Search hit BOTTOM, starting from TOP message"
                    directory.chosenFoundEntryIdx = 0;
                }
                else {
                    directory.chosenFoundEntryIdx++;
                }
                directory.selection = directory.foundEntries[directory.chosenFoundEntryIdx];
                focusScrolling();
            }
        }

        std::string findEntryInDirectory(const std::string &str) {
            directory.findAllEntriesInDirectory(str);
            if (directory.foundEntries.empty()) {
                std::string res = "Pattern not found: ";
                res.append(str);
                return res;
            }
            int firstFoundEntry = directory.foundEntries[0];
            directory.selection = firstFoundEntry;
            focusScrolling();
            return directory.contents[firstFoundEntry].path().filename().string();
        }

};