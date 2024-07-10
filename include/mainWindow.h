#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "directory.h"
#include "window.h"
#include <cstring>
#include <string>

enum TermColors
{
    DefaultColor = 0,
    DirColor = 1,
    SelectedColor = 2,
    ImageColor = 3,
    SelectedDirColor = 4,
    EmptyColor = 5,
    SearchHighlightColor = 6
};

// Defines on which line from the egde scrolling starts
const int SCROLL_OFFSET = 2;

class MainWindow : public Window {
    private:
        Directory directory;
        int ceiling;
        void printColoredString(const char* str, int y, int x, TermColors color);

    public:
        MainWindow();

        MainWindow(const std::string &workPath, int height, int width);

        // A workaround for a constructor
        void initialize(const std::string &workPath, int height, int width);

        Directory getDirectory();

        int getDirPosition();

        int getDirSize();

        void scrollUp();

        void scrollDown();

        void focusScrolling();

        void resetSetup() override;

        bool goUpDirectory();

        bool goIntoDirectory();

        void jumpToEntry(int idx);

        bool isSelectionAnImage();

        std::string getCurrentFilePath();

        void printDirectoryContents();

        void toggleDots();

        void toggleRelativePath();

        void sortContentsByName(bool ascending = true);

        std::vector<fs::path> getAllImages();

        std::string chooseNextFoundEntry(bool orderAsc);

        std::string findEntryInDirectory(const std::string &str);

};

#endif
