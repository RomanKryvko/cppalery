#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "directory.h"
#include "window.h"
#include <cstring>

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
        int ceiling;
        void printColoredString(const char* str, int y, int x, TermColors color);

    public:
        bool showPreview;

        Directory* directory;

        MainWindow();

        MainWindow(int height, int width, Directory* directory);

        MainWindow(const MainWindow& other);

        MainWindow& operator=(const MainWindow& other);

        void scrollUp();

        void scrollDown();

        void focusScrolling();

        void resetSetup() override;

        void jumpToEntry(int idx);

        void printDirectoryContents();

        std::string chooseNextFoundEntry(bool orderAsc);
};

#endif
